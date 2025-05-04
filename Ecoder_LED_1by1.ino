// WS2812B LED Strip with Progressive Lighting Control - Fixed Reversing
// For Arduino Uno with 39 LEDs

#include <FastLED.h>
#include <Encoder.h>

// LED strip configuration
#define LED_PIN     9         // Data pin to LED strip
#define NUM_LEDS    39        // Set for your 39 LED strip
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

// Encoder pins
#define ENCODER_DT  4         // DT pin
#define ENCODER_CLK 5         // CLK pin
#define ENCODER_SW  3         // Push button pin

// Create LED array and encoder
CRGB leds[NUM_LEDS];
Encoder myEncoder(ENCODER_DT, ENCODER_CLK);

// Variables
int brightness = 100;         // Fixed brightness (0-255)
long oldPosition = 0;         // Last encoder position
int activeCount = 0;          // Number of active LEDs
int colorIndex = 0;           // Current color index
int lastDirection = 0;        // Track last direction of movement

// Define some preset colors
CRGB colorChoices[] = {
  CRGB::White,
  CRGB::Red,
  CRGB::Orange,
  CRGB::Yellow,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Purple,
  CRGB::Pink
};
#define NUM_COLORS (sizeof(colorChoices) / sizeof(colorChoices[0]))

void setup() {
  Serial.begin(9600);
  Serial.println("Progressive LED Control with Encoder");
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  
  // Set up encoder button
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  // Initialize all LEDs to off
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();
  
  Serial.println("Controls:");
  Serial.println("- Turn encoder: Adjust number of lit LEDs");
  Serial.println("- Press button: Change color");
}

void loop() {
  // Read encoder with higher sensitivity
  long newPosition = myEncoder.read(); 
  
  if (newPosition != oldPosition) {
    // Determine direction of rotation
    int direction = (newPosition > oldPosition) ? 1 : -1;
    
    // Directly adjust the active count based on direction
    // This avoids mapping issues with encoder reversals
    if (direction == 1) {
      activeCount = min(activeCount + 1, NUM_LEDS);
    } else {
      activeCount = max(activeCount - 1, 0);
    }
    
    // Update LED display
    updateLEDs();
    
    // Print debug info
    Serial.print("Position: ");
    Serial.print(newPosition);
    Serial.print(", Direction: ");
    Serial.print(direction);
    Serial.print(", Active LEDs: ");
    Serial.println(activeCount);
    
    // Save current position
    oldPosition = newPosition;
    lastDirection = direction;
  }
  
  // Check button press to change colors
  if (digitalRead(ENCODER_SW) == LOW) {
    // Wait for button release with debounce
    delay(50);
    while (digitalRead(ENCODER_SW) == LOW) {
      delay(10);
    }
    delay(50);
    
    // Change to next color
    colorIndex = (colorIndex + 1) % NUM_COLORS;
    
    // Update LED display with new color
    updateLEDs();
    
    Serial.print("New color: Index ");
    Serial.println(colorIndex);
  }
  
  // Small delay for stability
  delay(5);
}

// Function to update LEDs based on active count and color
void updateLEDs() {
  // First turn all LEDs off
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  
  // Then turn on only the active number of LEDs
  for (int i = 0; i < activeCount; i++) {
    leds[i] = colorChoices[colorIndex];
  }
  
  // Show the updated LED pattern
  FastLED.show();
}