# FwBox_PMSX003
The library is for PMS5003, PMS5003T, PMS3003.


SoftwareSerial example:
-----------
    #include "FwBox_PMSX003.h"

    SoftwareSerial SerialSensor(D7, D8); // RX:D7, TX:D8

    FwBox_PMSX003 Pms(&SerialSensor);

    void setup() {
        Serial.begin(115200);
        Pms.begin();
    }

    void loop() {
        PMS5003T_DATA* pd = 0;

        //
        // Running readPms before running pm2_5, temp, humi and readDeviceType.
        //
        if(pd = Pms.readPms()) {
            if(Pms.readDeviceType() == FwBox_PMSX003::PMS5003T) {
            Serial.println("PMS5003T is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());
            Serial.print("Temperature=");
            Serial.println(Pms.temp());
            Serial.print("Humidity=");
            Serial.println(Pms.humi());
            }
            else if(Pms.readDeviceType() == FwBox_PMSX003::PMS5003) {
            Serial.println("PMS5003 is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());
            }
            else {
            Serial.println("PMS3003 is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());

            PMS3003_DATA* pd3003 = (PMS3003_DATA*)pd;
            }
        }
        else {
            Serial.println("PMS data format is wrong.");
        }

        Serial.println();

        delay(2000);
    }


HardwareSerial example:
-----------
    #include "FwBox_PMSX003.h"

    FwBox_PMSX003 Pms(&Serial2);

    void setup() {
        Serial.begin(115200);
        Pms.begin();
    }

    void loop() {
        PMS5003T_DATA* pd = 0;

        //
        // Running readPms before running pm2_5, temp, humi and readDeviceType.
        //
        if(pd = Pms.readPms()) {
            if(Pms.readDeviceType() == FwBox_PMSX003::PMS5003T) {
            Serial.println("PMS5003T is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());
            Serial.print("Temperature=");
            Serial.println(Pms.temp());
            Serial.print("Humidity=");
            Serial.println(Pms.humi());
            }
            else if(Pms.readDeviceType() == FwBox_PMSX003::PMS5003) {
            Serial.println("PMS5003 is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());
            }
            else {
            Serial.printf("millis() = %d\n", millis());
            Serial.println("PMS3003 is detected.");

            Serial.print("PM1.0=");
            Serial.println(Pms.pm1_0());
            Serial.print("PM2.5=");
            Serial.println(Pms.pm2_5());
            Serial.print("PM10=");
            Serial.println(Pms.pm10_0());

            PMS3003_DATA* pd3003 = (PMS3003_DATA*)pd;
            }
        }
        else {
            Serial.println("PMS data format is wrong.");
            Serial.printf("LastError : %d\n", Pms.getLastError());
        }

        Serial.println();

        delay(2000);
    }
