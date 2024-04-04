# Arduino 4G Module Library

This library provides a toolkit for interacting with the official Arduino 4G Modules. It allows you to connect to the internet, send and receive SMS messages, and get location from the cellular network or GPS. 

## Examples
* [examples/HTTPClient]() - Example of using this library together with 
* [examples/MQTTClient]() -
* [examples/SMSReceive]() -
* [examples/TimeAndLocation]() -
* [examples/ModemTerminal]() - 

## Features
* Fast 4G connection to the internet
    * Implementation compatible with the HTTPClient class, so it could be a drop-in replacement for any WiFi based solution
* Get location using GPS or GSM
* Synchronise time with the cell provider
* Send and Receive SMS Messages