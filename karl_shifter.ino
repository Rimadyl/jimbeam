#include <FastLED.h>

#define NUM_LEDS 92
#define LED_DATA_PIN 2

CRGB shifterLeds[NUM_LEDS];

//	Inputs
const int selSolidColor = 3;
const int selChill = 4;
const int selBoolMode = 5;
const int illuminationSig = 6;
const int ignitionSignalInput = 7;
const int huePot = A0;

//	Variables
int masterBrightness;
int hueValue;
// const int chillRangeStart = 128;
// const int chillRangeStop = 224;
// int chillHueValue = chillRangeStart;

// Chill Mode Gradient
DEFINE_GRADIENT_PALETTE (chill_grad1) {
	  0,   0, 255, 255, // Cyan
	 64,   0,   0, 255, // Blue
	128, 127,   0, 255, // Purple
	192, 255,   0, 255, // Magenta
	255,   0, 255, 255  // Cyan
};
CRGBPalette16 chillPallet1 = chill_grad1;
uint8_t paletteIndex = 0;

//  RPM Reader Variables
long highPulse;
long lowPulse;
long cycleTime;
float cycleFreq;
int cylinderFreq;
int EJ20RPM;

//  Bool Mode Variables
int ejBoolIdleRPM = 450;   // Pink
int ejBoolStartRPM = 3500; // Yellow
int ejBoolMidRPM = 4500;   // Red
int ejBoolHighRPM = 5500;  // Red Flashing \/
int ejBoolMaxRPM = 6500;

////MAPS the leds to a specific integer recieved from "EJ20RPM"
// LED Select Idle to Start Range
int boolLEDIdleToStartRangeLeft;
int boolLEDIdleToStartRangeRight;

// LED Select Start to Mid Range
int boolLEDStartToMidRangeLeft;
int boolLEDStartToMidRangeRight;

// LED Select Mid to High Range
int boolLEDMidToHighRangeLeft;
int boolLEDMidToHighRangeRight;


void setup() {
	FastLED.addLeds<WS2812B, LED_DATA_PIN, GRB>(shifterLeds, NUM_LEDS);
	pinMode(selSolidColor, INPUT);
	pinMode(selChill, INPUT);
	pinMode(selBoolMode, INPUT);
	pinMode(illuminationSig, INPUT);
	pinMode(huePot, INPUT);
	pinMode(ignitionSignalInput, INPUT);
	Serial.begin(9600);
}

void loop() {
	Serial.print("Tail Light Circuit: ");
	solidColor();
	chillMode();
	boolMode();
}

void brightnessControl() {
	if (digitalRead(illuminationSig) == HIGH) {
		masterBrightness = 64;
		Serial.print("On");
	} else {
		masterBrightness = 255;
		Serial.print("Off");
	}
	FastLED.setBrightness(masterBrightness);
}

