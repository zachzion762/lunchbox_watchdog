#include <Adafruit_NeoPixel.h>
#include <CapacitiveSensor.h>

// Pin configuration
#define NEOPIXEL_PIN    10  // Pin connected to the NeoPixels
#define TOUCH_PIN       3   // Touch sensor pin (SCL)

// NeoPixel configuration
#define NUMPIXELS       2   // Number of NeoPixels
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

// Capacitive touch sensor setup
CapacitiveSensor touchSensor = CapacitiveSensor(4, TOUCH_PIN); // 4 is an arbitrary unused pin, TOUCH_PIN is the capacitive sensor pin

// Variables for LED control and debounce
bool ledState = false;
unsigned long touchStart = 0;
bool displayMode = false;
unsigned long lastToggleTime = 0;
long lastTouchValue = 0;
int debounceDelay = 50;  // Debounce delay in milliseconds

void setup() {
  pixels.begin(); // Initialize NeoPixel strip
  pixels.clear(); // Set all pixel colors to 'off'
  pixels.show();
}

void loop() {
  long touchValue = touchSensor.capacitiveSensor(30); // Sensing the touch input

  // Debounce the touch input
  if (abs(lastTouchValue - touchValue) > 10) { // Check for significant change
    lastTouchValue = touchValue;  // Update the last touch value
    delay(debounceDelay);  // Wait for the debounce period to expire

    // Check if touch is detected
    if (touchValue > 50) { // Threshold value for touch detection
      if (touchStart == 0) { // Record the time when touch starts
        touchStart = millis();
      } else if (millis() - touchStart > 5000) { // Check if touch has lasted for 5 seconds
        displayMode = !displayMode; // Toggle display mode
        touchStart = 0; // Reset touch start time
      }
    } else {
      if (touchStart != 0) {
        if (millis() - touchStart < 5000) { // Short touch
          ledState = !ledState; // Toggle LEDs on/off
          updateLEDs(ledState ? 255 : 0); // Update LEDs based on ledState
        }
        touchStart = 0; // Reset touch start time
      }
    }
  }

  if (displayMode) {
    if (millis() - lastToggleTime >= (ledState ? 5000 : 300000)) { // 5 seconds on, 5 minutes off
      ledState = !ledState; // Toggle state
      updateLEDs(ledState ? 255 : 0);
      lastToggleTime = millis(); // Update the last toggle time
    }
  }
}

void updateLEDs(int brightness) {
  uint32_t color = pixels.Color(brightness, 0, 0); // Red color
  for (int i = 0; i < NUMPIXELS; i++) {
    pixels.setPixelColor(i, color);
  }
  pixels.show();
}
