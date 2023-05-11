#include "HPS166H.h"

#define HPS166_CHECK_COMMAND_LENGTH 10
#define HPS166_DISTANCE_COMMAND_LENGTH 10
#define HPS166_CHECK_RESPONSE_LENGTH 24

static const uint8_t HPS166_CHECK_COMMAND[HPS166_CHECK_COMMAND_LENGTH] = {0x0A, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3C};
static const uint8_t HPS166_DISTANCE_COMMAND[HPS166_DISTANCE_COMMAND_LENGTH] = {0x0A, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0x57};
static const uint8_t HPS166_CHECK_RESPONSE[HPS166_CHECK_RESPONSE_LENGTH] = {0x0A, 0x18, 0xB0, 0x72, 0x3B, 0x53, 0xD4, 0x66, 0x30, 0xE7, 0x11, 0xA6, 0xD2, 0xD8, 0x8A, 0xDA, 0xF7, 0xE7, 0x3E, 0x16, 0x08, 0x0C, 0x02, 0x03};

HPS166::HPS166() : _serial(hps166Rx, hps166Tx) {}

void HPS166::begin() {
    _serial.begin(115200);
}

bool HPS166::check() {
    sendCommand(HPS166_CHECK_COMMAND, HPS166_CHECK_COMMAND_LENGTH);
    uint8_t response[HPS166_CHECK_RESPONSE_LENGTH];
    int responseLength = receiveResponse(response, HPS166_CHECK_RESPONSE_LENGTH);

    if (responseLength != HPS166_CHECK_RESPONSE_LENGTH) {
        return false;
    }

    for (uint8_t i = 0; i < HPS166_CHECK_RESPONSE_LENGTH; ++i) {
        if (response[i] != HPS166_CHECK_RESPONSE[i]) {
            return false;
        }
    }

    return true;
}

float HPS166::distance() {
    sendCommand(HPS166_DISTANCE_COMMAND, HPS166_DISTANCE_COMMAND_LENGTH);
    uint8_t response[HPS166_DISTANCE_RESPONSE_LENGTH];
    int responseLength = receiveResponse(response, HPS166_DISTANCE_RESPONSE_LENGTH);

    if (responseLength != HPS166_DISTANCE_RESPONSE_LENGTH) {
        return -1;
    }

    uint16_t distance = (response[2] << 8) | response[3];
    return distance / 1000.0;
}

String HPS166::fullReceive() {
    sendCommand(HPS166_FULL_RECEIVE_COMMAND, HPS166_FULL_RECEIVE_COMMAND_LENGTH);
    uint8_t response[HPS166_DISTANCE_RESPONSE_LENGTH];
    int responseLength = receiveResponse(response, HPS166_DISTANCE_RESPONSE_LENGTH);

    if (responseLength != HPS166_DISTANCE_RESPONSE_LENGTH) {
        return "Invalid response";
    }

    uint16_t distance = (response[2] << 8) | response[3];
    uint16_t magnitude = (response[4] << 8) | response[5];
    uint16_t ambientADC = (response[6] << 8) | response[7];
    uint8_t precision = response[8];

    String result = "Distance: " + String(distance / 1000.0) + " m, Magnitude: " + String(magnitude) + ", Ambient ADC: " + String(ambientADC) + ", Precision: " + String(precision);
   
    return result;
}

void HPS166::sendCommand(const uint8_t *command, uint8_t length) {
    for (uint8_t i = 0; i < length; ++i) {
        _serial.write(command[i]);
    }
}

int HPS166::receiveResponse(uint8_t *response, uint8_t length) {
    unsigned long startTime = millis();
    uint8_t receivedBytes = 0;

    while (millis() - startTime < 1000) {
        if (_serial.available()) {
            response[receivedBytes++] = _serial.read();

            if (receivedBytes == length) {
                break;
            }
        }
    }

    return receivedBytes;
}

uint16_t HPS166::calculateCRC(const uint8_t *data, uint8_t length) {
    uint16_t crc = 0xFFFF;

    for (uint8_t i = 0; i < length; ++i) {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; ++j) {
            if (crc & 0x0001) {
                crc = (crc >> 1) ^ 0xA001;
            } else {
                crc >>= 1;
            }
        }
    }

    return crc;
}

