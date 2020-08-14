#include <ESP8266HTTPUpdateServer.h>
ESP8266HTTPUpdateServer httpUpdater;
const char* MDNS_NAME = "pingpongclock";
ESP8266WebServer server(80);    // Create a webserver object that listens for HTTP request on port 80


void initWebServer() {
	// Set up the web server
	server.on("/", handleRoot);               // Call the 'handleRoot' function when a client requests URI "/"
	server.on("/cycle", HTTP_POST, handleCycleMode);
	server.on("/leds", HTTP_POST, handleLedPush);
	httpUpdater.setup(&server);				// Handle OTA updates. This will create a /update route

	server.onNotFound(handleNotFound);        // When a client requests an unknown URI (i.e. something other than "/"), call function "handleNotFound"
	server.begin();                           // Actually start the server
	Serial.println("HTTP server started");
}

void handleWebConnections() {
	server.handleClient();
}
// Web server methods
void handleRoot() {
	String response = "<html><h3>Current mode: ";
	switch (displayMode) {
	case MODE_CLOCK:
		response += "Clock";
		break;
	case MODE_PLASMA:
		response += "Plasma";
		break;
	}
	response += "</h3>";
	response += "<form action=\"/cycle\" method=\"POST\"><input type=\"submit\" value=\"Toggle Mode\"></form>";
	server.send(200, "text/html", response);

	server.send(200, "text/plain", "Hello world!");   // Send HTTP status 200 (Ok) and send some text to the browser/client
}

void handleCycleMode() {                          // If a POST request is made to URI /cycle
	cycleDisplayMode();
	server.sendHeader("Location", "/");        // Add a header to respond with a new location for the browser to go to the home page again
	server.send(303);                         // Send it back to the browser with an HTTP status 303 (See Other) to redirect
}

void handleLedPush() {

}

void handleNotFound() {
	server.send(404, "text/plain", "404: Not found"); // Send HTTP status 404 (Not Found) when there's no handler for the URI in the request
}