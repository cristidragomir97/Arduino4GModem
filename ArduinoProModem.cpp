#include "ArduinoProModem.h"


ArduinoPro_Modem::ArduinoPro_Modem() {
    client = new TinyGsmClient(modem);
}

void ArduinoPro_Modem::begin() {
    Serial.println("Initializing modem...");
    modem.init();
    String modemInfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modemInfo);
}

bool ArduinoPro_Modem::connect(const char * apn, const char * gprsUser, const char * gprsPass){
    SimStatus simStatus = getSimStatus();
    if(simStatus != SimStatus::SIM_ANTITHEFT_LOCKED) {
       simUnlock("");
    }

    simStatus = getSimStatus();
    if(simStatus == SimStatus::SIM_READY) {
        if(waitForNetworkRegistration()){
            if(connectToGPRS(apn, gprsUser, gprsPass)){
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}

bool ArduinoPro_Modem::isConnected(){
    return modem.isGprsConnected() && modem.isNetworkConnected();
}

bool ArduinoPro_Modem::getGPSLocation(float* latitude, float* longitude){

    Serial.println("Getting GPS location...");
    Serial.print("RAW location: "); Serial.println(modem.getGPSraw());

    modem.getGPS(latitude, longitude);
}

bool ArduinoPro_Modem::getTime(int * year, int * month, int * day, int * hour, int * minute, int * second){
    Serial.println("Getting time...");
    modem.getGPSTime(year, month, day, hour, minute, second);
}

bool ArduinoPro_Modem::sendSMS(const char * number, const char * message){
    Serial.println("Sending SMS...");
    Serial.println("\t Number: " + String(number));
    Serial.println("\t Message: " + String(message));
    modem.sendSMS(number, message);
}

TinyGsmClient ArduinoPro_Modem::getGSMClient(){
    return *client;
}

HttpClient ArduinoPro_Modem::getHTTPClient(const char * server, const int port){
    return HttpClient(*client, server, port);
}

bool ArduinoPro_Modem::isNetworkConnected(){
    return modem.isNetworkConnected();
}

bool ArduinoPro_Modem::connectToGPRS(const char * apn, const char * gprsUser, const char * gprsPass){
    Serial.print(F("Connecting to 4G network..."));
    while(!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.print(".");
        delay(2000);
    }
    return true;
}

bool ArduinoPro_Modem::isGPRSConnected(){
    return modem.isGprsConnected();
}

SimStatus ArduinoPro_Modem::getSimStatus(){
    int simStatus = modem.getSimStatus();
    Serial.println("SIM Status: " + String(simStatus));

    if (modem.getSimStatus() == 0) {
        return SimStatus::SIM_ERROR;
    } else if (modem.getSimStatus() == 1) {
        return SimStatus::SIM_READY;
    } else if (modem.getSimStatus() == 2) {
        return SimStatus::SIM_LOCKED;
    } else if (modem.getSimStatus() == 3) {
        return SimStatus::SIM_ANTITHEFT_LOCKED;
    } else {
        return SimStatus::SIM_ERROR;
    }
}

bool ArduinoPro_Modem::simUnlock(const char * pin){
    Serial.println("Unlocking SIM...");
    modem.simUnlock(pin); 
}

bool ArduinoPro_Modem::waitForNetworkRegistration(){
    Serial.print("Waiting for network registration...");
    while (!modem.waitForNetwork()) {
        Serial.println(".");
        delay(2000);
    } 
    return true;
}

bool ArduinoPro_Modem::enableGPS(){
    Serial.println("Enabling GPS...");
    modem.enableGPS();
    delay(20000);
}

String ArduinoPro_Modem::getPhoneNumber(){
    Serial.println("Getting phone number...");
    return modem.sendUSSD("*111*2#");
}
