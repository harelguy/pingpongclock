
#include "FastLED.h"                                          // FastLED library.
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WebServer.h>   // Include the WebServer library

#include <WiFiUdp.h>

#if FASTLED_VERSION < 3001000
#error "Requires FastLED 3.1 or later; check github for latest code."
#endif

// For NodeMCU V3, Fastled gets the "D.." pin numbers, not the GPIO numbers
#define LED_DT 1                                            // NodeMCU D01   Serial data pin APA102 - Yellow
#define LED_CK 2                                            // NodeMCU D02   Clock pin APA102 - Green

// Built-in LED, for debugging
#define LED_BUILTIN 2                                       // NodeMCU GPIO02 - internal led

// Connected arcade button. Uses a pullup transistor, so when pressed goes low
#define BTN_PULLUP 0                                          // NodeMCU D03 - GPIO00
#define BTN_ISPRESSED LOW

#define COLOR_ORDER BGR                                       // It's GRB for WS2812B and BGR for APA102
#define LED_TYPE APA102                                       // What kind of strip are you using (APA102, WS2801 or WS2812B)?
#define NUM_LEDS 128                                          // Number of LED's

// Wireless config
const char* ssid = "dafniharel";
const char* password = "mitzmitz3103";
const long utcOffsetInSeconds = 10800; // UTC + 3 hours

// Initialize changeable global variables.
uint8_t max_bright = 80;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.


int connectionState = 0;  // 0 = Connecting, 1 = Connected

#define MODE_CLOCK 0
#define MODE_PLASMA 1
#define MODE_FILLER 2
#define MODE_INTERACTIVE 3  // TBD

// Make sure to update MODE_MIN and MODE_MAX so that mode cycling works
#define MODE_MIN MODE_CLOCK
#define MODE_MAX MODE_FILLER

int displayMode = MODE_CLOCK; // 0 = Clock, 1 = Plasma, 2 = Filler, 3 = Interactive
int buttonPressed = 0;

void setup() {
	pinMode(BTN_PULLUP, INPUT_PULLUP);
	pinMode(LED_BUILTIN, OUTPUT);
	Serial.begin(115200);                                        // Initialize serial port for debugging.
	delay(1000);                                                // Soft startup to ease the flow of electrons.

  //  LEDS.addLeds<LED_TYPE, LED_DT, COLOR_ORDER>(leds, NUM_LEDS);         // For WS2812B
	LEDS.addLeds<LED_TYPE, LED_DT, LED_CK, COLOR_ORDER>(leds, NUM_LEDS);   // For APA102 or WS2801 - Green

	FastLED.setBrightness(max_bright);
	FastLED.setMaxPowerInVoltsAndMilliamps(5, 2000);               // FastLED Power management set at 5V, 500mA

	WiFi.begin(ssid, password);
	Serial.print("Connecting to WiFi");

} // setup()



void loop() {
	connectToInternet();
	changeStateOnButtonPress();

	if (displayMode == MODE_CLOCK) {
		displayClock();
	}
	if (displayMode == MODE_PLASMA) {
		displayPlasma();
	}
	if (displayMode == MODE_FILLER) {
		displayFiller();
	}
	
	FastLED.show();

}


void connectToInternet() {
	if (connectionState == 0) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.print("Connected!");
			initTimeClient();
			if (!MDNS.begin("pingpongclock")) {             // Start the mDNS responder for esp8266.local
				Serial.println("Error setting up MDNS responder!");
			}
			Serial.println("mDNS responder started");
			initWebServer();
			connectionState = 1;
		}
	}
	if (connectionState == 1) {
		MDNS.update();
		handleWebConnections();
	}
}

void changeStateOnButtonPress() {

	if (digitalRead(BTN_PULLUP) == BTN_ISPRESSED) {
		digitalWrite(LED_BUILTIN, LOW); // Turn the internal LED on - the led is active LOW
		// buttonPressed is used to debounce, so the mode change only occurs on the initial press, otherwise would cycle very quickly
		if (buttonPressed == 0) { // Do this part only if this is the first detected press
			buttonPressed = 1;
			cycleDisplayMode();
		}
	}
	else {
		buttonPressed = 0;
		digitalWrite(LED_BUILTIN, HIGH);
	}
}

void cycleDisplayMode() {
	displayMode++;
	if (displayMode > MODE_MAX)
		displayMode = MODE_MIN;
	Serial.write("Display mode:");
	Serial.write(displayMode);
}
