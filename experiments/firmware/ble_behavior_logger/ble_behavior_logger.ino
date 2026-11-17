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
#include "BleAdParser.h"

constexpr uint16_t SCAN_INTERVAL_UNITS = 160; // 100 ms; GAP unit = 0.625 ms
constexpr uint16_t SCAN_WINDOW_UNITS = 128;   // 80 ms
constexpr uint32_t SCAN_DURATION_SECONDS = 600;
constexpr uint32_t HEARTBEAT_MS = 3000;
constexpr size_t QUEUE_LENGTH = 128;
constexpr size_t COLUMN_COUNT = 20;

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
String sessionId, trialId, deviceLabel, condition, scanMode;
String commandBuffer;
bool commandTooLong = false;
uint32_t lastHeartbeatMs = 0;
uint32_t lastCommandByteMs = 0;
uint32_t quickTrialNumber = 0;
uint32_t scanDurationSeconds = SCAN_DURATION_SECONDS;
constexpr size_t DETAILS_CACHE_SIZE = 24;
ScanReport detailsCache[DETAILS_CACHE_SIZE];
size_t detailsCacheCount = 0;
size_t nextDetailsSlot = 0;

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

String hexBytes(const uint8_t* bytes, size_t length) {
  const char digits[] = "0123456789ABCDEF";
  String value;
  value.reserve(length * 2);
  for (size_t i = 0; i < length; ++i) {
    value += digits[bytes[i] >> 4];
    value += digits[bytes[i] & 15];
  }
  return value;
}

void writeRow(String* fields) {
  String line;
  line.reserve(512);
  for (size_t i = 0; i < COLUMN_COUNT; ++i) {
    if (i) line += ',';
    line += '"';
    for (size_t j = 0; j < fields[i].length(); ++j) {
      const char ch = fields[i][j];
      if (ch == '"') line += '"';
      line += (ch == '\r' || ch == '\n') ? ' ' : ch;
    }
    line += '"';
  }
  Serial.println(line);
}

void baseRow(String* fields, const char* type, uint32_t timestamp) {
  fields[0] = type;
  fields[1] = "1";
  fields[2] = sessionId;
  fields[3] = trialId;
  fields[4] = deviceLabel;
  fields[5] = condition;
  fields[6] = scanMode;
  fields[7] = String(timestamp);
  fields[19] = String(droppedReports.load());
}

void emitEvent(const char* event, const String& detail) {
  String fields[COLUMN_COUNT];
  baseRow(fields, "EVENT", millis());
  fields[17] = event;
  fields[18] = detail;
  writeRow(fields);
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

  BleAdDecoded decoded = {};
  bleDecodeAd(report.data, report.advLength, decoded);
  bleDecodeAd(report.data + report.advLength, report.responseLength, decoded);
  String detail;
  detail.reserve(768);
  detail = String("name=") + (decoded.name[0] ? decoded.name : "not_advertised");
  detail += String(";manufacturer_ids=") + (decoded.manufacturerIds[0] ? decoded.manufacturerIds : "not_advertised");
  detail += String(";service_uuids16=") + (decoded.serviceUuids16[0] ? decoded.serviceUuids16 : "not_advertised");
  detail += String(";service_uuids32=") + (decoded.serviceUuids32[0] ? decoded.serviceUuids32 : "not_advertised");
  detail += String(";service_uuids128=") + (decoded.serviceUuids128[0] ? decoded.serviceUuids128 : "not_advertised");
  detail += String(";tx_power_dbm=") + (decoded.txPowerDbm[0] ? decoded.txPowerDbm : "not_advertised");
  detail += String(";flags=") + (decoded.flags[0] ? decoded.flags : "not_advertised");
  detail += String(";fef3_service=") + (decoded.hasFef3Service ? "yes" : "no");
  detail += String(";decode_warnings=") + decoded.warnings;
  String fields[COLUMN_COUNT];
  baseRow(fields, "EVENT", report.timestampMs);
  fields[8] = String(report.reportId);
  fields[9] = mac;
  fields[10] = String(report.addressType);
  fields[11] = String(report.rssi);
  fields[17] = "AD_DETAILS";
  fields[18] = detail;
  writeRow(fields);
}

