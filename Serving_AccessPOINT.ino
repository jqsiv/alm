#include <WiFiS3.h>
#include <WiFiServer.h>
#include <Stepper.h>

// Motor
#define STEPS 32
Stepper small_stepper(STEPS, 8, 10, 9, 11);

WiFiServer server(80);

// Access Point credentials
char ssid[] = "UnoMotorAP";    
char pass[] = "motormotor";    

// Track last command and source
String lastCommand = "None";
String lastSource = "None";

void setup() {
  Serial.begin(9600);

  small_stepper.setSpeed(500);

  Serial.println("Starting Access Point...");
  if (WiFi.beginAP(ssid, pass)) {
    Serial.println("AP Created Successfully!");
    Serial.println(WiFi.localIP()); // Should print 192.168.4.1
  } else {
    Serial.println("Failed to create AP");
    while (1); // stop
  }
  
  server.begin();
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  WiFiClient client = server.available();
  
  if (client) {
    Serial.println("Client Connected");
    digitalWrite(LED_BUILTIN, HIGH);

    String currentLine = "";
    String requestedPath = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        currentLine += c;

        if (c == '\n') {
          if (currentLine.length() == 1) {
            // Done reading request headers — now respond
            if (requestedPath.length() > 0) {
              handleWebRequest(requestedPath);
            }
            sendWebPage(client);
            break;
          }

          // Capture requested path from HTTP GET
          if (currentLine.startsWith("GET ")) {
            int pathStart = currentLine.indexOf(' ') + 1;
            int pathEnd = currentLine.indexOf(' ', pathStart);
            requestedPath = currentLine.substring(pathStart, pathEnd);
          }

          currentLine = "";
        }
      }
    }
    client.stop();
    Serial.println("Client Disconnected");
    digitalWrite(LED_BUILTIN, LOW);
  }
}

// Handle motor moves based on URL path
void handleWebRequest(String path) {
  path.trim();
  
  if (path == "/forward") {
    small_stepper.step(-2048);
    lastCommand = "Forward";
    lastSource = "Browser";
  } 
  else if (path == "/backward") {
    small_stepper.step(2048);
    lastCommand = "Backward";
    lastSource = "Browser";
  }

  // Always release motor coils after move
  digitalWrite(8, LOW);
  digitalWrite(9, LOW);
  digitalWrite(10, LOW);
  digitalWrite(11, LOW);
}

// Serve the HTML page
void sendWebPage(WiFiClient client) {
  client.println("HTTP/1.1 200 OK");
  client.println("Content-type:text/html");
  client.println();
  client.println("<!DOCTYPE html><html><head><meta charset='utf-8'>");
  client.println("<title>Motor Controller</title>");
  client.println("<meta http-equiv='refresh' content='2'>"); // Auto-refresh every 2 sec
  client.println("</head><body style='font-family:sans-serif;'>");
  client.println("<h1>Motor Command Monitor</h1>");
  client.println("<p><strong>Last Command:</strong> " + lastCommand + "</p>");
  client.println("<p><strong>Source:</strong> " + lastSource + "</p>");
  client.println("<button onclick=\"location.href='/forward'\" style='font-size:20px;padding:10px;margin:5px;'>Forward</button>");
  client.println("<button onclick=\"location.href='/backward'\" style='font-size:20px;padding:10px;margin:5px;'>Backward</button>");
  client.println("</body></html>");
  client.println();
}
