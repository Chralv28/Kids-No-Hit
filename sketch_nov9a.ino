#include "WiFiS3.h"
#include "arduino_secrets.h"

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
          digitalWrite(LED_BUILTIN, LOW);
        } else if (currentLine.endsWith("GET /L")) {
          digitalWrite(LED_BUILTIN, HIGH);
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