void boolMode() {
	while (digitalRead(selBoolMode) == HIGH) {

    brightnessControl();

		serialDebug("Bool Mode", EJ20RPM);
		highPulse = pulseIn(ignitionSignalInput, 1000);
		lowPulse = pulseIn(ignitionSignalInput, LOW);
		cycleTime = highPulse + lowPulse;
		cycleFreq = 1000000 / cycleTime;
		cylinderFreq = cycleFreq * 60;
		EJ20RPM = cylinderFreq * 4;

		if (EJ20RPM == 0) {
			for (int i = 0; i < NUM_LEDS; ++i) {
				shifterLeds[i] = CHSV(0, 255, 0);
			}
			FastLED.show();
		} else {

			// Idle to Start RPM Range
			if (EJ20RPM >= ejBoolIdleRPM && EJ20RPM < ejBoolStartRPM) {

        boolLEDIdleToStartRangeLeft = map(EJ20RPM, ejBoolIdleRPM, ejBoolStartRPM, 0, NUM_LEDS/2);
        boolLEDIdleToStartRangeRight = map(EJ20RPM, ejBoolIdleRPM, ejBoolStartRPM, NUM_LEDS-1, NUM_LEDS/2+1);

				for (int i = 0; i < boolLEDIdleToStartRangeLeft; ++i) {
					shifterLeds[i] = CHSV(224, 255, 255);
				}
				for (int i = boolLEDIdleToStartRangeRight; i > NUM_LEDS/2 ; --i) {
					shifterLeds[i] = CHSV(224, 255, 255);
				}
			}
			// Start to Mid RPM Range
			else if (EJ20RPM >= ejBoolStartRPM && EJ20RPM < ejBoolMidRPM) {

        boolLEDStartToMidRangeLeft = map(EJ20RPM, ejBoolStartRPM, ejBoolMidRPM, 0, NUM_LEDS/2);
        boolLEDStartToMidRangeRight = map(EJ20RPM, ejBoolStartRPM, ejBoolMidRPM, NUM_LEDS-1, NUM_LEDS/2+1);

				for (int i = 0; i < boolLEDStartToMidRangeLeft; ++i) {
					shifterLeds[i] = CHSV(64, 255, 255);
				}
				for (int i = boolLEDStartToMidRangeRight; i > NUM_LEDS/2 ; --i) {
					shifterLeds[i] = CHSV(64, 255, 255);
				}
			}
			// Mid to High RPM Range
			else if (EJ20RPM >= ejBoolMidRPM && EJ20RPM < ejBoolHighRPM) {

        boolLEDMidToHighRangeLeft = map(EJ20RPM, ejBoolMidRPM, ejBoolHighRPM, 0, NUM_LEDS/2);
        boolLEDMidToHighRangeRight = map(EJ20RPM, ejBoolMidRPM, ejBoolHighRPM, NUM_LEDS-1, NUM_LEDS/2+1);

				for (int i = 0; i < boolLEDMidToHighRangeLeft; ++i) {
					shifterLeds[i] = CHSV(0, 255, 255);
				}
				for (int i = boolLEDMidToHighRangeRight; i > NUM_LEDS/2 ; --i) {
					shifterLeds[i] = CHSV(0, 255, 255);
				}
			}
			// High to Max RPM Range
			else if (EJ20RPM >= ejBoolHighRPM && EJ20RPM < ejBoolMaxRPM) {
				for (int i = 0; i < NUM_LEDS; ++i) {
					shifterLeds[i] = CHSV(0, 255, 255);
				}
				EVERY_N_MILLISECONDS(100) {
					FastLED.show();
				}
				for (int i = 0; i < NUM_LEDS; ++i) {
					shifterLeds[i] = CHSV(0, 255, 0);
				}
				EVERY_N_MILLISECONDS(100) {
					FastLED.show();
				}
			}
			FastLED.show();
		}
	}
}

void solidColor() {
	while (digitalRead(selSolidColor) == HIGH) {

    brightnessControl();

		serialDebug("Solid Color", hueValue);
		hueValue = map(analogRead(huePot), 0, 1020, 0, 255);
		for (int i = 0; i < NUM_LEDS; i++) {
			shifterLeds[i] = CHSV(hueValue, 255, 255);
		}
	FastLED.show();
	}
}

void chillMode() {
	while (digitalRead(selChill) == HIGH) {

    brightnessControl();

		serialDebug("Chill Mode", paletteIndex);
		fill_palette(shifterLeds, NUM_LEDS, paletteIndex, 255 / NUM_LEDS, chillPallet1, 255, LINEARBLEND);
		EVERY_N_MILLISECONDS(20) {
			paletteIndex = paletteIndex + 3;
		}
		FastLED.show();
	}
}

void serialDebug(char currentMode[], int debugValue) {
  
  brightnessControl();

	Serial.print("Tail Light Circuit: ");
	Serial.print(digitalRead(illuminationSig));
	Serial.print(" Selected mode: ");
	Serial.print(currentMode);
	if (currentMode == "Bool Mode") {
		Serial.print(" RPM: ");
		Serial.println(debugValue);
	} 
	if (currentMode == "Solid Color") {
		Serial.print(" Hue Value: ");
		Serial.println(debugValue);
	}
	if (currentMode == "Chill Mode") {
		Serial.print(" Palette Index: ");
		Serial.println(debugValue);
	}

}
