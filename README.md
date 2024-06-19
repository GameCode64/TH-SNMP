# Yet another IoT Device to measure your temperatures and humidity

## Libraries
 - [SNMP_Agent by Aidan Cyr](https://github.com/0neblock/Arduino_SNMP)
 - [ClosedCube_HDC1080 by ClosedCube](https://github.com/closedcube/ClosedCube_HDC1080_Arduino) (Only applyable when using the HDC1080 unit)
 - [TFT_eSPI by Bodmer](https://github.com/Bodmer/TFT_eSPI)
 - [Adafruit_SHT4x By Adafruit](https://github.com/adafruit/Adafruit_SHT4x) (Only applyable when using a SHT4x unit, couldn't test [for now] due to a faulty unit)

## Configuration
Replace the SSID and W_PWD (Wifi Password) with your own credentials. These are just placeholder ;)
The current setup is specific for the LilyGo/TTGO T-display.

To use a normal ESP32 or ESP32c3 just set the TTGO definition to `false`
When using a ESP32/ESP32c3 the output that is normally shown on screen of the TTGO will appear in the console on Baudrate `115200`
