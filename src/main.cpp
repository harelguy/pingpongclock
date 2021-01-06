#include <Arduino.h>
#include <FastLED.h>                                         // FastLED library.
#include <NTPClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoOTA.h>
// #include <ESPAsyncTCP.h>
#include <WiFiUdp.h>
// #include <ESPAsyncWebServer.h>
// #include <SPIFFSEditor.h>
#include <ClockDigits.h>
#include <Hash.h>

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

// Fastled Definitions
#define COLOR_ORDER BGR                                       // It's GRB for WS2812B and BGR for APA102
#define LED_TYPE APA102                                       // What kind of strip are you using (APA102, WS2801 or WS2812B)?
#define NUM_LEDS 128                                          // Number of LED's

// Wireless config
const char* ssid = "dafniharel";
const char* password = "mitzmitz3103";
const long utcOffsetInSeconds = 10800; // UTC + 3 hours

uint8_t max_bright = 80;                                     // Overall brightness definition. It can be changed on the fly.

struct CRGB leds[NUM_LEDS];                                   // Initialize our LED array.

int connectionState = 0;  // 0 = Connecting, 1 = Connected

#define MODE_CLOCK 0
#define MODE_PLASMA 1
#define MODE_FILLER 2
#define MODE_INTERACTIVE 3

// Make sure to update MODE_MIN and MODE_MAX so that mode cycling works
#define MODE_MIN MODE_CLOCK
#define MODE_MAX MODE_INTERACTIVE

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

	SPIFFS.begin();		// Start the SPI Flash Files System
	Serial.print("Started SPIFFS");

	// List spiffs content to serial
	Dir root = SPIFFS.openDir("/");
	while (root.next()) {
		Serial.println(root.fileName());
	}

} // setup()


WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void initTimeClient() {
	timeClient.begin();
	timeClient.update();
}

void initWebServer();
void handleWebConnections();

void otaInit() {
	ArduinoOTA.onStart([]() {
		String type;
		if (ArduinoOTA.getCommand() == U_FLASH)
		type = "sketch";
		else // U_SPIFFS
		type = "filesystem";

		// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		Serial.println("Start updating " + type);
	});
	ArduinoOTA.onEnd([]() {
		Serial.println("\nEnd");
	});
	ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
		Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
	});
	ArduinoOTA.onError([](ota_error_t error) {
		Serial.printf("Error[%u]: ", error);
		if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
		else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
		else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
		else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
		else if (error == OTA_END_ERROR) Serial.println("End Failed");
	});
	Serial.println("Calling ArduinoOTA.begin()");
	ArduinoOTA.begin();
}
void otaError(ota_error_t err) {
	Serial.println("Arduino OTA error");
	Serial.println(err);
}
void connectToInternet() {
	if (connectionState == 0) {
		if (WiFi.status() == WL_CONNECTED) {
			Serial.print("Connected!");
			Serial.print(WiFi.localIP());
			initTimeClient();
			initWebServer();
			connectionState = 1;
			otaInit();
		}
	}
	if (connectionState == 1) {
		handleWebConnections();
		ArduinoOTA.handle();
	}
}

void changeStateOnButtonPress();
void cycleDisplayMode();
void displayClock();
void displayPlasma();
void displayFiller();
void displayInteractive();

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
	if (displayMode == MODE_INTERACTIVE) {
		displayInteractive();
	}
	FastLED.show();

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

void setDisplayMode(int mode) {
	displayMode = mode;
	Serial.write("Display mode:");
	Serial.write(displayMode);
}

void rainbow_beat();

//function that uses the Digit aray to display numbers between 0 and 100
void displaynumber(int place, int number) {
	for (int i = 0; i < 10; i++) {
		if (Digits[number / 10][i] != 0) {
			leds[(Digits[number / 10][i] + place)] = CRGB(255, 255, 255);;
		}
		if (Digits[number % 10][i] != 0) {
			leds[(Digits[number % 10][i] + 28 + place)] = CRGB(255, 255, 255);;
		}
	}
}

void displayClock() {

	rainbow_beat();
	fadeLightBy(leds, NUM_LEDS, 240);
	FastLED.show();

	if (connectionState == 1) {
		// fadeLightBy(leds, NUM_LEDS, 00);
		displaynumber(0, timeClient.getHours() /* tm.Hour */);
		displaynumber(70, timeClient.getMinutes() /* tm.Minute */);
		if (timeClient.getSeconds() /* tm.Second */ % 2 == 0) {
			leds[64] = CRGB(255, 255, 255);
			leds[66] = CRGB(255, 255, 255);
		}
	}
}


// Effects


// General effects

void rainbow_beat() {
	uint8_t beatA = beatsin8(17, 0, 255);                        // Starting hue
	uint8_t beatB = beatsin8(13, 0, 255);
	fill_rainbow(leds, NUM_LEDS, (beatA + beatB) / 2, 8);            // Use FastLED's fill_rainbow routine.
}

