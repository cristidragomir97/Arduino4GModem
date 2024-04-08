#include "ArduinoProModem.h"

ArduinoCellularModem::ArduinoCellularModem() {
    
}

void ArduinoCellularModem::begin() {
    // set sim slot
    modem.init();

    delay(1000);

 
    String modemInfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modemInfo);

    // Set GSM module to text mode
    modem.sendAT("+CMGF=1");
    modem.waitResponse();

    // Send intrerupt when SMS has been received
    modem.sendAT("+CNMI=2,1,0,0,0");
    modem.waitResponse();




}

bool ArduinoCellularModem::connect(const char * apn, const char * gprsUser, const char * gprsPass){
    SimStatus simStatus = getSimStatus();
    if(simStatus == SimStatus::SIM_LOCKED) {
       unlockSIM("1234");
    }

    simStatus = getSimStatus();
    if(simStatus == SimStatus::SIM_READY) {
        if(awaitNetworkRegistration()){
            if(connectToGPRS(apn, gprsUser, gprsPass)){
                Serial.println("Setting DNS...");
                Serial.println(this->sendATCommand("+QIDNSCFG=1,\"8.8.8.8\",\"8.8.4.4\""));
                return true;
            }
        } else {
            return false;
        }
    } else {
        return false;
    }
}


Location ArduinoCellularModem::getGPSLocation(unsigned long timeout){
    float latitude = 0.00000;
    float longitude = 0.00000;
    unsigned long startTime = millis();

    while((latitude == 0.00000 || longitude == 0.00000) && (millis() - startTime < timeout)) {
         modem.getGPS(&latitude, &longitude);
         delay(1000);
    }
    
    Location loc;
    loc.latitude = latitude;
    loc.longitude = longitude;

    return loc;
}

Time ArduinoCellularModem::getGPSTime(){
    int year, month, day, hour, minute, second;
    modem.getGPSTime(&year, &month, &day, &hour, &minute, &second);
    return Time(year, month, day, hour, minute, second);
}

Time ArduinoCellularModem::getCellularTime(){
    int year, month, day, hour, minute, second;
    float tz;
    modem.getNetworkTime(&year, &month, &day, &hour, &minute, &second, &tz);
    return Time(year, month, day, hour, minute, second);
}

bool ArduinoCellularModem::sendSMS(const char * number, const char * message){
    Serial.println("Sending SMS...");
    Serial.println("\t Number: " + String(number));
    Serial.println("\t Message: " + String(message));
    modem.sendSMS(number, message);
}

void ArduinoCellularModem::getConnectionStatus(){

   IPAddress local = modem.localIP();
   Serial.print("Local IP:"); Serial.println(local.toString());
   Serial.println("Signal quality:"); Serial.println(modem.getSignalQuality());
}

TinyGsmClient ArduinoCellularModem::getNetworkClient(){
    return TinyGsmClient(modem);
}

HttpClient ArduinoCellularModem::getHTTPClient(const char * server, const int port){
    TinyGsmClient client(modem);
    return HttpClient(client, server, port);
}

/*
HttpClient ArduinoCellularModem::getHTTPSClient(const char * server, const int port){

    BearSSLClient sslClient(TinyGsmClient(modem));
    return HttpClient(sslClient, server, port);
}*/

bool ArduinoCellularModem::isConnectedToOperator(){
    return modem.isNetworkConnected();
}

bool ArduinoCellularModem::connectToGPRS(const char * apn, const char * gprsUser, const char * gprsPass){
    Serial.print(F("Connecting to 4G network..."));
    while(!modem.gprsConnect(apn, gprsUser, gprsPass)) {
        Serial.print(".");
        delay(2000);
    }
    return true;
}

bool ArduinoCellularModem::isConnectedToInternet(){
    return modem.isGprsConnected();
}

