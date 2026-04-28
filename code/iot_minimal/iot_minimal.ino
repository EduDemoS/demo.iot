/*
 * This file is part of the EduDemoS IoT demo which is
 * co-funded by the European Union.
 * Copyright (C) 2025  Gerda Stetter Stiftung - Technik macht Spaß!
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>. */

#include <Arduino.h>

/*  To setup your project:
    1. Create a copy of configuration.default.cpp
    2. Name the copy "configuration.cpp"
    3. Adjust the settings in configuration.cpp according to your needs 
       (look for @todo-comments) */
#include "configuration.cpp"

// Include Libraries
#include <Ticker.h>

#include <MqttClient.h>
#include <WiFiSecureClientProvider.h>

#include <SimpleSoftTimer.h>
#include <SimpleStateProcessor.h>

using namespace HolisticSolutions;
using namespace HolisticSolutions::WiFi;
using namespace HolisticSolutions::Mqtt;

/* Combine demonstrator name from configuration */
#define DEMONSTRATOR_TYPE_NAME  "IOTDEMO"
#define MQTT_DEVICE_NAME        DEMONSTRATOR_TYPE_NAME MQTT_TEAM_ID

/* Recurring tasks to be executed repeatedly */

static void TaskCommunication(void);
static void TaskHeartbeat(void);

/* Callback functions */
static MQTT_MESSAGE_HANDLER_DECLARE(OnMirrorReceived);

/* Utilitiy functions */
static bool WiFiConnect();
static bool MqttConnect();

static void LogMessage(const char *topic, const void *data, size_t len);
static void SendJsonDoc(const char *topic, const JsonDocument &doc, bool retain = false);

/*! \brief  Template function to simplify datapoint transmission 

            This utility function wraps the value given in a JSON document according to the needs as setup 
            through the cloud.

    \param[in]  topic   Name of the topic, the data is to be sent for 
    \param[in]  value   Value to be sent
    \param[in]  retain  
                        - true    if the value shall be retained (stored) on the broker 
                        - false   if the value shall not be retained
    \param[in]  key     Key the value shall be associated with (optional, default name: 'value') */
template <typename T>
void SendDatapoint(const char *topic, const T& value, bool retain = false, const char *key = "value") {
  JsonDocument data;
  data[key] = value;
  SendJsonDoc(topic, data, retain);
}

// Create WiFi and MQTT objects:
static WiFiSecureClientProvider  upstream;
static MqttClient                mqtt(upstream);

static SimpleSoftTimer _heartbeat_timeout(MQTT_UPDATE_PERIOD / 2);  /*!< Timer object for heartbeat timing */

static bool     _mirror = false;  /*!< Mirroring value, controlled through MQTT. */
static bool     _toggle = false;  /*!< Toggle bit signalling heartbeat. */
static uint32_t _beatcount  = 0;  /*!< Number of heartbeats sent since bootup. */

void setup() {
#if defined (ESP8266)
  // Double the CPU Frequency:
  os_update_cpu_frequency(160);
#endif

  Serial.begin(SERIAL_BAUD_RATE);
  /* Wait for the serial channel to be initialized */
  while(!Serial) delay(1);

  Serial.println("Starting IoT minimal demo");

  pinMode(LED_BUILTIN_AUX,  OUTPUT);
  digitalWrite(LED_BUILTIN_AUX, HIGH);

  Serial.print("Resetting WiFi...");
  upstream.reset();
  Serial.println("done");
  
  Serial.print("Resetting MQTT client...");
  mqtt.reset();
  Serial.println("done");

  WiFiConnect();
  MqttConnect();

  /* Register a callback that is expected to be triggered, whenever ctrl/mirror is updated */
  mqtt.subscribe("ctrl/mirror", MQTT_MESSAGE_HANDLER_NAME(OnMirrorReceived), 0);
}

void loop() {
  TaskCommunication();
  TaskHeartbeat();
}

/*! \brief  Task maintaining the cloud connection. */
static void TaskCommunication() {
  upstream.run();
  if (upstream.connected()) {
    mqtt.run();
  }
}

