/*                                                                                                           *
 *  Thank you for building EduDemoS!                                                                         *
 *                                                                                                           *
 *  This file is part of the EduDemoS Project and is licensed under the Creative Commons Attribution         *
 *  NonCommercial ShareAlike 4.0 International license (CC BY-NC-SA 4.0). For full license details, see the  *
 *  license.txt file or visit https://creativecommons.org/licenses/by-nc-sa/4.0/                             *     
 *                                                                                                           *
 *  EduDemoS is co-funded by the European Union. You can find more information at edudemos.eu                *                                                         
 *                                                                                                           *
 *  +-----------------------------------------------------------------------------------------------------+  *
 *                                                                                                           *
 *  This file lets you change certain parameters of the Sun-Tracker and adapt its behaviour.                 */

// WiFi Settings:
/* @todo Update the WiFi settings according to your secrets-sheet */
#define WIFI_SSID           "SECRET_WIFI_SSID"      //  Name of your WiFi network
#define WIFI_PASSWORD       "SECRET_WIFI_PASSWORD"  //  Password of your WiFi network
#define WIFI_MAX_ATTEMPTS   20                      //  How many times to attempt to connect
                                                    //  before resuming without WiFi

// MQTT Settings:
/* @todo Update the MQTT settings according to your secrets-sheet, replace the XX 
         with your 2 digit team number, inlcuding leading zero. */
#define MQTT_WORKSHOP_ID        "undefined"                 // Workshop ID as provided
#define MQTT_TEAM_ID            "xx"                        // Team ID (two digits, leading zero - e.g. "01" or "10")
#define MQTT_USERNAME           "MQTT_USERNAME"             // MQTT Username
#define MQTT_PASSWORD           "MQTT_PASSWORD"             // MQTT Password

#define MQTT_SERVER             "iot-mqtt-broker.gbssg.ch"  // MQTT Broker Address
#define MQTT_PORT               8883                        // MQTT Broker Port
#define MQTT_UPDATE_PERIOD      1000                        // [ms] Time to wait before sending data to MQTT
#define MQTT_MAX_ATTEMPTS       10

#define LED_BUILTIN_AUX         16                          // Pin for the on module LED

// Serial Settings:
#define SERIAL_BAUD_RATE    115200    // [bits/s] // Communication speed for USB Connection