// For plasma fastled
// https://github.com/atuline/FastLED-Demos/blob/master/plasma/plasma.ino
#define qsubd(x, b)  ((x>b)?b:0)                              // Digital unsigned subtraction macro. if result <0, then => 0. Otherwise, take on fixed value.
#define qsuba(x, b)  ((x>b)?x-b:0)                            // Analog Unsigned subtraction macro. if result <0, then => 0

CRGBPalette16 currentPalette;                                 // Palette definitions
CRGBPalette16 targetPalette;
TBlendType currentBlending = LINEARBLEND;
bool plasmaInit = false;
void plasma();

void displayPlasma() {
	if (!plasmaInit) {
		currentPalette = OceanColors_p;
		plasmaInit = true;
	}

	EVERY_N_MILLISECONDS(50) {                                  // FastLED based non-blocking delay to update/display the sequence.
		plasma();
	}

	EVERY_N_MILLISECONDS(1000) {
		Serial.println(LEDS.getFPS());                            // Optional check of our fps.
	}

	EVERY_N_MILLISECONDS(100) {
		uint8_t maxChanges = 24;
		nblendPaletteTowardPalette(currentPalette, targetPalette, maxChanges);   // AWESOME palette blending capability.
	}

	EVERY_N_SECONDS(5) {                                 // Change the target palette to a random one every 5 seconds.
		uint8_t baseC = random8();                         // You can use this as a baseline colour if you want similar hues in the next line.
		targetPalette = CRGBPalette16(CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)), CHSV(baseC + random8(32), 192, random8(128, 255)), CHSV(baseC + random8(32), 255, random8(128, 255)));
	}
}

void plasma() {                                                 // This is the heart of this program. Sure is short. . . and fast.
	int thisPhase = beatsin8(6, -64, 64);                           // Setting phase change for a couple of waves.
	int thatPhase = beatsin8(7, -64, 64);

	for (int k = 0; k < NUM_LEDS; k++) {                              // For each of the LED's in the strand, set a brightness based on a wave as follows:

		int colorIndex = cubicwave8((k * 23) + thisPhase) / 2 + cos8((k * 15) + thatPhase) / 2;           // Create a wave and add a phase change and add another wave with its own phase change.. Hey, you can even change the frequencies if you wish.
		int thisBright = qsuba(colorIndex, beatsin8(7, 0, 96));                                 // qsub gives it a bit of 'black' dead space by setting sets a minimum value. If colorIndex < current value of beatsin8(), then bright = 0. Otherwise, bright = colorIndex..

		leds[k] = ColorFromPalette(currentPalette, colorIndex, thisBright, currentBlending);  // Let's now add the foreground colour.
	}

} // plasma()


int fillerCurrentLed = 0;
CRGB currentFillerColor = CRGB(255, 255, 255);
CRGB lastFillerColor = CRGB(0, 0, 0);

void displayFiller() {
	// Clear out the leds
	for (int x = 0; x < NUM_LEDS; x++) {
		leds[x] = lastFillerColor;
	}
	for (int x = 0; x <= fillerCurrentLed; x++) {
		leds[x] = currentFillerColor;
	}
	EVERY_N_MILLISECONDS(25) {                                  // FastLED based non-blocking delay to update/display the sequence.
		fillerCurrentLed++;
		if (fillerCurrentLed == NUM_LEDS) {
			fillerCurrentLed = 0;
			// Change Color
			lastFillerColor = currentFillerColor;
			currentFillerColor = CRGB(random(0, 255), random(0, 255), random(0, 255));
		}
	}
}

#include <ESP8266mDNS.h>        // Include the mDNS library
#include <ESP8266WebServer.h>   // Include the WebServer library
#include <ESP8266HTTPUpdateServer.h>
#include <uri/UriBraces.h>
#include <WebSockets.h>
#include <WebSocketsServer.h>
#define USE_SERIAL Serial

const String APP_VERSION = "0.9.1";
ESP8266HTTPUpdateServer httpUpdater;
const char* MDNS_NAME = "pingpongclock";
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80
WebSocketsServer webSocket = WebSocketsServer(3000); // Create a websocket server for updating the leds in real-time
struct CRGB leds_interactive[NUM_LEDS];

void handleWebConnections() {

	MDNS.update();
	server.handleClient();
	webSocket.loop();
}

// Web server methods

