#ifndef ARDUINO_PRO_MODEM_H
#define ARDUINO_PRO_MODEM_H

#include <Arduino.h>
#include <Arduino_4G_Module.h>

class ArduinoPro_Modem {
    public: 
        ArduinoPro_Modem();
        void begin();
        bool connect(const char * apn, const char * gprsUser, const char * gprsPass);
        bool connectToGPRS(const char * apn, const char * gprsUser, const char * gprsPass);
        bool isNetworkConnected();
        bool isGPRSConnected();
        bool isConnected();
        SimStatus getSimStatus();
        bool simUnlock(const char * pin);
        bool waitForNetworkRegistration();
        bool enableGPS();
        bool getGPSLocation(float* latitude, float* longitude);
        bool getTime(int * year, int * month, int * day, int * hour, int * minute, int * second);
        bool sendSMS(const char * number, const char * message);
        String getPhoneNumber();
        TinyGsmClient getGSMClient();
        HttpClient getHTTPClient(const char * server, const int port);

    private:
        TinyGsmClient * client;
};


#endif