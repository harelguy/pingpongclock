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