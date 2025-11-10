#ifndef SUSI_MASTER_H
#define SUSI_MASTER_H

#include <Arduino.h>
#include "susi_hal.h"
#include "susi_packet.h"

class SUSI_Master {
public:
    SUSI_Master(SusiHAL& hal);
    void begin();
    bool sendPacket(const SUSI_Packet& packet, bool expectAck = false);
    uint8_t readByteAfterRequest();

private:
    SusiHAL& _hal;
    unsigned long _last_packet_time_ms;
    uint8_t _packets_since_sync;

    void sendByte(uint8_t byte);
};

class SUSI_Master_API {
public:
    SUSI_Master_API(SUSI_Master& master);
    void begin();

    bool setFunction(uint8_t address, uint8_t function, bool on);
    bool setSpeed(uint8_t address, uint8_t speed, bool forward);
    bool writeCV(uint8_t address, uint16_t cv, uint8_t value);
    uint8_t readCV(uint8_t address, uint16_t cv);

private:
    SUSI_Master _master;
};

#endif // SUSI_MASTER_H
