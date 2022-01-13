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


#include <Arduino.h>

#include "FwBox_PMSX003.h"


FwBox_PMSX003::FwBox_PMSX003(SoftwareSerial* pPmsSerial)
{
    PmsSerial = pPmsSerial;
    FwBox_PMSX003::initVariables();
    FwBox_PMSX003::SerialType = SERIAL_TYPE_SOFT_WARE;
}
FwBox_PMSX003::FwBox_PMSX003(HardwareSerial* pPmsSerial)
{
    PmsSerial = pPmsSerial;
    FwBox_PMSX003::initVariables();
    FwBox_PMSX003::SerialType = SERIAL_TYPE_HARD_WARE;
}

void FwBox_PMSX003::initVariables()
{
    FwBox_PMSX003::SerialType = SERIAL_TYPE_SOFT_WARE;
	
    FwBox_PMSX003::DeviceType = PMS5003T; // default
    
    DelayValue[FwBox_PMSX003::AFTER_SEND_PASSIVE_CMD] = 100;
    DelayValue[FwBox_PMSX003::AFTER_SEND_REQUEST_CMD] = 50;
    DelayValue[FwBox_PMSX003::SERIAL_READ] = 5;
    
    FwBox_PMSX003::LastErr = 0;
}

void FwBox_PMSX003::begin()
{
    if (FwBox_PMSX003::SerialType == SERIAL_TYPE_SOFT_WARE) {
        ((SoftwareSerial*)PmsSerial)->begin(9600);
    }
    else {
        ((HardwareSerial*)PmsSerial)->begin(9600);
    }


    //
    // Change mode to passive
    //
    PmsSerial->write(PMS_CMD_PASSIVE_MODE, sizeof(PMS_CMD_PASSIVE_MODE));
    delay(DelayValue[FwBox_PMSX003::AFTER_SEND_PASSIVE_CMD]);

}

PMS5003T_DATA* FwBox_PMSX003::readPms() {
    int bi = 0;
    unsigned char c;
    unsigned long pms_timeout = 0;
    int data_len = 0;
    
    LastErr = 0;

    //
    // Send request read
    //
    PmsSerial->write(PMS_CMD_REQUEST_READ, sizeof(PMS_CMD_REQUEST_READ));
    delay(DelayValue[FwBox_PMSX003::AFTER_SEND_REQUEST_CMD]); // Wait for PMS5003 to calculate data.
  
    memset(&(Buff[0]), 0, MAX_PMS_DATA_SIZE);
  
    //
    // Search for the sign - {0x42, 0x4d}
    //
    pms_timeout = millis();
    while ((millis() - pms_timeout) < 1300) {
        if(PmsSerial->available()) {
            Buff[1] = PmsSerial->read();
            if((Buff[0] == 0x42) && (Buff[1] == 0x4d)) {
                bi = 2;
                break;
            }
            else {
                  Buff[0] = Buff[1];
            }
            delay(DelayValue[FwBox_PMSX003::SERIAL_READ]); // PMS5003T is a slow sensor
        }
    }

    if(bi != 2) { // timeout
        LastErr = 1;
        Pd = 0;
        return Pd;
    }

    //
    // Receive data and copy to "Buff".
    //
    pms_timeout = millis();
    while ((millis() - pms_timeout) < 1500) {

        if(bi >= MAX_PMS_DATA_SIZE) break; // exceed buffer size
        if((data_len != 0) && (bi >= (data_len + 4))) break; // exceed data length

        if(PmsSerial->available()) {
            c = PmsSerial->read();
            Buff[bi] = c;
            bi++;
            
            if(bi == 4) {
                //
                // Buff[2] : high byte
                // Buff[3] : low byte
                //
                data_len = Buff[2];
                data_len = (data_len << 8) + Buff[3];
            }
      
            delay(DelayValue[FwBox_PMSX003::SERIAL_READ]); // PMS5003T is a slow sensor
        }
    }
    
    if(data_len == 0) { // data error
        LastErr = 2;
        Pd = 0;
        //Serial.println("ERROR : 02");
        return Pd;
    }
    else {
        if(bi < (data_len + 4)) { // timeout
            LastErr = 3;
            Pd = 0;
            //Serial.println("ERROR : 03");
            return Pd;
        }
    }

    //
    // Exchange high low bytes
    //
    for(int i = 2; i < MAX_PMS_DATA_SIZE; i = i + 2) {
        uint8_t temp8 = Buff[i];
        Buff[i] = Buff[i + 1];
        Buff[i + 1] = temp8;
    }

    Pd = (PMS5003T_DATA*)(&(Buff[0]));

    //
    // Use bi to calculate checksum
    //
    bi = 0;
    for(int i = 0; i < (Pd->DATA_LENGTH + 4 - 2); i++) {
      bi += Buff[i];
    }

    //Serial.printf("Pd->DATA_LENGTH=%d\n", Pd->DATA_LENGTH);

    if (Pd->DATA_LENGTH == 28) {
        if(bi != Pd->CHECKSUM) { // checksum error
            LastErr = 4;
            Pd = 0;
            Serial.println("ERROR : 04");
            return Pd;
        }
        else {
            if(Pd->PM_HUMI < 20) // Temperature=Pd->PM_TEMP/10, Humidity=Pd->PM_HUMI/10
                FwBox_PMSX003::DeviceType = PMS5003;
            else
                FwBox_PMSX003::DeviceType = PMS5003T;
        }
    }
    else {
        PMS3003_DATA* Pd3003 = (PMS3003_DATA*)Pd;
        if(bi != Pd3003->CHECKSUM) { // checksum error
            LastErr = 5;
            Pd = 0;
            //Serial.println("ERROR : 05");
            return Pd;
        }
        else {
            FwBox_PMSX003::DeviceType = PMS3003;
        }
    }

    if((Pd->SIG_1 == 0x42) && (Pd->SIG_2 == 0x4d)) {
        return Pd;
    }
    else {
        LastErr = 6;
        Pd = 0;
        //Serial.println("ERROR : 06");
        return Pd;
    }
}

