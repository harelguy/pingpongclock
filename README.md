# Ping Pong Clock
Based on the fantastic ping pong clock instructable by tomasj152 (https://www.instructables.com/Ping-Pong-Ball-LED-Clock/).

This build uses the basic build of the clock, but upgrades it into a smart screen with a few built-in apps:

1. A clock app, which pulls the current time from the internet using NTP
1. A "plasma" app which displays cool, color-shifting plasma
1. A "screensaver" app which simply colors the screen with random colors
1. A "draw" app, which you can connect to using your phone and draw anything on the clock face :)

Additionally, the clock has a physical button that switches between the modes. This is optional.

## Hardware
The clock uses an ESP8266 instead of an Arduino. This makes it possible for the clock to connect to the Internet via WiFi. It needs to be configured with a local network. It is slightly smaller than the Arduino as well.

There is also a physical button which can be wired up to the ESP8266, so that no phone is needed to switch between apps.

A Fritzing diagram is included in the `./hardware` folder.

*Note*: The LED strip in my build is wired slightly differently than the original, so tweaks are necessary to make the script work with the original strip direction (TBD).

## Software
The software includes 2 main parts, the software for the ESP8266, and the web application which can run on your phone. The web application is also "hosted" by the ESP8266. This is done by using the ESP8266 as a web server, which serves files from the SPIFFS file system. 

## Deploying the software
The code in the `./src` directory needs to be uploaded to the ESP8266. Once initially uploaded, it can then be updated via OTA. With platform.io, this can be done by running the "General -> Upload" task.

The web application needs to be uploaded by building it into the data folder, and then uploading that folder to SPIFFS. With platform.io, this can be done by running the "Platform -> Upload Filesystem Image" task.