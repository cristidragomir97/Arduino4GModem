#include "ArduinoProModem.h"

#define ARDUINO_PRO_MODEM_DEBUG 

ArduinoCellularModem fourgee = ArduinoCellularModem();

boolean newSMS = false;

void newSMSCallback(){
    Serial.println("new sms received");
    newSMS = true;
}

void setup(){
    Serial.begin(115200);
    while (!Serial);
    fourgee.begin();

    // 
    attachInterrupt(digitalPinToInterrupt(A0), newSMSCallback, RISING);

    std::vector<SMS> allMessages = fourgee.getReadSMS();
    for(int i = 0; i < allMessages.size(); i++){
        Serial.println("SMS:");
        Serial.print("\t * From: "); Serial.println(allMessages[i].number);
        Serial.print("\t * Message: "); Serial.println(allMessages[i].message);
        Serial.print("\t * Timestamp: "); Serial.println(allMessages[i].timestamp.getISO8601());
    }

    //delay(1000);
    //fourgee.sendSMS("+41796530528", "Hello from Arduino Pro Modem!");
    delay(1000);
    fourgee.sendSMS("+40788494946", "Hello from Arduino Pro Modem!");
}

void loop(){
    if(newSMS){
        newSMS = false;
        std::vector<SMS> allMessages = fourgee.getUnreadSMS();
        if (allMessages.size() > 0){
            for(int i = 0; i < allMessages.size(); i++){
                Serial.println("SMS:");
                Serial.print("\t * From: "); Serial.println(allMessages[i].number);
                Serial.print("\t * Message: "); Serial.println(allMessages[i].message);
                Serial.print("\t * Timestamp: "); Serial.println(allMessages[i].timestamp.getISO8601());
            }
        } else {
            Serial.println("No new messages");
        }
    }
}