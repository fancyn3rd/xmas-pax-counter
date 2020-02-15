# xmas-pax-counter

Pax counter for my [Interactive xmas tree](https://github.com/fancyn3rd?tab=projects#interactive-xmas-tree) project.    
Its a wifi device counter (wifi-sniffer) for an esp32 microcontroller.    
It is based on the [ESP8266 WifiSniffer](https://github.com/kalanda/esp8266-sniffer)    
Much thanks goes out to [Marzac](https://github.com/Marzac/) who supported me on this project.

The controller scans for wifi devices and when a new device is found, its mac adress is temoprarely stored in an array.
When the device is not found anymore for 30 seconds, the device is removed from the array.

Its originally made for the TTGO board which also has a OLED display, which is used to display the number of the found devices; But it should work with any other esp32 controller as well. Just make sure so set the flag HAS_DISPLAY to false, when there is no dislay available.

Requierements for deployment:

* Visual Stduio Code / Atom
* PlatformIO Extension
