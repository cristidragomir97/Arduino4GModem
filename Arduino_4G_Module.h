#ifndef ARDUINO_4G_MODULE_H
#define ARDUINO_4G_MODULE_H

#if defined(ARDUINO_CELLULAR_DEBUG)

#endif 

#define DUMP_AT_COMMANDS
#define TINY_GSM_RX_BUFFER 1024
#define TINY_GSM_MODEM_BG96
#define TINY_GSM_DEBUG  Serial

#include <Arduino.h>
#include <StreamDebugger.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

class Arduino_4G_Modem : public TinyGsmBG96 {
public:
  explicit Arduino_4G_Modem(Stream& stream, int power_pin) : TinyGsmBG96(stream),stream(&stream),power_pin(power_pin) {
    
  };
  bool init(const char* pin = NULL) {
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, HIGH);
    delay(1000);
    #ifdef DUMP_AT_COMMANDS
      #if defined(ARDUINO_PORTENTA_C33)
        extern UART Serial1_FC;
        Serial1_FC.begin(115200);
      #endif
    #else
    ((arduino::HardwareSerial*)stream)->begin(115200);
    #endif
    return TinyGsmBG96::init();
  };
public:
  Stream* stream;
  int power_pin;
};

extern Arduino_4G_Modem modem;



#endif