void drainReports() {
  ScanReport report;
  // Limit work per pass so commands and status events are serviced under load.
  for (int i = 0; i < 16 && xQueueReceive(reportQueue, &report, 0) == pdTRUE; ++i) {
    String fields[COLUMN_COUNT];
    baseRow(fields, "REPORT", report.timestampMs);
    fields[8] = String(report.reportId);
    char mac[18];
    snprintf(mac, sizeof(mac), "%02X:%02X:%02X:%02X:%02X:%02X",
             report.mac[0], report.mac[1], report.mac[2],
             report.mac[3], report.mac[4], report.mac[5]);
    fields[9] = mac;
    fields[10] = String(report.addressType);
    fields[11] = String(report.rssi);
    fields[12] = String(report.eventType);
    fields[13] = String(report.advLength);
    fields[14] = String(report.responseLength);
    fields[15] = hexBytes(report.data, report.advLength);
    fields[16] = hexBytes(report.data + report.advLength, report.responseLength);
    writeRow(fields);
    emitAdDetails(report, mac);
  }
}

void closeTrial(const char* reason) {
  scanning = false;
  stopping = false;
  stopAfterStart = false;
  // Called only after the controller confirms stop/completion (or failed start).
  while (uxQueueMessagesWaiting(reportQueue)) drainReports();
  emitEvent("TRIAL_END", String(reason) + ";received_reports=" + reportCount.load());
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
    emitEvent("HELP", "on=Bluetooth on/home; off=Bluetooth off; share=Quick Share visible; lock=screen locked; unlock=Quick Share after unlock; each scans 30s. Change phone state yourself before sending. stop/status also supported.");
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
            " HONOR_X7B_5G " + quickCondition + " passive";
  }
  if (shortCommand == "status") {
    emitEvent("STATUS", String("trial_open=") + trialOpen + ";scanning=" + scanning +
              ";stopping=" + stopping + ";received_reports=" + reportCount.load());
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
      emitEvent("STOP_REQUESTED", "Controller stop requested");
    }
    else emitEvent("STOP_REQUEST_ERROR", String(esp_err_to_name(err)));
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
  reportCount.store(0); droppedReports.store(0); scanFinished.store(false);
  trialOpen = true;
  emitEvent("TRIAL_BEGIN", "Labels describe the experiment, not verified transmitter identity");
  scanParams = {};
  scanParams.scan_type = scanMode == "active" ? BLE_SCAN_TYPE_ACTIVE : BLE_SCAN_TYPE_PASSIVE;
  scanParams.own_addr_type = BLE_ADDR_TYPE_PUBLIC;
  scanParams.scan_filter_policy = BLE_SCAN_FILTER_ALLOW_ALL;
  scanParams.scan_interval = SCAN_INTERVAL_UNITS;
  scanParams.scan_window = SCAN_WINDOW_UNITS;
  scanParams.scan_duplicate = BLE_SCAN_DUPLICATE_DISABLE;
  emitEvent("SCAN_CONFIG", String("interval_ms=100;window_ms=80;duplicate_filter=disabled;duration_s=") + scanDurationSeconds + ";legacy_reports_only");
  esp_err_t err = esp_ble_gap_set_scan_params(&scanParams);
  if (err != ESP_OK) closeTrial(esp_err_to_name(err));
}

void setup() {
  Serial.begin(115200);
  uint32_t waitStart = millis();
  while (!Serial && millis() - waitStart < 3000) delay(10);
  Serial.println("record_type,schema_version,session_id,trial_id,device_label,condition,scan_mode,timestamp_ms,report_id,mac,addr_type,rssi_dbm,event_type,adv_len,scan_rsp_len,adv_hex,scan_rsp_hex,event,detail,dropped_reports");
  emitEvent("BOOT", String("chip=") + ESP.getChipModel() + ";sdk=" + ESP.getSdkVersion() +
            ";reset_reason=" + String((int)esp_reset_reason()));
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
  while (Serial.available()) {
    char c = Serial.read();
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
  drainReports();
  serviceSerialCommands();
  if (millis() - lastHeartbeatMs >= HEARTBEAT_MS) {
    lastHeartbeatMs = millis();
    emitEvent("HEARTBEAT", String("scanning=") + scanning + ";received_reports=" + reportCount.load());
  }
  delay(1);
}
#endif // CONFIG_BLUEDROID_ENABLED
