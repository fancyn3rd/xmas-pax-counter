# xmas-pax-counter

Pax counter for the interactive xmas tree project.
Technically its wifi-sniffer for an esp32 microcontroller.
It is based on the ESP8266 WifiSniffer: https://github.com/kalanda/esp8266-sniffer

The controller scans for other wifi devices and when a new device is found, its mac adress is temoprarely stored in an array.
When the device is not found anymore for 30 seconds, the device is removed from the array.

Its originally made for the ttgo board which also has a OLED display, which is used to display the count of the found devices; But it should work with any other esp32 controller as well. Just make sure so set the flag HAS_DISPLAY to false, when there is no dislay available.

Requierements for deployment:
- Visual Stduio Code / Atom
- PlatformIO Extension
