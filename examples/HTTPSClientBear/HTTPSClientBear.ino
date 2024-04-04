
//#define ARDUINO_PRO_MODEM_DEBUG
#define LOGGING 
#define DEBUGSERIAL Serial
#define GSM_PIN ""

#include <Arduino.h>
#include <ArduinoBearSSL.h>
#include <Arduino_4G_Module.h>
#include <ArduinoHttpClient.h>
#include "arduino_secrets.h"

const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";

const char server[]   = "www.howsmyssl.com";
const char resource[] = "/a/check";
const int  port       = 443;

TinyGsmClient client(modem);
BearSSLClient sslClient(client);
HttpClient http = HttpClient(sslClient, server, port);

unsigned long getTime() {
    Serial.println("Getting time...");
    //Time fourgeeTime = fourgee.getCellularTime();
    //Serial.println(fourgeeTime.getISO8601());
    //return fourgeeTime.getUNIXTimestamp();
    return 1712138142;
}

void setup(){
    Serial.begin(115200);
    while (!Serial);
 
    Serial.println("Initializing modem...");
    modem.init();

    Serial.println(TINY_GSM_RX_BUFFER);

    String modemInfo = modem.getModemInfo();
    Serial.print("Modem Info: ");
    Serial.println(modemInfo);

    // Unlock your SIM card with a PIN if needed
    if (GSM_PIN && modem.getSimStatus() != 3) { modem.simUnlock(GSM_PIN); }

    Serial.print("Waiting for network...");
    if (!modem.waitForNetwork()) {
      Serial.println(" fail");
      delay(10000);
      return;
    }

    Serial.println(" success");

    if (modem.isNetworkConnected()) { Serial.println("Network connected"); }

    // GPRS connection parameters are usually set after network registration
    Serial.print(F("Connecting to "));
    Serial.print(apn);
    if (!modem.gprsConnect(apn, gprsUser, gprsPass)) {
      Serial.println(" fail");
      delay(10000);
      return;
    }
    Serial.println(" success");
    ArduinoBearSSL.onGetTime(getTime);

    delay(1000);
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