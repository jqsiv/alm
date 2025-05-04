// WS2812B LED Strip with Animations
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
int animationMode = 0;        // Current animation mode
unsigned long lastUpdate = 0; // Time of last animation update

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
#define NUM_ANIMATIONS 5      // Total number of animation modes

void setup() {
  Serial.begin(9600);
  Serial.println("LED Strip Animation Control");
  
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
  Serial.println("- Press button: Change animation mode");
}

void loop() {
  // Read encoder with higher sensitivity
  long newPosition = myEncoder.read(); 
  
  if (newPosition != oldPosition) {
    // Determine direction of rotation
    int direction = (newPosition > oldPosition) ? 1 : -1;
    
    // Directly adjust the active count based on direction
    if (direction == 1) {
      activeCount = min(activeCount + 1, NUM_LEDS);
    } else {
      activeCount = max(activeCount - 1, 0);
    }
    
    // Save current position
    oldPosition = newPosition;
    
    // Print debug info
    Serial.print("Position: ");
    Serial.print(newPosition);
    Serial.print(", Active LEDs: ");
    Serial.println(activeCount);
  }
  
  // Check button press to change animation mode
  if (digitalRead(ENCODER_SW) == LOW) {
    // Wait for button release with debounce
    delay(50);
    while (digitalRead(ENCODER_SW) == LOW) {
      delay(10);
    }
    delay(50);
    
    // Change to next animation mode
    animationMode = (animationMode + 1) % NUM_ANIMATIONS;
    
    Serial.print("New animation mode: ");
    Serial.println(animationMode);
  }
  
  // Update animations
  updateAnimation();
  
  // Small delay for stability
  delay(5);
}

// Main animation controller
void updateAnimation() {
  // Only update animation every 30ms for smooth motion
  if (millis() - lastUpdate < 30) return;
  lastUpdate = millis();
  
  // Clear all LEDs first
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  
  // Run the selected animation
  switch (animationMode) {
    case 0:
      // Progressive lighting (original behavior)
      progressiveLighting();
      break;
    case 1:
      // Rainbow wave
      rainbowWave();
      break;
    case 2:
      // Cylon scanner
      cylonScanner();
      break;
    case 3:
      // Twinkle stars
      twinkleStars();
      break;
    case 4:
      // Color wipe
      colorWipe();
      break;
  }
  
  // Show the updated LED pattern
  FastLED.show();
}

// Animation 0: Progressive lighting (original behavior)
void progressiveLighting() {
  for (int i = 0; i < activeCount; i++) {
    leds[i] = colorChoices[colorIndex % NUM_COLORS];
  }
}

// Animation 1: Rainbow wave
void rainbowWave() {
  // Only show LEDs up to activeCount
  for (int i = 0; i < activeCount; i++) {
    // Calculate hue based on position and time
    int hue = (i * 256 / NUM_LEDS + millis() / 50) % 256;
    leds[i] = CHSV(hue, 255, 255);
  }
}

// Animation 2: Cylon scanner
void cylonScanner() {
  // Calculate scanner position (moving dot)
  static int pos = 0;
  static int direction = 1;
  
  // Clear previous position
  fadeToBlackBy(leds, NUM_LEDS, 70);
  
  // Show dot only if it's within active range
  if (pos < activeCount) {
    // Draw the bright center pixel
    leds[pos] = colorChoices[colorIndex % NUM_COLORS];
    
    // Add fading trail
    for (int i = 1; i < 5; i++) {
      if (pos - i >= 0 && pos - i < activeCount) {
        leds[pos - i] = leds[pos - i].fadeToBlackBy(i * 50);
      }
      if (pos + i < activeCount) {
        leds[pos + i] = leds[pos + i].fadeToBlackBy(i * 50);
      }
    }
  }
  
  // Move the dot for next time
  pos += direction;
  
  // Reverse direction at the ends
  if (pos >= activeCount - 1) direction = -1;
  if (pos <= 0) direction = 1;
}

// Animation 3: Twinkle stars
void twinkleStars() {
  // Randomly twinkle LEDs
  fadeToBlackBy(leds, NUM_LEDS, 20);  // Fade existing LEDs
  
  // Add new random twinkles
  if (random8() < 80) {  // Probability of new twinkle
    int pos = random16(activeCount);
    if (pos < activeCount) {
      leds[pos] = CHSV(random8(), 200, 255);
    }
  }
}

// Animation 4: Color wipe
void colorWipe() {
  static int pos = 0;
  static unsigned long lastWipe = 0;
  
  // Only update every 50ms for a slower wipe effect
  if (millis() - lastWipe < 50) return;
  lastWipe = millis();
  
  // Start a new wipe when previous completes
  if (pos >= activeCount) {
    pos = 0;
    colorIndex = (colorIndex + 1) % NUM_COLORS;
  }
  
  // Wipe in the new color
  if (pos < activeCount) {
    leds[pos] = colorChoices[colorIndex];
    pos++;
  }
}