# IoT Demonstrator "iot_minimal" ( 🌐 IoT-Ready )

This sketch (`.ino`-file) contains a minimal demonstrator for the IoT infrastructure. The software features a WiFi/MQTT-based uplink to an MQTT broker.

## Running this application

In order to run this application execute the following steps:

1. Create a copy of the `configuration.default.cpp` file, acting as a template. Name the copy to `configuration.cpp`.
2. Open `configuration.cpp` for editing.
3. Fill in the WiFi’s data with the  information provided by your workshop host:
   - SSID
   - Password
4. Fill in the MQTT data as assigned by your workshop host:
   - Workshop ID
   - Team ID
   - Username
   - Password
5. Doublecheck the MQTT configuration with your workshop instructions:
   - Server
   - Port

## Dashboard

An example for a dashboard to be used with this demo can be found as [`iot_demo_base.json`](../../dashboards/iot_demo_base.json) in this repository. 

To use this dashbaord

1. Import the JSON file as ThingsBoard dashboard
2. Enter edit mode
3. For each widget, adjust the source device according to your configuration
4. Save

## MQTT concept

This project exposes multiple datapoints that can be used. Inputs and outputs are separated by their path:

- Inputs: `ctrl/...`
- Outputs `data/...`

The available datapoints are:

| **Key**     | **Direction** | **MQTT path**    | **Format**              | **Description**                                       |
| :---------- | :-----------: | :--------------- | :---------------------- | :---------------------------------------------------- |
| _beatcount_ |      Out      | `data/beats`     | [Scalar](#scalars)      | Number of heartbeats sent so far                      |
| _heartbeat_ |      Out      | `data/heartbeat` | [Heartbeat](#heartbeat) | Heartbeat signal sent every second to signal activity |
| _mirror_    |      Out      | `data/mirror`    | [Plain](#plain-values)  | see [Mirror feature](#mirror-feature)                 |
| _mirror_    |      In       | `ctrl/mirror`    | [Plain](#plain-values)  | see [Mirror feature](#mirror-feature)                 |

### Plain values

Plain values do not have any specific format. The values are transferred without any special markup.

### Scalars

For simplified processing on the ThingsBoard end and better forward compatibility, every data point is wrapped within a JSON-object. For scalar values, the object is structured as follows:

```json
{
  "value": <the value>
}
```

Objects of such a structure are automatically interpret by the EduDemoS ThingsBoard instance to scalar key-value-pairs. However, it also allows for further extension of the datapoint without breaking existing data formats by simply adding further keys to the JSON-object in the future.

### Heartbeat

The heartbeat signal is a JSON-object consisting of two keys:

- state - a toggling string-value switching between `on` and `off` automatically.
- time  - number of milliseconds since bootup.

```json
{
  "state": "off",
  "time": 651982
}
```

### Mirror feature

For demonstrating downlink (i.e. sending through the broker to the device), the software features a mirroring service. This service exposes an input under `ctrl/mirror` and an output through `data/mirror`.

The service follows the following logic:

Whenever a value is set for `ctrl/mirror`, the value is interpret by the following rule:

|  `ctrl/mirror`  | `data/mirror` |
| :-------------: | :-----------: |
|       on        |      off      |
| _anything else_ |      on       |

## Licensing

Please note: different licenses apply depending on the type of content.

All documents and 3d models (files ending with .pdf and .stl) are licensed under [CC-BY-NC-SA 4.0](https://creativecommons.org/licenses/by-nc-sa/4.0/).

All files containing code are licensed under [GNU GPL V3](https://www.gnu.org/licenses/gpl-3.0.txt).

## Disclaimer

Funded by the European Union. Views and opinions expressed are however those of the author(s) only and do not necessarily reflect those of the European Union or the European Education and Culture Executive Agency (EACEA). Neither the European Union nor EACEA can be held responsible for them.