//
// Running readPms before running pm1_0
//
int FwBox_PMSX003::pm1_0()
{
    if(Pd == 0)
        return 0;
    else
        return Pd->PM_AE_UG_1_0;
}

//
// Running readPms before running pm2_5
//
int FwBox_PMSX003::pm2_5()
{
    if(Pd == 0)
        return 0;
    else
        return Pd->PM_AE_UG_2_5;
}

//
// Running readPms before running pm10_0
//
int FwBox_PMSX003::pm10_0()
{
    if(Pd == 0)
        return 0;
    else
        return Pd->PM_AE_UG_10_0;
}

//
// Running readPms before running temp
//
float FwBox_PMSX003::temp()
{
    if(Pd == 0)
        return 0;
    else
        return ((float)(Pd->PM_TEMP)) / 10;
}

//
// Running readPms before running humi
//
float FwBox_PMSX003::humi()
{
    if(Pd == 0)
        return 0;
    else
        return ((float)(Pd->PM_HUMI)) / 10;
}

//
// Running readPms before running readDeviceType
//
FwBox_PMSX003::PMS_TYPE FwBox_PMSX003::readDeviceType()
{
    return FwBox_PMSX003::DeviceType;
}

//
// The function must be run before begin.
//
void FwBox_PMSX003::setDelay(FwBox_PMSX003::PMS_DELAY PmsDelay, int Value)
{
    DelayValue[PmsDelay] = Value;
}

void FwBox_PMSX003::sleep()
{
    PmsSerial->write(PMS_CMD_SLEEP, sizeof(PMS_CMD_SLEEP));
    delay(DelayValue[FwBox_PMSX003::AFTER_SEND_PASSIVE_CMD]);
}

void FwBox_PMSX003::wakeup()
{
    PmsSerial->write(PMS_CMD_WAKEUP, sizeof(PMS_CMD_WAKEUP));
    delay(DelayValue[FwBox_PMSX003::AFTER_SEND_PASSIVE_CMD]);
}

int FwBox_PMSX003::getLastError()
{
	return FwBox_PMSX003::LastErr;
}
