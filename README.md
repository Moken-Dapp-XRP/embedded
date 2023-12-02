# QR Code embedded

## Overview

This project involves a QR code reading and control system using a combination of K210, ESP32, and ESP01 modules. The K210 reads QR codes and communicates the data to an ESP32 via serial UART. The ESP32 then sends this data to an MQTT topic. An ESP01 subscribed to this MQTT topic controls a relay module based on the received MQTT messages.
![overview](https://github.com/Moken-Dapp-XRP/embedded/assets/40807526/bc4a48cf-d3f9-4213-bdf4-2aead5844a7c)

## Demo

TODO

## Components

-   [**Sipeed Maix Bit K210**](https://wiki.sipeed.com/soft/maixpy/en/develop_kit_board/maix_bit.html)
-   [**ESP32**](https://www.espressif.com/en/products/socs/esp32)
-   [**ESP01 with Relay Module**](https://www.instructables.com/ESP0101S-RELAY-MODULE-TUTORIAL/)
-   [**Mini Solenoid Electronic Lock**](https://www.amazon.com/Dc12v-Electric-Cabinet-Small-solenoid/dp/B012KISKU6)

## Wiring Diagram

### K210 to ESP32 Connection:

-   **K210 Ports:**
    -   TX = 10
    -   RX = 15
-   **ESP32 Ports:**
    -   RX2 = 16
    -   TX2 = 17

### ESP01 Setup:

-   **ESP01 Ports:**
    -   Relay = 0

### Data Flow:

-   K210 reads QR codes and sends data to ESP32 via UART.
-   ESP32 forwards QR code data to MQTT topic.
-   ESP01 listens to MQTT topic and controls a relay based on the topic's messages.

## MQTT Topic Interaction

-   **Publisher:** ESP32 sends QR code data.
-   **Subscriber:** ESP01 subscribes and reacts to the MQTT topic.

## Relay Module

-   Connected to ESP01.
-   Controls a Mini Solenoid Electronic Lock (12V).

## Getting Started

1. Connect the K210 to ESP32 using UART (K210 TX to ESP32 RX2, and K210 RX to ESP32 TX2).
2. Connect the relay module to the ESP01.
3. Set up the MQTT broker and configure the ESP32 and ESP01 to connect to it.
4. Deploy the code to each module.
