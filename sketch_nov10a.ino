#include "WiFiS3.h"
#include <Servo.h>
#include "arduino_secrets.h"
#include "SPIFFS.h"

Servo myservo;

int pos = 90;  // Global position variable for the servo
int pin = 9;   // Pin for the servo
int count = 0;

// WiFi credentials from arduino_secrets.h
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int led = 5;
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  // Initialize SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("SPIFFS initialization failed!");
    while (1);
  }
  Serial.println("SPIFFS initialized successfully!");

  // Print available files
  Serial.println("Listing files in SPIFFS:");
  File root = SPIFFS.open("/");
  File file = root.openNextFile();
  while(file) {
    Serial.print("  - ");
    Serial.println(file.name());
    file = root.openNextFile();
  }

  // Check WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true);
  }

  // Connect to WiFi
  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  server.begin();
  printWifiStatus();

  // Initialize servo
  myservo.attach(pin);
  myservo.write(90);
  delay(1000);
}

void moveServoPos() {
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);
    Serial.println(pos);
    delay(15);
  }
}

void moveServoNeg() {
  for (pos = 90; pos >= 0; pos--) {
    myservo.write(pos);
    Serial.println(pos);
    delay(15);
  }
}

void serveImage(WiFiClient &client, String filename) {
  if (filename.startsWith("/")) filename = "/" + filename;
  
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file: " + filename);
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
    return;
  }

  String contentType = "image/jpeg";
  if (filename.endsWith(".png")) {
    contentType = "image/png";
  }

  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: " + contentType);
  client.println("Connection: close");
  client.println();

  // Send file in chunks
  const int bufSize = 1024;
  uint8_t buf[bufSize];
  while (file.available()) {
    int n = file.read(buf, bufSize);
    client.write(buf, n);
  }

  file.close();
}

void glamifyAndFunction(WiFiClient &client);

void loop() {
  WiFiClient client = server.available();

  if (client) {
    Serial.println("New Client");
    String currentLine = "";
    String requestedFile = "";

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        Serial.write(c);

        if (c == '\n') {
          if (currentLine.length() == 0) {
            // Check if this is an image request
            if (requestedFile.endsWith(".jpg") || requestedFile.endsWith(".png")) {
              serveImage(client, requestedFile);
            } else {
              // Send regular HTML page
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println("Connection: close");
              client.println();
              
              // HTML content
              glamifyAndFunction(client);
            }
            break;
          } else {
            // Extract requested file from GET request
            if (currentLine.startsWith("GET ")) {
              requestedFile = currentLine.substring(4);
              requestedFile = requestedFile.substring(0, requestedFile.indexOf(' '));
              Serial.println("Requested file: " + requestedFile);
            }
            currentLine = "";
          }
        } else if (c != '\r') {
          currentLine += c;
        }

        if (currentLine.endsWith("GET /H")) {
          if (count == 1) {
            digitalWrite(led, HIGH);
            moveServoPos();
            count = 0;
          }
        } else if (currentLine.endsWith("GET /L")) {
          if (count == 0) {
            digitalWrite(led, LOW);
            moveServoNeg();
            count = 1;
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

void glamifyAndFunction(WiFiClient &client) {
  // HTML content with embedded CSS and JavaScript
  client.println("<!DOCTYPE html><html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width, initial-scale=3.0'>");
  client.println("<title>KNS</title>");
  client.println("<style>");
  // CSS Styles
  client.println("body { font-family: Arial, sans-serif; text-align: center; background-color: #f2f2f2; }");
  client.println("h1 { color: #333; font-size: 2.5em; margin-top: 20px; }");
  client.println(".switch { position: relative; display: inline-block; width: 60px; height: 34px; }");
  client.println(".switch input { opacity: 0; width: 0; height: 0; }");
  client.println(".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; transition: .4s; border-radius: 34px; }");
  client.println(".slider:before { position: absolute; content: ''; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; transition: .4s; border-radius: 50%; }");
  client.println("input:checked + .slider { background-color: #4CAF50; }");
  client.println("input:checked + .slider:before { transform: translateX(26px); }");
  client.println("#status { font-size: 1.2em; margin-top: 10px; color: #555; }");
  client.println("#busImage, #stopsignImage { max-width: 100%; height: auto; margin-top: 20px; }");
  client.println("#stopsignImage { display: none; }");
  client.println("#busImage { display: block; margin: 10px auto; max-width: 400px; }");
  client.println("</style>");
  client.println("</head>");
  
  client.println("<body>");
  client.println("<h1>Kids No Hit!</h1>");
  client.println("<label class='switch'>");
  client.println("<input type='checkbox' id='toggleSwitch'>");
  client.println("<span class='slider'></span>");
  client.println("</label>");
  client.println("<p>Status: <span id='status'>Bus is not here!</span></p>");
  client.println("<img id='busImage' src='/bus.jpg' alt='Bus Image'>");
  client.println("<img id='stopsignImage' src='/stopsign.png' alt='Stop Sign Image'>");
  
  client.println("<script>");
  client.println("document.getElementById('toggleSwitch').addEventListener('change', function() {");
  client.println("  let statusText = document.getElementById('status');");
  client.println("  let busImage = document.getElementById('busImage');");
  client.println("  let stopsignImage = document.getElementById('stopsignImage');");
  client.println("  if (this.checked) {");
  client.println("    statusText.innerText = 'Bus is here!';");
  client.println("    busImage.style.display = 'none';");
  client.println("    stopsignImage.style.display = 'block';");
  client.println("    fetch('/H');");
  client.println("  } else {");
  client.println("    statusText.innerText = 'Bus is not here!';");
  client.println("    busImage.style.display = 'block';");
  client.println("    stopsignImage.style.display = 'none';");
  client.println("    fetch('/L');");
  client.println("  }");
  client.println("});");
  client.println("</script>");
  
  client.println("</body></html>");
}