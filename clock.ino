const int Digits[10][10] =
{
  {11, 10, 8, 7, 18, 14, 24, 22}, //{7,8,10,11,14,18,22,24},
  {14,16,18,22,24}, // OK
  {11, 10, 9, 7, 18, 16, 14, 22}, //      {7,8,9,11,14,16,18,24},
  {7,9,11,14,16,18,22,24}, // OK
  {9, 8, 7, 16, 14, 24, 22}, //{9,10,11,16,18,22,24},
  {11, 9, 8, 7, 18, 16, 14, 24}, // {7,9,10,11,14,16,18,22},
  {11, 10, 9, 18, 16, 14, 24}, // {7,8,9,14,15,16,18,22},
  {11, 7, 18, 16, 15, 22}, // {7,11,14,16,17,24},
  {11, 10, 9, 8, 7, 18, 16, 14, 24, 22}, // {7,8,9,10,11,14,16,18,22,24},
  {11, 9, 8, 7, 18, 16, 15, 22}, // {7,9,10,11,14,16,17,24},
};

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", utcOffsetInSeconds);

void initTimeClient() {
	timeClient.begin();
	timeClient.update();
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