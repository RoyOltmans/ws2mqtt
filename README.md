# Updated version - ESP32 WROOM 32D

I Wanted the new features of from MattFryer based on the base version of Tho85, I have been using this setup for a while with pleasure!
Thank you Tho85 and MattFryer for all the effort and time! 
See the original code from Tho85 https://github.com/Tho85/ws2mqtt and the code from MattFryer https://github.com/MattFryer/ws2mqtt

I had trouble with compiling and building the code via platformio based on MattFryer setup, this was based on a fresh install. 
My board a ESP32-WROOM-32D, I used this details for in platformio.ini:
[env]
platform = espressif32
board = esp32dev
framework = arduino

Afterwards I made multiple corrections to get it to compile, below a summary:

**DEVICE_ID Conflict:**  
  - Removed (or commented out) the `DEVICE_ID` definition in **wisafe2_protocol.h** so that the value defined in **config.h** (0x317E33) is used consistently.

**Packet Structures & Macros:**  
  - Ensured that the packet structures for silence and emergency messages (`pkt_tx_silence_button_t` and `pkt_tx_emergency_button_t`) are defined in **wisafe2_protocol.h**.
  - Defined the corresponding macros (`SPI_TX_SILENCE_BUTTON`, `SPI_TX_EMERGENCY_BUTTON`, and `SPI_STOP_WORD`) in the same header.

**ESP32 Chip Info:**  
  - Replaced the ESP8266-specific call `ESP.getChipModel()` in **homeassistant.cpp** with ESP32-compatible code using `esp_chip_info()` to retrieve chip details.

**String Comparison Fix:**  
  - Changed direct pointer comparisons (e.g., `if (unit != "None")`) to use `strcmp()` for proper C-string comparison.
  - Added `#include <string.h>` at the top of **homeassistant.cpp** to support the `strcmp()` function.

**Format Specifier Corrections:**  
  - Updated `sprintf()` calls to match variable types:
    - In **homeassistant.cpp**, replaced `%d` with `%lld` for printing `int64_t` uptime values and increased the buffer size.
    - In **wisafe2_rx.cpp**, replaced `%d` with `%lu` for printing `device_id` (cast as `unsigned long`).
    - Changed `%x` to `%llx` (with a cast to `(unsigned long long)`) when printing 64-bit hexadecimal values like the SID map.

# ws2mqtt - WiSafe2 to MQTT gateway

ws2mqtt is an inofficial gateway to connect WiSafe2 smoke, heat and CO detectors to your smart home.

The gateway consists of an ESP32 module that handles Wifi and MQTT communication and integrates with Home Assistant through MQTT auto discovery. It is connected to an Arduino that acts as an SPI/UART converter. The Arduino talks to an official WiSafe2 module that is connected to your smoke detector network.

## Disclaimer

This project is not related to or endorsed by the manufacturer of WiSafe2 devices. It is an unofficial hobby project.

:warning: **DO NOT USE THIS PROJECT FOR SAFETY-CRITICAL PURPOSES!** :warning:

This is a hobby project after all, so do not rely on it for your personal safety or the saftey of others. Always assume that it will not work in a case of emergency, and plan your emergency procedures accordingly. As the [license](LICENSE.md) states in more detail, I won't be liable for any damages incurred by this project.

## Pictures

![image](doc/pcb1.jpg)

![image](doc/pcb2.jpg)

![image](doc/homeassistant1.png)

![image](doc/homeassistant2.png)

## Supported devices

Tested:

- ST-630-DE(P) Smoke detector
- HT-630-EUT Heat detector

Not tested, but should be working:

- WST-630 Smoke alarm
- W2-CO-10X CO alarm
- FP1720W2-R Smoke alarm
- FP2620W2-R Smoke alarm

## Features

- [x] Smoke / heat detector states:
  - [x] Smoke / heat detected
  - [x] Battery state
  - [x] Radio module battery state
  - [x] Device attached to base
- [x] Test button detection
- [x] Combined test button / detection event (see Tips & troubleshooting)
- [x] Test all devices from Home Assistant

TODO:

- [ ] Test single device from Home Assistant
- [ ] Logging via MQTT
- [ ] ESP32 over-the-air updates

## Additional Hardware required

- [ESP32-DevKitC V4](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/hw-reference/esp32/get-started-devkitc.html)
- [Arduino Pro Mini](https://www.arduino.cc/en/pmwiki.php?n=Main/ArduinoBoardProMini), or chinese clone
- [WiSafe2 module](https://www.directtradesupplies.co.uk/product.php/199997/fireangel-wisafe2-module--wireless-alarm-pairing--all-wisafe2--mains---battery-only--alarms-can-be-paired---white-)
- A 868 MHz antenna, e.g. stripped from a defective detector. Alternatively, an 8.8 cm long wire
- Custom PCB (see `PCB` folder). Alternatively, lots of breadboard wires (see [schematic](PCB/schematic.pdf))

## Setup

- Setup Arduino
  - Mount Arduino to PCB
  - Flash Arduino firmware (see `Firmware-Atmega` folder) via programmer
- Setup ESP32
  - Mount ESP32 to PCB
  - Configure your secrets in `Firmware-ESP/include/credentials.h` (see example)
  - Flash ESP32 firmware (see `Firmware-ESP` folder) via USB
- Power board via ESP32 USB connector
- Setup WiSafe2 module
  - Mount WiSafe2 module to PCB
  - Join WiSafe2 network
    - Push button on module for 5 seconds
    - Module LED should light up
    - Within 5 seconds, push test button on a different detector
    - Module LED should blink multiple times as a confirmation
- Mount ESP32, Arduino and WiSafe2 module on PCB
- Configure Home Assistant
  - The gateway and all detectors should be visible in the "Devices" tab
  - Create an automation that listens for the `alarm` event emitted by the gateway, see below

## Tips & troubleshooting

* The recommended way to react to smoke detection and test events in Home Assistant is to listen to the `alarm` event emitted by the `ws2mqtt Bridge` device. This event is fired when any of the connected detectors detects smoke, or when any of the detectors' test buttons is pressed. This way you can test your emergency scenes and automations by pressing a detector's test button.

* _The "smoke detected" sensor shows an unknown state_

  This is actually a deficiency of the WiSafe2 protocol. As far as I know, there is no way to query a device for the current smoke / heat detector state. Additionally, a "smoke detected" message is only sent once to other detectors, and not repeated later. So if you start the gateway while a "smoke detected" event is ongoing, there is no way for the gateway to know about that event. I decided to err on the side of caution and show the state of devices as "unknown".

* _The "battery" sensor, "docked" sensor etc. show an unknown state_

  To conserve battery power, the gateway does not actively query the state of these diagnostic sensors. The sensor state is only updated on change. The easiest way to force an update is to remove a detector from its base, wait a bit, and re-mount the device. This should update not only the "docked" sensor, but also the battery ones.

* What is the programming language you used, and why is it not perfect C?

  C is not my mother tongue, so it's nearly certain that there are race conditions, buffer overflows or bad C programming practices in the code. Feel free to file an issue to point things out, or even better file a pull request.

## License

See [LICENSE](LICENSE.md).

## Final notes

Shout out to **C19HOP** and his [WiSafe2-to-HomeAssistant-Bridge](https://github.com/C19HOP/WiSafe2-to-HomeAssistant-Bridge), for helping in reversing the protocol, and for giving me the motivation to finish this project I've been working on for the last months.
