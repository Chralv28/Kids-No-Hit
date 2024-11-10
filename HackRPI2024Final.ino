#include "WiFiS3.h"
#include <Servo.h>
#include "arduino_secrets.h"
Servo myservo;

int pos = 90;  // Global position variable for the servo
int pin = 9;   // Pin for the servo

int count = 0;

// Please enter your sensitive information
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

int led = 5;
int status = WL_IDLE_STATUS;

WiFiServer server(80);

void setup() {
  Serial.begin(9600);
  pinMode(led, OUTPUT);

  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("Communication with WiFi module failed!");
    while (true)
      ;
  }

  while (status != WL_CONNECTED) {
    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    status = WiFi.begin(ssid, pass);
    delay(2000);
  }

  server.begin();
  printWifiStatus();

  myservo.attach(pin);

  // Set the servo to a known starting position (e.g., 90 degrees)
  myservo.write(90);  // Set servo to 90 degrees at startup (prevents jumping to the initial position)
  delay(1000);        // Wait for 1 second to ensure the servo has moved to 90 degrees
}

void moveServoPos() {
  for (pos = 0; pos <= 90; pos++) {
    myservo.write(pos);   // Move the servo to the new position
    Serial.println(pos);  // Print the current position to Serial Monitor
    delay(15);            // Add a small delay for smooth movement
  }
}

void moveServoNeg() {
  for (pos = 90; pos >= 0; pos--) {
    myservo.write(pos);   // Move the servo to the new position
    Serial.println(pos);  // Print the current position to Serial Monitor
    delay(15);            // Add a small delay for smooth movement
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
            glamifyAndFunction(client);

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
  client.println("<title>KNS </title>");
  client.println("<style>");
  client.println("body {");
  client.println("    background-color: #79deec;");
  client.println("    margin: 0;");
  client.println("    padding: 0;");
  client.println("    overflow: hidden;");
  client.println("    display: flex;");
  client.println("    justify-content: center;");
  client.println("    align-items: center;");
  client.println("    height: 100vh;");
  client.println("    flex-direction: column;");
  client.println("}");
  client.println(".switch-container { margin-bottom: 30px; }");
  client.println(".switch {");
  client.println("    width: 80px;");
  client.println("    height: 40px;");
  client.println("    background-color: #ccc;");
  client.println("    border-radius: 20px;");
  client.println("    position: relative;");
  client.println("    cursor: pointer;");
  client.println("    box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1);");
  client.println("    transition: background-color 0.3s;");
  client.println("}");
  client.println(".switch input { opacity: 0; position: absolute; z-index: -1; }");
  client.println(".switch .slider {");
  client.println("    position: absolute;");
  client.println("    cursor: pointer;");
  client.println("    top: 0;");
  client.println("    left: 0;");
  client.println("    right: 0;");
  client.println("    bottom: 0;");
  client.println("    background-color: #ccc;");
  client.println("    transition: 0.4s;");
  client.println("    border-radius: 20px;");
  client.println("}");
  client.println(".switch input:checked + .slider { background-color: #ff0000; }");
  client.println(".switch .slider:before {");
  client.println("    content: '';");
  client.println("    position: absolute;");
  client.println("    height: 32px;");
  client.println("    width: 32px;");
  client.println("    border-radius: 50%;");
  client.println("    left: 4px;");
  client.println("    bottom: 4px;");
  client.println("    background-color: white;");
  client.println("    transition: 0.4s;");
  client.println("}");
  client.println(".switch input:checked + .slider:before { transform: translateX(40px); }");
  client.println("#busImage {");
  client.println("    width: 500px;");
  client.println("    height: auto;");
  client.println("    position: fixed;");
  client.println("    top: 50%;");
  client.println("    left: 0;");
  client.println("    transform: translate(25%, -25%);");
  client.println("    opacity: 0;");
  client.println("    transition: opacity 2s ease-out;");
  client.println("}");
  client.println("#busImage.show { opacity: 1; }");
  client.println("#status {");
  client.println("    font-size: 1.2em;");
  client.println("    margin-top: 20px;");
  client.println("    font-weight: bold;");
  client.println("}");
  client.println("</style>");
  client.println("</head>");
  client.println("<body>");
  client.println("<h1>Kids No Hit!</h1>");
  client.println("<label class='switch'>");
  client.println("    <input type='checkbox' id='toggleSwitch'>");
  client.println("    <span class='slider'></span>");
  client.println("</label>");
  client.println("<p>Status: <span id='status'>Bus is not here!</span></p>");
  client.println("<img id='busImage' src='bus.jpg' alt='Bus Image'>");
  client.println("<script>");
  client.println("const toggleSwitch = document.getElementById('toggleSwitch');");
  client.println("const statusDisplay = document.getElementById('status');");
  client.println("const busImage = document.getElementById('busImage');");
  client.println("function updateStateFromHash() {");
  client.println("    const isHerePage = window.location.hash === '#/here';");
  client.println("    toggleSwitch.checked = isHerePage;");
  client.println("    updateUI(isHerePage);");
  client.println("}");
  client.println("function updateUI(isHere) {");
  client.println("    if (isHere) {");
  client.println("        statusDisplay.textContent = 'Bus is here!';");
  client.println("        busImage.classList.add('show');");
  client.println("        fetch('/H');");  // Added to communicate with Arduino
  client.println("    } else {");
  client.println("        statusDisplay.textContent = 'Bus is not here!';");
  client.println("        busImage.classList.remove('show');");
  client.println("        fetch('/L');");  // Added to communicate with Arduino
  client.println("    }");
  client.println("}");
  client.println("function handleToggleChange() {");
  client.println("    const newHash = toggleSwitch.checked ? '#/here' : '#/main';");
  client.println("    window.location.hash = newHash;");
  client.println("    updateUI(toggleSwitch.checked);");
  client.println("}");
  client.println("toggleSwitch.addEventListener('change', handleToggleChange);");
  client.println("window.addEventListener('hashchange', updateStateFromHash);");
  client.println("window.addEventListener('load', function() {");
  client.println("    if (!window.location.hash) {");
  client.println("        window.location.hash = '#/main';");
  client.println("    }");
  client.println("    updateStateFromHash();");
  client.println("});");
  client.println("</script>");
  client.println("</body>");
  client.println("</html>");
}