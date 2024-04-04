#define DUMP_AT_COMMANDS 

#include "ArduinoProModem.h"



ArduinoCellularModem fourgee = ArduinoCellularModem();

float lat = 0.00000;
float lon = 0.00000;
int year = 0;
int month = 0;
int day = 0;
int hour = 0;
int minute = 0;
int second = 0;


const char apn[]      = "internet";
const char gprsUser[] = "";
const char gprsPass[] = "";

void setup(){
    Serial.begin(115200);
    while (!Serial);
    fourgee.begin();
    //fourgee.connect(apn, gprsUser, gprsPass);
    fourgee.enableGPS(false);
}

void loop(){
    Location loc = fourgee.getGPSLocation(10000);
    Serial.print("Latitude: "); Serial.println(lat);
    Serial.print("Longitude: "); Serial.println(lon);

    Time t = fourgee.getGPSTime();
    Serial.print("ISO String: "); Serial.println(t.getISO8601());
    
    delay(1000);

}