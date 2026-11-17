#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Display hints only: the original bytes remain in each REPORT row.
struct BleAdDecoded {
    char name[65];
    char manufacturerIds[96];
    char serviceUuids16[160];
    char serviceUuids32[96];
    char serviceUuids128[192];
    char txPowerDbm[32];
    char flags[32];
    bool hasCompleteName;
    bool hasFef3Service;
    unsigned warnings;
};

inline void bleAppendValue(char* destination, size_t capacity, const char* value, unsigned& warnings) {
    const size_t used = strlen(destination);
    const size_t required = strlen(value) + (used ? 1 : 0);
    if (used + required >= capacity) { ++warnings; return; }
    if (used) strcat(destination, "|");
    strcat(destination, value);
}

inline uint32_t bleReadLittleEndian(const uint8_t* data, size_t length) {
    uint32_t value = 0;
    for (size_t i = 0; i < length; ++i) value |= (uint32_t)data[i] << (8 * i);
    return value;
}

inline void bleAddService(const uint8_t* data, size_t width, BleAdDecoded& decoded) {
    char value[37];
    if (width == 2) {
        const uint32_t id = bleReadLittleEndian(data, 2);
        snprintf(value, sizeof(value), "0x%04lX", (unsigned long)id);
        bleAppendValue(decoded.serviceUuids16, sizeof(decoded.serviceUuids16), value, decoded.warnings);
        if (id == 0xFEF3) decoded.hasFef3Service = true;
    } else if (width == 4) {
        snprintf(value, sizeof(value), "0x%08lX", (unsigned long)bleReadLittleEndian(data, 4));
        bleAppendValue(decoded.serviceUuids32, sizeof(decoded.serviceUuids32), value, decoded.warnings);
    } else if (width == 16) {
        const char digits[] = "0123456789ABCDEF";
        size_t output = 0;
        for (int i = 15; i >= 0; --i) {
            value[output++] = digits[data[i] >> 4];
            value[output++] = digits[data[i] & 15];
            if (i == 12 || i == 10 || i == 8 || i == 6) value[output++] = '-';
        }
        value[output] = '\0';
        bleAppendValue(decoded.serviceUuids128, sizeof(decoded.serviceUuids128), value, decoded.warnings);
    }
}

// Decode one AD block; call again on scan response to merge its hints.
inline void bleDecodeAd(const uint8_t* bytes, size_t size, BleAdDecoded& decoded) {
    size_t offset = 0;
    while (offset < size) {
        const size_t length = bytes[offset];
        if (length == 0) break;
        if (length > size - offset - 1) { ++decoded.warnings; break; }
        const uint8_t type = bytes[offset + 1];
        const uint8_t* data = bytes + offset + 2;
        const size_t count = length - 1;
        char value[16];
        if (type == 0x08 || type == 0x09) {
            if (type == 0x09 || !decoded.hasCompleteName) {
                const size_t copied = count < sizeof(decoded.name) - 1 ? count : sizeof(decoded.name) - 1;
                memcpy(decoded.name, data, copied);
                decoded.name[copied] = '\0';
                for (size_t i = 0; i < copied; ++i) {
                    if ((unsigned char)decoded.name[i] < 32 || decoded.name[i] == ';') decoded.name[i] = ' ';
                }
                if (copied < count) ++decoded.warnings;
                if (type == 0x09) decoded.hasCompleteName = true;
            }
        } else if (type == 0xFF) {
            if (count >= 2) {
                snprintf(value, sizeof(value), "0x%04lX", (unsigned long)bleReadLittleEndian(data, 2));
                bleAppendValue(decoded.manufacturerIds, sizeof(decoded.manufacturerIds), value, decoded.warnings);
            } else ++decoded.warnings;
        } else if (type >= 0x02 && type <= 0x07) {
            const size_t width = type <= 0x03 ? 2 : (type <= 0x05 ? 4 : 16);
            if (count % width) ++decoded.warnings;
            for (size_t i = 0; i + width <= count; i += width) bleAddService(data + i, width, decoded);
        } else if (type == 0x16 || type == 0x20 || type == 0x21) {
            const size_t width = type == 0x16 ? 2 : (type == 0x20 ? 4 : 16);
            if (count >= width) bleAddService(data, width, decoded);
            else ++decoded.warnings;
        } else if (type == 0x0A) {
            if (count == 1) {
                const int power = data[0] < 128 ? data[0] : (int)data[0] - 256;
                snprintf(value, sizeof(value), "%d", power);
                bleAppendValue(decoded.txPowerDbm, sizeof(decoded.txPowerDbm), value, decoded.warnings);
            } else ++decoded.warnings;
        } else if (type == 0x01) {
            if (count == 1) {
                snprintf(value, sizeof(value), "0x%02X", data[0]);
                bleAppendValue(decoded.flags, sizeof(decoded.flags), value, decoded.warnings);
            } else ++decoded.warnings;
        }
        offset += length + 1;
    }
}
