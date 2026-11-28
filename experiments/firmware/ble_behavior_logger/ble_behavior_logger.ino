/* BLE behavior logger: legacy GAP reports, without target fusion or ranging.
 * Target: Arduino Nano ESP32, Bluedroid-based Arduino ESP32 core.
 * Commands (newline terminated):
 * start SESSION TRIAL DEVICE CONDITION passive|active
 * stop
 * status
 * on / off: labelled 30-second passive trial, then automatic stop.
 * share / lock / unlock: correctly labelled 30-second manual trials.
 * Labels must use ASCII letters, digits, underscores or hyphens (1-40 chars).
 */
#include <Arduino.h>
#include <sdkconfig.h>

// This logger needs raw Bluedroid GAP reports. A NimBLE-only core is a
// different API, not a missing Arduino library that can be installed separately.
#if !defined(CONFIG_BLUEDROID_ENABLED)
#error "Select Arduino Nano ESP32 from Arduino ESP32 Boards by Arduino (2.0.18-arduino.5). This logger does not support NimBLE-only cores."
#else
#include <BLEDevice.h>
#include <esp_gap_ble_api.h>
#include <esp_system.h>
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>
#include <atomic>
#include <stdarg.h>
#include "BleAdParser.h"

constexpr uint16_t SCAN_INTERVAL_UNITS = 160; // 100 ms; GAP unit = 0.625 ms
constexpr uint16_t SCAN_WINDOW_UNITS = 128;   // 80 ms
constexpr uint32_t SCAN_DURATION_SECONDS = 600;
constexpr uint32_t HEARTBEAT_MS = 3000;
constexpr uint32_t STATE_TIMEOUT_MS = 5000;
constexpr uint32_t SERIAL_LINE_TIMEOUT_MS = 250;
constexpr size_t QUEUE_LENGTH = 128;
constexpr size_t MAX_REPORTS_PER_LOOP = 4;
constexpr size_t LINE_BUFFER_CAPACITY = 1280;
#if ARDUINO_USB_CDC_ON_BOOT && !ARDUINO_USB_MODE
constexpr size_t SERIAL_CHUNK_MAX = 48; // Stay strictly inside 64-byte TinyUSB CDC TX FIFO.
#else
constexpr size_t SERIAL_CHUNK_MAX = 256;
#endif

struct ScanReport {
  uint32_t timestampMs;
  uint32_t reportId;
  uint8_t mac[6];
  uint8_t addressType;
  int8_t rssi;
  uint8_t eventType;
  uint8_t advLength;
  uint8_t responseLength;
  uint8_t data[ESP_BLE_ADV_DATA_LEN_MAX + ESP_BLE_SCAN_RSP_DATA_LEN_MAX];
};

QueueHandle_t reportQueue = nullptr;
esp_ble_scan_params_t scanParams = {};
std::atomic<uint32_t> droppedReports(0);
std::atomic<uint32_t> reportCount(0);
std::atomic<int> paramsStatus(-1), startStatus(-1), stopStatus(-1);
std::atomic<bool> scanFinished(false);
bool trialOpen = false;
bool scanning = false;
bool stopping = false;
bool stopAfterStart = false;
bool serialLineDirty = false;
String sessionId, trialId, deviceLabel, condition, scanMode;
String commandBuffer;
bool commandTooLong = false;
uint32_t lastHeartbeatMs = 0;
uint32_t lastCommandByteMs = 0;
uint32_t trialOpenMs = 0;
uint32_t scanStartedMs = 0;
uint32_t stopRequestedMs = 0;
uint32_t quickTrialNumber = 0;
uint32_t scanDurationSeconds = SCAN_DURATION_SECONDS;
constexpr size_t DETAILS_CACHE_SIZE = 24;
ScanReport detailsCache[DETAILS_CACHE_SIZE];
size_t detailsCacheCount = 0;
size_t nextDetailsSlot = 0;
char lineBuf[LINE_BUFFER_CAPACITY];

