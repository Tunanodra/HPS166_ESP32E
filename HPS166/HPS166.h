#include <Arduino.h>

class HPS166 {
public:
    HPS166(uint8_t txPin, uint8_t rxPin) : serial(txPin, rxPin) {}

    void begin() {
        serial1.begin(115200);
    }

    bool check() {
        static const uint8_t checkCmd[] = {0x0A, 0x2E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFC, 0x3C};
        static const uint8_t checkResponse[] = {0x0A, 0x18, 0xB0, 0x72, 0x3B, 0x53, 0xD4, 0x66, 0x30, 0xE7, 0x11, 0xA6, 0xD2, 0xD8, 0x8A, 0xDA, 0xF7, 0xE7, 0x3E, 0x16, 0x08, 0x0C, 0x02, 0x03, 0x7B, 0x98};
        sendCommand(checkCmd, sizeof(checkCmd));

        uint8_t response[sizeof(checkResponse)];
        size_t bytesRead = readResponse(response, sizeof(response));
        return bytesRead == sizeof(checkResponse) && memcmp(response, checkResponse, sizeof(checkResponse)) == 0;
    }

    float distance() {
        static const uint8_t distanceCmd[] = {0x0A, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0x57};
        sendCommand(distanceCmd, sizeof(distanceCmd));

        uint8_t response[15];
        size_t bytesRead = readResponse(response, sizeof(response));
        if (bytesRead == sizeof(response)) {
            uint16_t distanceRaw = response[5] * 256 + response[6];
            return distanceRaw / 1000.0f;
        }
        return -1.0f;
    }

    String fullReceive() {
        static const uint8_t fullReceiveCmd[] = {0x0A, 0x22, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAE, 0x57};
        sendCommand(fullReceiveCmd, sizeof(fullReceiveCmd));

        uint8_t response[15];
        size_t bytesRead = readResponse(response, sizeof(response));
        if (bytesRead == sizeof(response)) {
            uint16_t distanceRaw = response[5] * 256 + response[6];
            float distance = distanceRaw / 1000.0f;

            uint32_t magnitudeRaw = ((response[7] * 256 + response[8]) << response[9]) / 10000.0f;
            uint8_t ambientADC = response[10];
            uint16_t precision = response[11] * 256 + response[12];

            String result = "Distance: ";
            result += distance;
            result += " m, Magnitude: ";
            result += magnitudeRaw;
			result += ", Ambient ADC: ";
			result += ambientADC;
			result += ", Precision: ";
			result += precision;
			return result;
		}
		return "Error: Invalid response";
	}
	
private:
    SoftwareSerial serial;

    void sendCommand(const uint8_t *cmd, size_t cmdLength) {
        serial.write(cmd, cmdLength);
    }

    size_t readResponse(uint8_t *response, size_t responseLength) {
        size_t bytesRead = 0;
        uint32_t startTime = millis();
        while (bytesRead < responseLength && (millis() - startTime) < 1000) {
            if (serial.available()) {
                response[bytesRead++] = serial.read();
            }
        }
        return bytesRead;
    }
};