void handleCycleMode() {                          // If a POST request is made to URI /cycle
	cycleDisplayMode();
	server.sendHeader("Location", "/");        // Add a header to respond with a new location for the browser to go to the home page again
	server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void drawClear() {
	for (int i = 0; i < NUM_LEDS; i++)
		leds_interactive[i] = CRGB(0, 0, 0);
	displayInteractive();
}

void handleDrawClear() {
	drawClear();
	server.sendHeader("Location", "/draw.html");
	server.send(303);
}

void handleSwitch() {
	String mode = server.pathArg(0);
	setDisplayMode(mode.toInt());
	if (mode.toInt() == MODE_INTERACTIVE) {
		drawClear();
		server.sendHeader("Location", "/draw.html");
	}
	else 
		server.sendHeader("Location", "/");
	server.send(303);
}

void handleVersion() {
	String response = String("<html><h1>Version: " + APP_VERSION + "</h1></html>");
	server.send(200, "text/html", response);
}

String getContentType(String filename) { // convert the file extension to the MIME type
	if (filename.endsWith(".html") || filename.endsWith(".htm")) return "text/html";
	else if (filename.endsWith(".css")) return "text/css";
	else if (filename.endsWith(".js")) return "application/javascript";
	else if (filename.endsWith(".ico")) return "image/x-icon";
	else if (filename.endsWith(".gz")) return "application/x-gzip";
	return "text/plain";
}

bool handleFileRead(String path) { // send the right file to the client (if it exists)
	Serial.println("handleFileRead: " + path);
	if (path.endsWith("/")) path += "index.html";           // If a folder is requested, send the index file
	String contentType = getContentType(path);             // Get the MIME type
	String pathWithGz = path + ".gz";
	
	if (SPIFFS.exists(pathWithGz) || SPIFFS.exists(path)) {  // If the file exists, either as a compressed archive, or normal
		if (SPIFFS.exists(pathWithGz))                          // If there's a compressed version available
			path += ".gz";                                         // Use the compressed version
		File file = SPIFFS.open(path, "r");                    // Open the file
		size_t sent = server.streamFile(file, contentType);    // Send it to the client
		file.close();                                          // Close the file again
		Serial.println(String("\tSent file: ") + path);
		return true;
	}
	Serial.println(String("\tFile Not Found: ") + path);
	return false;                                          // If the file doesn't exist, return false
}

void handleNotFound() {
	if (!handleFileRead(server.uri()))
		server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}


void webSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {

	switch (type) {
		case WStype_DISCONNECTED:
			USE_SERIAL.printf("[%u] Disconnected!\n", num);
			break;
		case WStype_CONNECTED:
			{
				IPAddress ip = webSocket.remoteIP(num);
				USE_SERIAL.printf("[%u] Connected from %d.%d.%d.%d url: %s\n", num, ip[0], ip[1], ip[2], ip[3], payload);

				// send message to client
				webSocket.sendTXT(num, "Connected");
			}
			break;
		case WStype_TEXT:
			USE_SERIAL.printf("[%u] get Text: %s\n", num, payload);
			
			// convert the payload : #<led_number_HEX><rgb_HEX>
			if (payload[0] == '#') {            // we get RGB data
				char buffer[68] = {'\0'};
				String message((char *)payload);
				webSocket.sendTXT(num, "Received an RGB value: " + message);
				long number = strtol((char*)&payload[1], NULL, 16);
				long led_id = number >> 24 & 0xFF;
				// Split them up into r, g, b values
				long r = number >> 16 & 0xFF;
				long g = number >> 8 & 0xFF;
				long b = number & 0xFF;
				
				sprintf(buffer, "Decoded: LED ID = %u, R = %u, G = %u, B = %u", led_id, r, g, b);
				webSocket.sendTXT(num, buffer);
				leds_interactive[led_id] = CRGB(r, g, b);
			} else if (payload[0] == 'C') { // The command for Clear
				drawClear();
				webSocket.sendTXT(num, "Clear");
			}
			// send data to all connected clients
			// webSocket.broadcastTXT("message here");
			break;
		case WStype_BIN:
			USE_SERIAL.printf("[%u] get binary length: %u\n", num, length);
			hexdump(payload, length);

			// send message to client
			// webSocket.sendBIN(num, payload, length);
			break;
	}
}

void displayInteractive() {
	for (int i = 0; i < NUM_LEDS; i++) {
		leds[i] = leds_interactive[i];
	}
	FastLED.show();
}

void initWebServer() {

	if (!MDNS.begin(MDNS_NAME)) {             // Start the mDNS responder for pingpongclock.local
		Serial.println("Error setting up MDNS responder!");
	}
	Serial.println("mDNS responder started");

	// Set up the web server
	
	server.on(UriBraces("/switch/{}"), HTTP_GET, handleSwitch);
	server.on("/cycle", HTTP_POST, handleCycleMode);
	server.on("/version", HTTP_GET, handleVersion);
	server.on("/draw/clear", HTTP_GET, handleDrawClear);

	server.onNotFound(handleNotFound);

	httpUpdater.setup(&server);				// Handle OTA updates. This will create a /update route

	server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
	server.begin();                           // Actually start the server
	Serial.println("HTTP server started");

	webSocket.begin();
	webSocket.onEvent(webSocketEvent);
	Serial.println("Websocket server started");

	for (int i = 0; i < NUM_LEDS; i++) {
		leds_interactive[i] = CRGB(0, 0, 0);	
	}
}