// Only loop() writes Serial; the BLE callback copies reports into a bounded queue.
void gapCallback(esp_gap_ble_cb_event_t event, esp_ble_gap_cb_param_t* param) {
  if (event == ESP_GAP_BLE_SCAN_PARAM_SET_COMPLETE_EVT) {
    paramsStatus.store(param->scan_param_cmpl.status);
  } else if (event == ESP_GAP_BLE_SCAN_START_COMPLETE_EVT) {
    startStatus.store(param->scan_start_cmpl.status);
  } else if (event == ESP_GAP_BLE_SCAN_STOP_COMPLETE_EVT) {
    stopStatus.store(param->scan_stop_cmpl.status);
  } else if (event == ESP_GAP_BLE_SCAN_RESULT_EVT) {
    const auto& result = param->scan_rst;
    if (result.search_evt == ESP_GAP_SEARCH_INQ_CMPL_EVT) {
      scanFinished.store(true);
      return;
    }
    if (result.search_evt != ESP_GAP_SEARCH_INQ_RES_EVT) return;
    ScanReport report = {};
    report.timestampMs = millis();
    report.reportId = reportCount.fetch_add(1) + 1;
    if (result.adv_data_len > ESP_BLE_ADV_DATA_LEN_MAX ||
        result.scan_rsp_len > ESP_BLE_SCAN_RSP_DATA_LEN_MAX) {
      droppedReports.fetch_add(1);
      return;
    }
    memcpy(report.mac, result.bda, sizeof(report.mac));
    report.addressType = result.ble_addr_type;
    report.rssi = result.rssi;
    report.eventType = result.ble_evt_type;
    report.advLength = result.adv_data_len;
    report.responseLength = result.scan_rsp_len;
    memcpy(report.data, result.ble_adv, report.advLength + report.responseLength);
    if (xQueueSend(reportQueue, &report, 0) != pdTRUE) droppedReports.fetch_add(1);
  }
}

struct CsvBuilder {
  char* buf;
  size_t cap;
  size_t len;
  size_t cols;

  void reset(char* target, size_t capacity) {
    buf = target;
    cap = capacity;
    len = 0;
    cols = 0;
    if (cap) buf[0] = '\0';
  }

  void pushChar(char ch) {
    if (len + 1 < cap) {
      buf[len++] = ch;
      buf[len] = '\0';
    }
  }

  void beginField() {
    if (cols > 0) pushChar(',');
    pushChar('"');
    ++cols;
  }

  void endField() { pushChar('"'); }

  void fieldText(const char* text) {
    beginField();
    if (text) {
      for (const char* p = text; *p; ++p) {
        char ch = *p;
        if (ch == '"') pushChar('"');
        pushChar((ch == '\r' || ch == '\n') ? ' ' : ch);
      }
    }
    endField();
  }

  void fieldString(const String& s) { fieldText(s.c_str()); }

  void fieldU32(uint32_t value) {
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%lu", (unsigned long)value);
    fieldText(tmp);
  }

  void fieldI32(int32_t value) {
    char tmp[16];
    snprintf(tmp, sizeof(tmp), "%ld", (long)value);
    fieldText(tmp);
  }

  void fieldHex(const uint8_t* bytes, size_t length) {
    static const char digits[] = "0123456789ABCDEF";
    beginField();
    for (size_t i = 0; i < length; ++i) {
      pushChar(digits[bytes[i] >> 4]);
      pushChar(digits[bytes[i] & 15]);
    }
    endField();
  }

  void finishLine() {
    pushChar('\r');
    pushChar('\n');
  }

  void appendBase(const char* type, uint32_t timestamp) {
    fieldText(type);        // 0: record_type
    fieldText("1");         // 1: schema_version
    fieldString(sessionId); // 2: session_id
    fieldString(trialId);   // 3: trial_id
    fieldString(deviceLabel); // 4: device_label
    fieldString(condition); // 5: condition
    fieldString(scanMode);  // 6: scan_mode
    fieldU32(timestamp);    // 7: timestamp_ms
  }
};

