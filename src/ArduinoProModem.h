#ifndef ARDUINO_PRO_MODEM_H
#define ARDUINO_PRO_MODEM_H

#include <Arduino.h>
#include <vector>
#include <Arduino_4G_Module.h>


class SMS {
    public:
        String number;
        String message;
        String timestamp;
        
        SMS(String number, String message, String timestamp) {
            this->number = number;
            this->message = message;
            this->timestamp = timestamp;
        }
};

/**
 * @class ArduinoPro_Modem
 * @brief Represents an Arduino Pro Modem.
 * 
 * This class provides methods to interact with the Arduino Pro Modem, such as connecting to the network,
 * sending SMS messages, getting GPS location, and more.
 */
class ArduinoPro_Modem {
    public: 
        /**
         * @brief Default constructor.
         */
        ArduinoPro_Modem();

        /**
         * @brief Initializes the modem.
         */
        void begin();

        /**
         * @brief Connects to the network using the specified APN, GPRS username, and GPRS password.
         * @param apn The Access Point Name.
         * @param gprsUser The GPRS username.
         * @param gprsPass The GPRS password.
         * @return True if the connection is successful, false otherwise.
         */
        bool connect(const char * apn, const char * gprsUser, const char * gprsPass);


        /**
         * @brief Checks if the modem is registered on the network.
         * @return True if the network is connected, false otherwise.
         */
        bool isConnectedToOperator();

        /**
         * @brief Checks if the GPRS network is connected.
         * @return True if the GPRS network is connected, false otherwise.
         */
        bool isConnectedToInternet();


        /**
         * @brief Gets the SIM card status.
         * @return The SIM card status.
         */
        SimStatus getSimStatus();

        /**
         * @brief Unlocks the SIM card using the specified PIN.
         * @param pin The SIM card PIN.
         * @return True if the SIM card is unlocked, false otherwise.
         */
        bool simUnlock(const char * pin);

        /**
         * @brief Waits for network registration.
         * @return True if the network registration is successful, false otherwise.
         */
        bool waitForNetworkRegistration();

        /**
         * @brief Enables or disables the GPS module.
         * @param assisted True to enable assisted GPS, false to disable it. Assist GPS uses the network to get the GPS location faster, so cellular needs to be enabled. 
         * @return True if the GPS module is enabled, false otherwise.
         */
        bool enableGPS(bool assisted);

        /**
         * @brief Gets the GPS location.
         * @param latitude Pointer to store the latitude.
         * @param longitude Pointer to store the longitude.
         * @return True if the GPS location is retrieved, false otherwise.
         */
        bool getGPSLocation(float* latitude, float* longitude);

        /**
         * @brief Gets the current time from the network.
         * @param year Pointer to store the year.
         * @param month Pointer to store the month.
         * @param day Pointer to store the day.
         * @param hour Pointer to store the hour.
         * @param minute Pointer to store the minute.
         * @param second Pointer to store the second.
         * @return True if the time is retrieved, false otherwise.
         */
        bool getTime(int * year, int * month, int * day, int * hour, int * minute, int * second);

        /**
         * @brief Sends an SMS message to the specified number.
         * @param number The phone number to send the SMS to.
         * @param message The message to send.
         * @return True if the SMS is sent successfully, false otherwise.
         */
        bool sendSMS(const char * number, const char * message);

        /**
         * @brief Gets the list of read SMS messages.
         * @return A vector of SMS messages.
         */
        std::vector<SMS> getReadSMS();

        /**
         * @brief Gets the list of unread SMS messages.
         * @return A vector of SMS messages.
         */
        std::vector<SMS> getUnReadSMS();

        /**
         * @brief Sends an AT command to the modem and waits for a response, then returns the response.
         * @param command The AT command to send.
         * @return The response from the modem.
         */
        String sendATCommand(char * command);

        /**
         * @brief Gets the GSM client.
         * @return The GSM client.
         */
        TinyGsmClient getGSMClient();

        /**
         * @brief Gets the HTTP client for the specified server and port.
         * @param server The server address.
         * @param port The server port.
         * @return The HTTP client.
         */
        HttpClient getHTTPClient(const char * server, const int port);

    private:
        bool connectToGPRS(const char * apn, const char * gprsUser, const char * gprsPass);
        TinyGsmClient * client; /**< The GSM client. */
};


#endif