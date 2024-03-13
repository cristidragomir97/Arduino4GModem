#ifndef ARDUINO_4G_MODULE_H
#define ARDUINO_4G_MODULE_H

#define TINY_GSM_MODEM_BG96
#define TINY_GSM_RX_BUFFER 1024
#include <Arduino.h>
#include <StreamDebugger.h>
#include <TinyGsmClient.h>
#include <ArduinoHttpClient.h>

#if defined(ARDUINO_PRO_MODEM_DEBUG)
#define TINY_GSM_DEBUG  Serial
#define DUMP_AT_COMMANDS
#endif 

class Arduino_4G_Modem : public TinyGsmBG96 {
public:
  explicit Arduino_4G_Modem(Stream& stream, int power_pin) : TinyGsmBG96(stream),stream(&stream),power_pin(power_pin) {
    
  };
  bool init(const char* pin = NULL) {
    pinMode(power_pin, OUTPUT);
    digitalWrite(power_pin, HIGH);
    delay(1000);
    #ifdef DUMP_AT_COMMANDS
    // FIXME: this is hugly
    extern UART Serial1_FC;
    Serial1_FC.begin(115200);
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