// Write a complete line without entering USBCDC::write()'s unbounded busy-wait
// when the 64-byte TinyUSB TX FIFO is full or the host pauses reading.
bool writeSerialBufferSafe(const char* data, size_t length, bool countDropOnStall) {
  if (!Serial) {
    if (countDropOnStall) droppedReports.fetch_add(1);
    return false;
  }

  uint32_t startMs = millis();
  if (serialLineDirty) {
    while (Serial && Serial.availableForWrite() < 2) {
      if (millis() - startMs >= SERIAL_LINE_TIMEOUT_MS) {
        if (countDropOnStall) droppedReports.fetch_add(1);
        return false;
      }
      delay(1);
    }
    if (!Serial) {
      if (countDropOnStall) droppedReports.fetch_add(1);
      return false;
    }
    Serial.write("\r\n", 2);
    serialLineDirty = false;
    delay(1);
  }

  // Wait for initial space before starting a new line to avoid partial rows.
  while (Serial && Serial.availableForWrite() < 16) {
    if (millis() - startMs >= SERIAL_LINE_TIMEOUT_MS) {
      if (countDropOnStall) droppedReports.fetch_add(1);
      return false;
    }
    delay(1);
  }
  if (!Serial) {
    if (countDropOnStall) droppedReports.fetch_add(1);
    return false;
  }

  size_t offset = 0;
  while (offset < length) {
    if (!Serial) {
      serialLineDirty = (offset > 0);
      if (countDropOnStall) droppedReports.fetch_add(1);
      return false;
    }
    int avail = Serial.availableForWrite();
    if (avail <= 0) {
      if (millis() - startMs >= SERIAL_LINE_TIMEOUT_MS) {
        serialLineDirty = (offset > 0);
        if (countDropOnStall) droppedReports.fetch_add(1);
        return false;
      }
      delay(1);
      continue;
    }
    size_t remaining = length - offset;
    size_t chunk = remaining;
    if (chunk > (size_t)avail) chunk = (size_t)avail;
    if (chunk > SERIAL_CHUNK_MAX) chunk = SERIAL_CHUNK_MAX;
    size_t written = Serial.write(reinterpret_cast<const uint8_t*>(data + offset), chunk);
    if (written == 0) {
      if (millis() - startMs >= SERIAL_LINE_TIMEOUT_MS) {
        serialLineDirty = (offset > 0);
        if (countDropOnStall) droppedReports.fetch_add(1);
        return false;
      }
      delay(1);
      continue;
    }
    offset += written;
    // Yield 1 ms between chunks so arduino_usb_events drains its 5-slot TX event queue.
    if (offset < length) delay(1);
  }
  return true;
}

void emitEvent(const char* event, const char* detail) {
  CsvBuilder csv;
  csv.reset(lineBuf, sizeof(lineBuf));
  csv.appendBase("EVENT", millis());
  for (int col = 8; col <= 16; ++col) csv.fieldText("");
  csv.fieldText(event);                      // 17: event
  csv.fieldText(detail);                     // 18: detail
  csv.fieldU32(droppedReports.load());       // 19: dropped_reports
  csv.finishLine();
  writeSerialBufferSafe(csv.buf, csv.len, false);
}

void emitEvent(const char* event, const String& detail) {
  emitEvent(event, detail.c_str());
}

void emitAdDetails(const ScanReport& report, const char* mac) {
  for (size_t i = 0; i < detailsCacheCount; ++i) {
    const ScanReport& previous = detailsCache[i];
    if (memcmp(previous.mac, report.mac, 6) == 0 && previous.addressType == report.addressType &&
        previous.advLength == report.advLength && previous.responseLength == report.responseLength &&
        memcmp(previous.data, report.data, report.advLength + report.responseLength) == 0) return;
  }
  detailsCache[nextDetailsSlot] = report;
  nextDetailsSlot = (nextDetailsSlot + 1) % DETAILS_CACHE_SIZE;
  if (detailsCacheCount < DETAILS_CACHE_SIZE) ++detailsCacheCount;

  static BleAdDecoded decoded;
  memset(&decoded, 0, sizeof(decoded));
  bleDecodeAd(report.data, report.advLength, decoded);
  bleDecodeAd(report.data + report.advLength, report.responseLength, decoded);

  static char detailBuf[768];
  snprintf(
      detailBuf, sizeof(detailBuf),
      "name=%s;manufacturer_ids=%s;service_uuids16=%s;service_uuids32=%s;service_uuids128=%s;tx_power_dbm=%s;flags=%s;fef3_service=%s;decode_warnings=%u",
      decoded.name[0] ? decoded.name : "not_advertised",
      decoded.manufacturerIds[0] ? decoded.manufacturerIds : "not_advertised",
      decoded.serviceUuids16[0] ? decoded.serviceUuids16 : "not_advertised",
      decoded.serviceUuids32[0] ? decoded.serviceUuids32 : "not_advertised",
      decoded.serviceUuids128[0] ? decoded.serviceUuids128 : "not_advertised",
      decoded.txPowerDbm[0] ? decoded.txPowerDbm : "not_advertised",
      decoded.flags[0] ? decoded.flags : "not_advertised",
      decoded.hasFef3Service ? "yes" : "no",
      decoded.warnings);

  CsvBuilder csv;
  csv.reset(lineBuf, sizeof(lineBuf));
  csv.appendBase("EVENT", report.timestampMs);
  csv.fieldU32(report.reportId);             // 8: report_id
  csv.fieldText(mac);                        // 9: mac
  csv.fieldU32(report.addressType);          // 10: addr_type
  csv.fieldI32(report.rssi);                 // 11: rssi_dbm
  for (int col = 12; col <= 16; ++col) csv.fieldText("");
  csv.fieldText("AD_DETAILS");               // 17: event
  csv.fieldText(detailBuf);                  // 18: detail
  csv.fieldU32(droppedReports.load());       // 19: dropped_reports
  csv.finishLine();
  writeSerialBufferSafe(csv.buf, csv.len, false);
}

