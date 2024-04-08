#include "ArduinoProModem.h"
#include <ArduinoMqttClient.h>

bool ledStatus = false;
uint32_t lastReconnectAttempt = 0;

// APN data
const char apn[]      = "live.vodafone.com";
const char gprsUser[] = "live";
const char gprsPass[] = "";


// MQTT topics
const char* topicLed       = "GsmClientTest/led";
const char* topicInit      = "GsmClientTest/init";
const char* topicLedStatus = "GsmClientTest/init";

ArduinoCellularModem cellularModem = ArduinoCellularModem();
TinyGsmClient client = cellularModem.getNetworkClient();
MqttClient mqttClient(client);

const char broker[] = "broker.emqx.io";
int        port     = 1883;
const char topic[]  = "arduino/simple";

void mqttCallback(char* topic, byte* payload, unsigned int length) {
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");
    for (int i = 0; i < length; i++) {
        Serial.print((char)payload[i]);
    }
    Serial.println();
}

void setup(){
    Serial.begin(115200);
    while (!Serial);

    cellularModem.begin();
    cellularModem.connect(apn, gprsUser, gprsPass);
    Serial.println(cellularModem.isConnectedToInternet());

    mqttClient.setId("clientId7464");

    Serial.print("Attempting to connect to the MQTT broker: ");
    Serial.println(broker);

    while (!mqttClient.connect(broker, 1883)) {
        Serial.print("MQTT connection failed! Error code = ");
        Serial.println(mqttClient.connectError());

        delay(1000);
    }

    Serial.println("You're connected to the MQTT broker!");
    Serial.println();

    // set the message receive callback
    mqttClient.onMessage(onMqttMessage);

    Serial.print("Subscribing to topic: ");
    Serial.println(topic);
    Serial.println();

    // subscribe to a topic
    mqttClient.subscribe(topic);

    // topics can be unsubscribed using:
    // mqttClient.unsubscribe(topic);

    Serial.print("Waiting for messages on topic: ");
    Serial.println(topic);
    Serial.println();

}


void loop() {
  // call poll() regularly to allow the library to receive MQTT messages and
  // send MQTT keep alives which avoids being disconnected by the broker
  mqttClient.poll();


}

void onMqttMessage(int messageSize) {
  // we received a message, print out the topic and contents
  Serial.println("Received a message with topic '");
  Serial.print(mqttClient.messageTopic());
  Serial.print("', length ");
  Serial.print(messageSize);
  Serial.println(" bytes:");

  // use the Stream interface to print the contents
  while (mqttClient.available()) {
    Serial.print((char)mqttClient.read());
  }
  Serial.println();

  Serial.println();
}