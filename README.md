# esp_clock

An implementation, to show the current time on a 60 LED neo-pixel ring with an ESP8266.

The time is fetched from a NTP server.

## configuration

* set WLAN SSID and key in time.ino startWiFi
* set NTPServerName in time.ino to your prefered NTP-server.
* set TIMEZONE_OFFSET in time.ino to adjust to your time-zone
* set PIN in time.ino to the pin your neopixel ring is connected to

I use an additional 2nd, smaller ring on pin2 for other notifications - you may just ignore that.
