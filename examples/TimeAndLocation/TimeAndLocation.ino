#define DUMP_AT_COMMANDS 

#include "ArduinoProModem.h"



ArduinoPro_Modem fourgee = ArduinoPro_Modem();

float lat = 0.00000;
float lon = 0.00000;

const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";

void setup(){
    Serial.begin(115200);
    while (!Serial);
    fourgee.begin();
    //fourgee.connect(apn, gprsUser, gprsPass);
    fourgee.enableGPS(false);
}

void loop(){
    fourgee.getGPSLocation(&lat, &lon);
    Serial.print("Latitude: "); Serial.println(lat);
    Serial.print("Longitude: "); Serial.println(lon);
    delay(1000);

}