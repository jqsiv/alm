// WS2812B LED Strip Control with Rotary Encoder
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
int brightness = 50;          // Starting brightness (0-255)
long oldPosition = 0;         // Last encoder position
int colorIndex = 0;           // Current color index

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
  Serial.println("LED Strip Control with Encoder");
  
  // Initialize FastLED
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS)
    .setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(brightness);
  
  // Set up encoder button
  pinMode(ENCODER_SW, INPUT_PULLUP);
  
  // Set initial color (all LEDs to first color)
  fill_solid(leds, NUM_LEDS, colorChoices[colorIndex]);
  FastLED.show();
  
  Serial.println("Controls:");
  Serial.println("- Turn encoder: Adjust brightness");
  Serial.println("- Press button: Change color");
  Serial.print("Initial brightness: ");
  Serial.println(brightness);
}

void loop() {
  // Read encoder for brightness control
  long newPosition = myEncoder.read() / 3;  // Divide for smoother control
  
  if (newPosition != oldPosition) {
    // Change brightness based on encoder direction
    if (newPosition > oldPosition) {
      brightness = min(brightness + 5, 255);
    } else {
      brightness = max(brightness - 5, 0);
    }
    
    // Update LED brightness
    FastLED.setBrightness(brightness);
    FastLED.show();
    
    // Print debug info
    Serial.print("Brightness: ");
    Serial.println(brightness);
    
    oldPosition = newPosition;
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
    
    // Update all LEDs to the new color
    fill_solid(leds, NUM_LEDS, colorChoices[colorIndex]);
    FastLED.show();
    
    Serial.print("New color: Index ");
    Serial.println(colorIndex);
  }
  
  // Small delay for stability
  delay(10);
}