void drainReports() {
  ScanReport report;
  // Limit work per pass so USB events, commands and status events stay responsive.
  for (size_t i = 0; i < MAX_REPORTS_PER_LOOP && xQueueReceive(reportQueue, &report, 0) == pdTRUE; ++i) {
    char mac[18];
    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
             report.mac[0], report.mac[1], report.mac[2],
             report.mac[3], report.mac[4], report.mac[5]);

    CsvBuilder csv;
    csv.reset(lineBuf, sizeof(lineBuf));
    csv.appendBase("REPORT", report.timestampMs);
    csv.fieldU32(report.reportId);           // 8: report_id
    csv.fieldText(mac);                      // 9: mac
    csv.fieldU32(report.addressType);        // 10: addr_type
    csv.fieldI32(report.rssi);               // 11: rssi_dbm
    csv.fieldU32(report.eventType);          // 12: event_type
    csv.fieldU32(report.advLength);          // 13: adv_len
    csv.fieldU32(report.responseLength);     // 14: scan_rsp_len
    csv.fieldHex(report.data, report.advLength);                          // 15: adv_hex
    csv.fieldHex(report.data + report.advLength, report.responseLength);  // 16: scan_rsp_hex
    csv.fieldText("");                       // 17: event
    csv.fieldText("");                       // 18: detail
    csv.fieldU32(droppedReports.load());     // 19: dropped_reports
    csv.finishLine();
    writeSerialBufferSafe(csv.buf, csv.len, true);
    emitAdDetails(report, mac);
  }
}

void closeTrial(const char* reason) {
  scanning = false;
  stopping = false;
  stopAfterStart = false;
  // Drain remaining queued reports with a bounded pass count so closeTrial cannot hang.
  for (int pass = 0; pass < 40 && uxQueueMessagesWaiting(reportQueue); ++pass) {
    drainReports();
    delay(1);
  }
  xQueueReset(reportQueue);
  char detail[96];
  snprintf(detail, sizeof(detail), "%s;received_reports=%lu", reason, (unsigned long)reportCount.load());
  emitEvent("TRIAL_END", detail);
  trialOpen = false;
}

