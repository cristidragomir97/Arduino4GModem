
//#define ARDUINO_PRO_MODEM_DEBUG
#define LOGGING 
#define DEBUGSERIAL Serial
#define GSM_PIN ""

#include <Arduino.h>
#include "ArduinoBearSSLConfig.h"
#include <ArduinoBearSSL.h>
#include <ArduinoProModem.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";


const char server[]   = "store.arduino.cc";
const char resource[] = "/products/arduino-nano-33-ble";
const int  port       = 443;

ArduinoCellularModem cellularModem = ArduinoCellularModem();
TinyGsmClient networkClient = cellularModem.getNetworkClient();
BearSSLClient sslClient(networkClient);
HttpClient http = HttpClient(sslClient, server, port);

unsigned long getTime() {
    Serial.print("Getting time... ");
    Time cellularModemTime = cellularModem.getCellularTime();
    Serial.println(cellularModemTime.getISO8601());
    return cellularModemTime.getUNIXTimestamp();
}

void setup(){
    Serial.begin(115200);
    while (!Serial);
 
    cellularModem.begin();
    cellularModem.connect(apn, gprsUser, gprsPass);
    ArduinoBearSSL.onGetTime(getTime);
}


void loop()
{
  Serial.println("Making GET request...");

  http.get(resource);

  int status_code = http.responseStatusCode();
  String response = http.responseBody();

  Serial.print("Status code: ");
  Serial.println(status_code);
  Serial.print("Response: ");
  Serial.println(response);

  http.stop();

  delay(5000);
}