SimStatus ArduinoCellularModem::getSimStatus(){
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

bool ArduinoCellularModem::unlockSIM(const char * pin){
    Serial.println("Unlocking SIM...");
    modem.simUnlock(pin); 
}

bool ArduinoCellularModem::awaitNetworkRegistration(){
    Serial.print("Waiting for network registration...");
    while (!modem.waitForNetwork()) {
        Serial.println(".");
        delay(2000);
    } 
    return true;
}

bool ArduinoCellularModem::enableGPS(bool assisted){
    Serial.println("Enabling GPS...");
    if(assisted){
        sendATCommand("AT+QGPSCFG=\"agpsposmode\",33488767");
    } else {
        sendATCommand("AT+QGPSCFG=\"agpsposmode\",8388608");
    }

    //modem.sendAT(GF("+UTIME=1,1"));
    //modem.waitResponse();
    //modem.sendAT(GF("+UGPIOC=23,0,1"));

    //modem.waitResponse();

    modem.enableGPS();
    //delay(10000);
    // TODO: ^ move this to the sketch 
}

String ArduinoCellularModem::sendATCommand( char * command, unsigned long timeout){
    String resp;
    modem.sendAT(const_cast<char *>(command)); 
    modem.waitResponse(timeout, resp);
    return resp;
}



Time parseTimestamp(const String &timestampStr) {
  int hour, minute, second, day, month, year, offset;
  
  int commaIndex = timestampStr.indexOf(',');
  String date = timestampStr.substring(0, commaIndex);
  String time = timestampStr.substring(commaIndex + 1, timestampStr.indexOf('+'));
  String offsetStr = timestampStr.substring(timestampStr.indexOf('+') + 1);

  int firstSlashIndex = date.indexOf('/');
  int secondSlashIndex = date.lastIndexOf('/');
  day = date.substring(0, firstSlashIndex).toInt();
  month = date.substring(firstSlashIndex + 1, secondSlashIndex).toInt();
  year = date.substring(secondSlashIndex + 1).toInt() + 2000;

  int firstColonIndex = time.indexOf(':');
  int secondColonIndex = time.lastIndexOf(':');
  hour = time.substring(0, firstColonIndex).toInt();
  minute = time.substring(firstColonIndex + 1, secondColonIndex).toInt();
  second = time.substring(secondColonIndex + 1).toInt();

  offset = offsetStr.toInt();

  return Time(year, month, day, hour, minute, second, offset);
}
// Parses a single SMS entry from the data
SMS parseSMSEntry(const String& entry) {
  SMS sms;
  int firstQuoteIndex = entry.indexOf('"');
  int secondQuoteIndex = entry.indexOf('"', firstQuoteIndex + 1);
  int thirdQuoteIndex = entry.indexOf('"', secondQuoteIndex + 1);
  int fourthQuoteIndex = entry.indexOf('"', thirdQuoteIndex + 1);
  int commaIndexBeforeTimestamp = entry.lastIndexOf(',', entry.lastIndexOf(',') - 1);

  // Extracting number and raw timestamp
  sms.number = entry.substring(thirdQuoteIndex + 1, fourthQuoteIndex);
  String rawTimestamp = entry.substring(commaIndexBeforeTimestamp + 2, entry.indexOf('+', commaIndexBeforeTimestamp) + 3);

  // Parse the timestamp
  sms.timestamp = parseTimestamp(rawTimestamp);

  // Extracting the message content
  int messageStartIndex = entry.indexOf('\n') + 1; // Assuming message starts after the newline
  if (messageStartIndex != -1) {
      sms.message = entry.substring(messageStartIndex);
  }

  return sms;
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


std::vector<SMS> ArduinoCellularModem::getReadSMS(){
    String rawMessages = sendATCommand("+CMGL=\"REC READ\"");
    if(rawMessages.indexOf("OK") == -1){
        return std::vector<SMS>();
    } else if (rawMessages.indexOf("ERROR") != -1){
        return std::vector<SMS>();
    } else {
        return parseSMSData(rawMessages);
    }
}

std::vector<SMS> ArduinoCellularModem::getUnreadSMS(){
    String rawMessages = sendATCommand("+CMGL=\"REC UNREAD\"");
    if(rawMessages.indexOf("OK") == -1){
        return std::vector<SMS>();
    } else if (rawMessages.indexOf("ERROR") != -1){
        return std::vector<SMS>();
    } else {
        return parseSMSData(rawMessages);
    }
}