bool validLabel(const String& value) {
  if (value.length() == 0 || value.length() > 40) return false;
  for (size_t i = 0; i < value.length(); ++i) {
    char c = value[i];
    if (!((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') ||
          (c >= '0' && c <= '9') || c == '_' || c == '-')) return false;
  }
  return true;
}

void handleCommand(String input) {
  input.trim();
  String shortCommand = input;
  shortCommand.toLowerCase();
  uint32_t requestedDuration = SCAN_DURATION_SECONDS;
  if (shortCommand == "help") {
    emitEvent("HELP", "on=Bluetooth on/home; off=Bluetooth off; share=Quick Share visible; lock=screen locked; unlock=Quick Share after unlock; each scans 30s for REDMI_PAD_PRO. Change target device state before sending. stop/status also supported.");
    return;
  }
  if (shortCommand == "on" || shortCommand == "off" || shortCommand == "share" ||
      shortCommand == "lock" || shortCommand == "unlock") {
    requestedDuration = 30;
    const char* quickCondition = "BT_ON_HOME";
    if (shortCommand == "off") quickCondition = "BT_OFF";
    else if (shortCommand == "share" || shortCommand == "unlock") quickCondition = "SHARING_ON";
    else if (shortCommand == "lock") quickCondition = "SCREEN_LOCKED";
    input = String("start QUICK trial") + String(++quickTrialNumber) +
            " REDMI_PAD_PRO " + quickCondition + " passive";
  }
  if (shortCommand == "status") {
    char detail[128];
    snprintf(detail, sizeof(detail), "trial_open=%d;scanning=%d;stopping=%d;received_reports=%lu;free_heap=%lu",
             (int)trialOpen, (int)scanning, (int)stopping,
             (unsigned long)reportCount.load(), (unsigned long)ESP.getFreeHeap());
    emitEvent("STATUS", detail);
    return;
  }
  if (shortCommand == "stop") {
    emitEvent("COMMAND_RECEIVED", "stop");
    if (!scanning) {
      if (trialOpen) {
        stopAfterStart = true;
        emitEvent("STOP_PENDING", "Stop queued until scan startup completes");
      } else {
        emitEvent("STATUS", "STOPPED;no scan is running");
      }
      return;
    }
    if (stopping) {
      emitEvent("STOP_PENDING", "Stop already requested; waiting for controller");
      return;
    }
    esp_err_t err = esp_ble_gap_stop_scanning();
    if (err == ESP_OK) {
      stopping = true;
      stopRequestedMs = millis();
      emitEvent("STOP_REQUESTED", "Controller stop requested");
    }
    else emitEvent("STOP_REQUEST_ERROR", esp_err_to_name(err));
    return;
  }
  String tokens[6];
  int count = 0;
  int offset = 0;
  while (offset < (int)input.length() && count < 6) {
    int end = input.indexOf(' ', offset);
    if (end < 0) end = input.length();
    if (end > offset) tokens[count++] = input.substring(offset, end);
    offset = end + 1;
  }
  if (count != 6 || offset < (int)input.length() || tokens[0] != "start" ||
      !validLabel(tokens[1]) || !validLabel(tokens[2]) ||
      !validLabel(tokens[3]) || !validLabel(tokens[4]) ||
      (tokens[5] != "passive" && tokens[5] != "active")) {
    emitEvent("COMMAND_ERROR", "Use: start SESSION TRIAL DEVICE CONDITION passive|active");
    return;
  }
  if (trialOpen) {
    emitEvent("COMMAND_ERROR", "Wait for TRIAL_END before starting another trial");
    return;
  }
  sessionId = tokens[1]; trialId = tokens[2]; deviceLabel = tokens[3];
  condition = tokens[4]; scanMode = tokens[5];
  scanDurationSeconds = requestedDuration;
  detailsCacheCount = 0;
  nextDetailsSlot = 0;
  xQueueReset(reportQueue);
  paramsStatus.store(-1); startStatus.store(-1); stopStatus.store(-1);
  reportCount.store(0); droppedReports.store(0); scanFinished.store(false);
  trialOpen = true;
  trialOpenMs = millis();
  emitEvent("TRIAL_BEGIN", "Labels describe the experiment, not verified transmitter identity");
  scanParams = {};
  scanParams.scan_type = scanMode == "active" ? BLE_SCAN_TYPE_ACTIVE : BLE_SCAN_TYPE_PASSIVE;
  scanParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  scanParams.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL;
  scanParams.scan_interval = SCAN_INTERVAL_UNITS;
  scanParams.scan_window = SCAN_WINDOW_UNITS;
  scanParams.scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE;
  char cfgDetail[128];
  snprintf(cfgDetail, sizeof(cfgDetail),
           "interval_ms=100;window_ms=80;duplicate_filter=disabled;duration_s=%lu;legacy_reports_only",
           (unsigned long)scanDurationSeconds);
  emitEvent("SCAN_CONFIG", cfgDetail);
  esp_err_t err = esp_ble_gap_set_scan_params(&scanParams);
  if (err != ESP_OK) closeTrial(esp_err_to_name(err));
}

void setup() {
  Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
  Serial.setTxTimeoutMs(50);
#endif
  uint32_t waitStart = millis();
  while (!Serial && millis() - waitStart < 3000) delay(10);
  const char* header = "record_type,schema_version,session_id,trial_id,device_label,condition,scan_mode,timestamp_ms,report_id,mac,addr_type,rssi_dbm,event_type,adv_len,scan_rsp_len,adv_hex,scan_rsp_hex,event,detail,dropped_reports\r\n";
  writeSerialBufferSafe(header, strlen(header), false);
  char bootDetail[160];
#if ARDUINO_USB_CDC_ON_BOOT && ARDUINO_USB_MODE
  const char* usbModeName = "hwcdc";
#elif ARDUINO_USB_CDC_ON_BOOT
  const char* usbModeName = "tinyusb";
#else
  const char* usbModeName = "uart";
#endif
  snprintf(bootDetail, sizeof(bootDetail), "chip=%s;sdk=%s;reset_reason=%d;usb=%s",
           ESP.getChipModel(), ESP.getSdkVersion(), (int)esp_reset_reason(), usbModeName);
  emitEvent("BOOT", bootDetail);
  reportQueue = xQueueCreate(QUEUE_LENGTH, sizeof(ScanReport));
  if (!reportQueue) {
    emitEvent("FATAL", "Could not allocate report queue");
    while (true) delay(1000);
  }
  BLEDevice::init("");
  BLEDevice::setCustomGapHandler(gapCallback);
  emitEvent("READY", "Commands: on, off, share, lock, unlock (30s each); stop, status, help. REPORT=raw scan result; AD_DETAILS=decoded hints, not device identity.");
}

void finishSerialCommand() {
  if (commandTooLong) emitEvent("COMMAND_ERROR", "Command exceeds 240 characters");
  else if (commandBuffer.length()) handleCommand(commandBuffer);
  commandBuffer = "";
  commandTooLong = false;
}

void serviceSerialCommands() {
  while (Serial.available() > 0) {
    int byteRead = Serial.read();
    if (byteRead < 0) break;
    char c = (char)byteRead;
    lastCommandByteMs = millis();
    if (c == '\n' || c == '\r') {
      finishSerialCommand();
    } else {
      if (commandBuffer.length() < 240) commandBuffer += c;
      else commandTooLong = true;
    }
  }
  // Short commands also work with Serial Monitor's "No line ending" setting.
  // Long start commands still require a delimiter, avoiding partial dispatch.
  if (commandBuffer.length() && millis() - lastCommandByteMs >= 300) {
    String pending = commandBuffer;
    pending.trim();
    pending.toLowerCase();
    if (pending == "stop" || pending == "status" || pending == "help" || pending == "on" ||
        pending == "off" || pending == "share" || pending == "lock" || pending == "unlock") {
      finishSerialCommand();
    }
  }
}

void loop() {
  serviceSerialCommands(); // Service input before writing a batch of reports.
  int status = paramsStatus.exchange(-1);
  if (status >= 0 && trialOpen) {
    if (status != ESP_BT_STATUS_SUCCESS) closeTrial("SCAN_PARAM_FAILED");
    else {
      esp_err_t err = esp_ble_gap_start_scanning(scanDurationSeconds);
      if (err != ESP_OK) closeTrial(esp_err_to_name(err));
    }
  }
  status = startStatus.exchange(-1);
  if (status >= 0 && trialOpen) {
    if (status != ESP_BT_STATUS_SUCCESS) closeTrial("SCAN_START_FAILED");
    else {
      scanning = true;
      scanStartedMs = millis();
      emitEvent("SCAN_STARTED", "Controller confirmed scan start");
      if (stopAfterStart) {
        stopAfterStart = false;
        handleCommand("stop");
      }
    }
  }
  status = stopStatus.exchange(-1);
  if (status >= 0 && trialOpen) {
    if (status == ESP_BT_STATUS_SUCCESS) closeTrial("STOP_CONFIRMED");
    else { stopping = false; emitEvent("SCAN_STOP_FAILED", String(status)); }
  }
  if (scanFinished.exchange(false) && trialOpen) closeTrial("DURATION_COMPLETE");

  // Safety timeouts so a missed GAP callback can never leave trialOpen stuck.
  uint32_t now = millis();
  if (trialOpen && !scanning && (now - trialOpenMs) >= STATE_TIMEOUT_MS) {
    closeTrial("START_TIMEOUT");
  } else if (trialOpen && scanning && !stopping &&
             (now - scanStartedMs) >= (scanDurationSeconds + 5UL) * 1000UL) {
    esp_err_t err = esp_ble_gap_stop_scanning();
    if (err == ESP_OK) {
      stopping = true;
      stopRequestedMs = now;
      emitEvent("STOP_REQUESTED", "Safety duration timeout requested controller stop");
    } else {
      closeTrial("DURATION_TIMEOUT");
    }
  } else if (trialOpen && stopping && (now - stopRequestedMs) >= STATE_TIMEOUT_MS) {
    closeTrial("STOP_TIMEOUT");
  }

  drainReports();
  serviceSerialCommands();
  now = millis();
  if (now - lastHeartbeatMs >= HEARTBEAT_MS) {
    lastHeartbeatMs = now;
    char hbDetail[96];
    snprintf(hbDetail, sizeof(hbDetail), "scanning=%d;received_reports=%lu;free_heap=%lu",
             (int)scanning, (unsigned long)reportCount.load(), (unsigned long)ESP.getFreeHeap());
    emitEvent("HEARTBEAT", hbDetail);
  }
  delay(1);
}
#endif // CONFIG_BLUEDROID_ENABLED
