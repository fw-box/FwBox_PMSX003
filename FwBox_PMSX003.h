//
// Copyright (c) 2020 Fw-Box (https://fw-box.com)
// Author: Hartman Hsieh
//
// Description :
//   Dust sensor - PMS5003T or PMS3003
//   Environment:
//     NodeMCU 1.0
//     Arduino 1.8.9
//
// Connections :
//   PMS5003T => Serial Port
//
// Required Library :
//   None
//

#ifndef __FWBOX_PMSX003_H__
#define __FWBOX_PMSX003_H__


#include "SoftwareSerial.h"


#define MAX_PMS_DATA_SIZE 32

#pragma pack(push)
#pragma pack(1)
struct PMS5003T_DATA {
    uint8_t SIG_1;
    uint8_t SIG_2;
    uint16_t DATA_LENGTH;
    
    //
    // Standard Particles, CF=1
    //
    uint16_t PM_SP_UG_1_0;
    uint16_t PM_SP_UG_2_5;
    uint16_t PM_SP_UG_10_0;

    //
    // Atmospheric environment
    //
    uint16_t PM_AE_UG_1_0;
    uint16_t PM_AE_UG_2_5;
    uint16_t PM_AE_UG_10_0;
    
    uint16_t PM_NUM_0_3;
    uint16_t PM_NUM_0_5;
    uint16_t PM_NUM_1_0;
    uint16_t PM_NUM_2_5;
    
    uint16_t PM_TEMP;
    uint16_t PM_HUMI;
    
    uint8_t ERRCODE;
    uint8_t VERCODE;
    
    uint16_t CHECKSUM;
};

struct PMS3003_DATA {
    uint8_t SIG_1;
    uint8_t SIG_2;
    uint16_t DATA_LENGTH;
    
    //
    // Standard Particles, CF=1
    //
    uint16_t PM_SP_UG_1_0;
    uint16_t PM_SP_UG_2_5;
    uint16_t PM_SP_UG_10_0;

    //
    // Atmospheric environment
    //
    uint16_t PM_AE_UG_1_0;
    uint16_t PM_AE_UG_2_5;
    uint16_t PM_AE_UG_10_0;
    
    uint16_t RSV7;
    uint16_t RSV8;
    uint16_t RSV9;

    uint16_t CHECKSUM;
};
#pragma pack(pop)

const uint8_t PMS_CMD_PASSIVE_MODE[] = { 0x42, 0x4D, 0xE1, 0x00, 0x00, 0x01, 0x70 };
const uint8_t PMS_CMD_REQUEST_READ[] = { 0x42, 0x4D, 0xE2, 0x00, 0x00, 0x01, 0x71 };
const uint8_t PMS_CMD_SLEEP[]        = { 0x42, 0x4D, 0xE4, 0x00, 0x00, 0x01, 0x73 };
const uint8_t PMS_CMD_WAKEUP[]       = { 0x42, 0x4D, 0xE4, 0x00, 0x01, 0x01, 0x74 };


class FwBox_PMSX003
{

public:
    typedef enum {
      PMS5003T,
      PMS3003,
      PMS5003
    }
    PMS_TYPE;

    typedef enum {
      AFTER_SEND_PASSIVE_CMD,
      AFTER_SEND_REQUEST_CMD,
      SERIAL_READ
    }
    PMS_DELAY;

    int LastErr;

    FwBox_PMSX003(SoftwareSerial* pPmsSerial);
    void begin();
    PMS5003T_DATA* readPms();
    int pm1_0(); // PM1.0 ug/m^3
    int pm2_5(); // PM2.5 ug/m^3
    int pm10_0(); // PM10 ug/m^3
    float temp();
    float humi();
    PMS_TYPE readDeviceType();

    //
    // The function must be run before begin.
    //
    void setDelay(PMS_DELAY PmsDelay, int Value);

private:
    SoftwareSerial* PmsSerial;
    uint8_t Buff[MAX_PMS_DATA_SIZE];
    PMS5003T_DATA* Pd;
    PMS_TYPE DeviceType;
    int DelayValue[3];

};

#endif // __FWBOX_PMSX003_H__
