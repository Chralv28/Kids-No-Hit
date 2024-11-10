#include "WiFiS3.h"
#include "arduin_secrets.h"
#include <Servo.h>

Servo myservo;

int pos = 90;   // Global position variable for the servo
int pin = 9;    // Pin for the servo

int count = 0;

// Please enter your sensitive information
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int led = LED_BUILTIN;
int status = WL_IDLE_STATUS;



WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(10000);
  }

  server.begin();
  printWifiStatus();

  myservo.attach(pin);

  // Set the servo to a known starting position (e.g., 90 degrees)
  myservo.write(90);  // Set servo to 90 degrees at startup (prevents jumping to the initial position)
  delay(1000);  // Wait for 1 second to ensure the servo has moved to 90 degrees
}

void moveServoPos() {
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);  // Move the servo to the new position
    Serial.println(pos); // Print the current position to Serial Monitor
    delay(15);           // Add a small delay for smooth movement
  }
}

void moveServoNeg() {
  for (pos = 90; pos >= 0; pos--) {
    myservo.write(pos);  // Move the servo to the new position
    Serial.println(pos); // Print the current position to Serial Monitor
    delay(15);           // Add a small delay for smooth movement
  }
}

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client");
    String currentLine = "";
    bool faviconRequest = false;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            if (faviconRequest) {
              client.stop();
              Serial.println("Favicon request ignored.");
              break;
            }

            // Send HTTP headers
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");  // Ensures the connection is properly closed
            client.println();

            // HTML content
            client.print("<html><body>");
            client.print("<p style=\"font-size:7vw;\">Click <a href=\"/H\">here</a> to turn the LED on<br></p>");
            client.print("<p style=\"font-size:7vw;\">Click <a href=\"/L\">here</a> to turn the LED off<br></p>");
            client.print("</body></html>");
            
            client.println();
            break;
          } else {
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /favicon.ico")) {
          faviconRequest = true;
        } else if (currentLine.endsWith("GET /H")) {
          if (count == 1) {
            moveServoPos();
            count = 0; // Toggle the count value to 1
          }
        } else if (currentLine.endsWith("GET /L")) {
          if (count == 0) {
            moveServoNeg(); 
            count = 1; // Toggle the count value to 0
          } 
        }
      }
    }
    client.stop();
    Serial.println("Client disconnected.");
  }
}

void printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
  Serial.print("To see this page in action, open a browser to http://");
  Serial.println(ip);
}