/*! \brief  Task running the heartbeat logic */
static void TaskHeartbeat() {
  if (_heartbeat_timeout.isTimeout()) {
    _heartbeat_timeout.restart();

    if (!mqtt.connected()) {
      Serial.println("...and my heart skips, skips a beat (no connection)");
      return;
    }

    /* Heartbeat construction and transmission */
    JsonDocument heartbeat;
    heartbeat["state"] = _toggle ? "on" : "off";
    heartbeat["time"] = millis();
    SendJsonDoc("data/heartbeat", heartbeat);
    _beatcount++;

    /* Update number of beats sent. */
    SendDatapoint("data/beats", _beatcount);

    _toggle = !_toggle;
  }
}

/*! \brief  Setup the WiFi connection according to the configuration 

    \return 
            - true  if setup succeeded
            - false if setup failed */
static bool WiFiConnect()
{
  Serial.print("Connecting to WiFi network '");
  Serial.print(WIFI_SSID);
  Serial.print("' .");

  // Attempt WiFI connection:
  upstream.reset();
  upstream.connect(WIFI_SSID, WIFI_PASSWORD);

  return true;
}

/*! \brief Setup connection with the MQTT Broker.

    \return 
            - true  if setup succeeded
            - false if setup failed */
static bool MqttConnect()
{
  Serial.print("Connecting to MQTT server  '" MQTT_SERVER "' .");

  /* Only use this mode for experimental setups, 
    refrain from any productive use!*/
  mqtt.InsecureAccept();
  
  if ((strlen(MQTT_WORKSHOP_ID) > 0) 
      && (strcmp(MQTT_WORKSHOP_ID, "undefined") != 0))
  {
    mqtt.TopicPrefixSet("EduDemoS/" MQTT_WORKSHOP_ID "/" MQTT_DEVICE_NAME);
  }
  else 
  {
    Serial.println("WARNING: Workshop ID not set");
    mqtt.TopicPrefixSet("EduDemoS/WSxxx/" MQTT_DEVICE_NAME);
  }

  mqtt.CredentialsSet(MQTT_USERNAME, MQTT_PASSWORD);
  mqtt.connect(MQTT_WORKSHOP_ID "-" MQTT_DEVICE_NAME, MQTT_SERVER, MQTT_PORT);

  return true;
}

/*! \brief  Sends the JSON document given through \p doc to the MQTT broker

    \param[in]  topic   Topic name the document shall be sent for
    \param[in]  doc     JSON document to be sent for the given \p topic
    \param[in]  retain  
                        - true    if the value shall be retained (stored) on the broker 
                        - false   if the value shall not be retained */
static void SendJsonDoc(const char *topic, const JsonDocument &doc, bool retain) {
  String payload;
  serializeJson(doc, payload);
  mqtt.publish(topic, payload, retain);
}

/*! \brief  Callback function reacting on writing to the topic it was registered to. 

            To showcase the mechanism, the data received is logged and tested for the string
            "on". If "on" is received,  _mirror is set to false and vice versa. This updated
            state is then published again through the topic data/mirror.

    \param[in]    context   Context the callback was registered with
    \param[in]    topic     Topic name the write action was received for
    \param[in]    data      Data received for the \p topic
    \param[in]    len       [byte] Length of the \p data received */
static MQTT_MESSAGE_HANDLER_DECLARE(OnMirrorReceived) {
  const char *payload = (const char *)data;

  LogMessage(topic, data, len);

  _mirror = strncmp("on", payload, 2) == 0;
  mqtt.publish("data/mirror", _mirror ? "off" : "on");
}

/*! \brief  Logs an MQTT message received to the serial port
    
    \param[in]    topic   Topic name associated with the message
    \param[in]    data    Pointer to the data blob received
    \param[in]    len     Length of the data blob received 
    
    \note This function only works for text content and 
          fails to render binary content properly */
static void LogMessage(const char *topic, const void *data, size_t len) {
  const char *message = (const char *)data;
  String text;
  
  text.concat(message, len);
  
  Serial.println("Topic: '" + String(topic) + "' Message: '" + text + "'");
}
