#include "ArduinoProModem.h"

const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";


const char apiKey[]   = "86bba1ae6c7ecf6de0df290fb1d0a795";

float lat = 51.5074;
float lon = 0.1278;

ArduinoCellularModem cellularModem = ArduinoCellularModem();
//

void setup(){
    Serial.begin(115200);
    while (!Serial);
    cellularModem.begin();
    cellularModem.connect(apn, gprsUser, gprsPass);
}


String httpGETRequest(const char* serverName) {
  HttpClient http = cellularModem.getHTTPClient(serverName, 80);
  String payload = "";
  
  // Send HTTP POST request
   int err = http.get(serverName);
  if (err != 0) {
    Serial.println(F("failed to connect"));
    Serial.println(err);
    return "" ;
  }

  int httpResponseCode = http.responseStatusCode(); 
  
  if (httpResponseCode>0) {
    Serial.print("HTTP Response code: ");
    Serial.println(httpResponseCode);
    payload = http.responseBody();
  }
  else {
    Serial.print("Error code: ");
    Serial.println(httpResponseCode);
  }


  return payload;
}


void loop(){
    String serverPath = "http://api.openweathermap.org/data/2.5/weather?lat=" + String(lat) + "&lon=" + String(lon) + "&appid=" + apiKey;
    Serial.println(serverPath.c_str());
    String payload = httpGETRequest(serverPath.c_str());
    Serial.println(payload);

    delay(10000);
}
