#ifndef HPS166H_H
#define HPS166H_H

#include "Arduino.h"
#include <SoftwareSerial.h>

class HPS166 {
public:
    HPS166();
    void begin();
    bool check();
    float distance();
    String fullReceive();

private:
    SoftwareSerial _serial;
    void sendCommand(const uint8_t *command, uint8_t length);
    int receiveResponse(uint8_t *response, uint8_t length);
    uint16_t calculateCRC(const uint8_t *data, uint8_t length);
};

#endif // HPS166H_H
