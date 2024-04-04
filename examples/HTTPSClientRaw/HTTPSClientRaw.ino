
#define ARDUINO_PRO_MODEM_DEBUG
#define LOGGING 
#define DEBUGSERIAL Serial

#include <Arduino.h>
#include "SSLClient.h"
#include "arduino_secrets.h"
#include "ArduinoProModem.h"


const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";

const char server[] = "arduino.cc";
const char resource[] = "/pro";
const int  port       = 443;



ArduinoCellularModem fourgee = ArduinoCellularModem();
TinyGsmClient networkClient = fourgee.getNetworkClient();
SSLClient ssl_client(&networkClient);
HttpClient http = HttpClient(ssl_client, server, port);

unsigned long getTime() {
    Time fourgeeTime = fourgee.getCellularTime();
    Serial.println(fourgeeTime.getISO8601());
    return fourgeeTime.getUNIXTimestamp();
}

void setup(){
       Serial.begin(115200);
    while (!Serial);
 
    fourgee.begin();
    fourgee.connect(apn, gprsUser, gprsPass);

    Serial.println(fourgee.isConnectedToInternet() ? "Connected to the internet" : "Not connected to the internet");
    fourgee.getConnectionStatus();

    ssl_client.setCACert(root_ca);
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