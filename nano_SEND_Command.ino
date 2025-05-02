#include <WiFi.h>

const char* ssid = "UnoMotorAP";       // Uno R4 AP name
const char* password = "motormotor";   // AP password
const char* serverIP = "192.168.4.1";   // Default IP of Uno R4 WiFi AP
const uint16_t serverPort = 80;

WiFiClient client;

// Button pins
#define BUTTON_PIN1 12
#define BUTTON_PIN2 13

void setup() {
  Serial.begin(9600);
  
  pinMode(BUTTON_PIN1, INPUT_PULLUP);
  pinMode(BUTTON_PIN2, INPUT_PULLUP);

  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("Connecting to Uno R4 WiFi AP...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to Uno AP!");
  digitalWrite(LED_BUILTIN, HIGH); // LED on when connected
}

void loop() {
  if (digitalRead(BUTTON_PIN1) == LOW) {
    sendCommand('F');
    delay(500); // debounce
  }

  if (digitalRead(BUTTON_PIN2) == LOW) {
    sendCommand('B');
    delay(500); // debounce
  }
}

void sendCommand(char cmd) {
  if (!client.connected()) {
    if (client.connect(serverIP, serverPort)) {
      Serial.println("Connected to Server!");
    } else {
      Serial.println("Failed to connect to server.");
      return;
    }
  }
  client.print(cmd);
  Serial.print("Sent: ");
  Serial.println(cmd);
}
