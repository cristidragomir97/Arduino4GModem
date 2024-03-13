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


    // Set GSM module to text mode
    modem.sendAT("+CMGF=1");
    modem.waitResponse();

    // Send intrerupt when SMS has been received
    modem.sendAT("AT+CNMI=2,1,0,0,0");
    modem.waitResponse();
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

bool ArduinoPro_Modem::getGPSLocation(float* latitude, float* longitude){
    float tempLat = 0.00000;
    float tempLon = 0.00000;
    modem.getGPS(&tempLat, &tempLon);

    while(tempLat == 0.00000 || tempLon == 0.00000) {
         modem.getGPS(&tempLat, &tempLon);
         delay(1000);
    }
    
    *latitude = tempLat;
    *longitude = tempLon;
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

bool ArduinoPro_Modem::isConnectedToOperator(){
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

bool ArduinoPro_Modem::isConnectedToInternet(){
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

bool ArduinoPro_Modem::enableGPS(bool assisted){
    Serial.println("Enabling GPS...");
    if(assisted){
        modem.sendAT("AT+QGPSCFG=\"agpsposmode\",33488767");
    } else {
        modem.sendAT("AT+QGPSCFG=\"agpsposmode\",8388608");
    }

    modem.waitResponse();
    //modem.sendAT(GF("+UTIME=1,1"));
    //modem.waitResponse();
    //modem.sendAT(GF("+UGPIOC=23,0,1"));

    //modem.waitResponse();

    modem.enableGPS();
    delay(10000);
}

String ArduinoPro_Modem::getPhoneNumber(){
    Serial.println("Getting phone number...");
    return modem.sendUSSD("*111*2#");
}

String ArduinoPro_Modem::sendATCommand( char * command){
    String resp;
    modem.sendAT((const char *)command);
    modem.waitResponse(1000, resp);
    return resp;
}

// Parses a single SMS entry from the data
SMS parseSMSEntry(const String& entry) {
    String number;
    String timestamp;
    String message;

    int firstQuoteIndex = entry.indexOf('"');
    int secondQuoteIndex = entry.indexOf('"', firstQuoteIndex + 1);
    int thirdQuoteIndex = entry.indexOf('"', secondQuoteIndex + 1);
    int fourthQuoteIndex = entry.indexOf('"', thirdQuoteIndex + 1);
    int commaIndexBeforeTimestamp = entry.lastIndexOf(',', entry.lastIndexOf(',') - 1);

    // Extracting ID, status, sender, and timestamp
    //sms.id = entry.substring(6, entry.indexOf(',')).toInt();
    //sms.status = entry.substring(firstQuoteIndex + 1, secondQuoteIndex);
    number = entry.substring(thirdQuoteIndex + 1, fourthQuoteIndex);
    timestamp = entry.substring(commaIndexBeforeTimestamp + 2, entry.indexOf('+', commaIndexBeforeTimestamp) - 1);

    // Extracting the message content
    int messageStartIndex = entry.indexOf('\n') + 1; // Assuming message starts after the newline
    if (messageStartIndex != -1) {
        message = entry.substring(messageStartIndex);
    }

    return SMS(number, message, timestamp);
}

// Splits the entire message string into individual SMS entries and parses them
std::vector<SMS> parseSMSData(const String& data) {
  std::vector<SMS> smsList;
  int start = 0;
  int end = data.indexOf("\n+CMGL: ", start);
  
  while (end != -1) {
    String entry = data.substring(start, end);
    smsList.push_back(parseSMSEntry(entry));
    start = end + 1;
    end = data.indexOf("\n+CMGL: ", start);
  }
  // Adding the last SMS entry, if there's any remaining part
  if (start < data.length()) {
    smsList.push_back(parseSMSEntry(data.substring(start)));
  }

  return smsList;
}


std::vector<SMS> ArduinoPro_Modem::getReadSMS(){
    String rawMessages = sendATCommand("+CMGL=\"REC READ\"");
    if(rawMessages.indexOf("OK") == -1){
        return std::vector<SMS>();
    } else if (rawMessages.indexOf("ERROR") != -1){
        return std::vector<SMS>();
    } else {
        return parseSMSData(rawMessages);
    }
}

std::vector<SMS> ArduinoPro_Modem::getUnReadSMS(){
    String rawMessages = sendATCommand("+CMGL=\"REC UNREAD\"");
    if(rawMessages.indexOf("OK") == -1){
        return std::vector<SMS>();
    } else if (rawMessages.indexOf("ERROR") != -1){
        return std::vector<SMS>();
    } else {
        return parseSMSData(rawMessages);
    }
}