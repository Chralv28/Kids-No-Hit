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

int led = LED_BUILTIN;
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
            moveServoPos();
            count = 0;
            digitalWrite(LED_BUILTIN, LOW);
          }
        } else if (currentLine.endsWith("GET /L")) {
          if (count == 0) {
            moveServoNeg();
            count = 1;
            digitalWrite(LED_BUILTIN, HIGH);
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
  client.println("<!DOCTYPE html>");
  client.println("<html lang=\"en\">");
  client.println("<head>");
  client.println("    <meta charset=\"UTF-8\">");
  client.println("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
  client.println("    <title>KidsNoHit Complete Interface</title>");
  client.println("    <style>");
  client.println("        * { margin: 0; padding: 0; box-sizing: border-box; }");
  client.println("        body { display: flex; flex-direction: column; align-items: center;");
  client.println("                background-color: #79deec; font-family: Arial, sans-serif;");
  client.println("                min-height: 100vh; overflow-x: hidden; }");
  client.println("        header { padding: 20px; text-align: center; width: 100%; z-index: 20; }");
  client.println("        .logo { font-size: 32px; font-weight: bold; color: #000; margin-bottom: 10px; }");
  client.println("        .control-panel { position: fixed; top: 20px; right: 20px; background: white;");
  client.println("                          padding: 20px; border-radius: 10px; box-shadow: 0 2px 10px rgba(0,0,0,0.1); z-index: 100; }");
  client.println("        .switch { width: 80px; height: 40px; background-color: #ccc; border-radius: 20px;");
  client.println("                  position: relative; cursor: pointer; box-shadow: 0 4px 8px rgba(0, 0, 0, 0.1); transition: background-color 0.3s; }");
  client.println("        .switch input { opacity: 0; position: absolute; z-index: -1; }");
  client.println("        .switch .slider { position: absolute; cursor: pointer; top: 0; left: 0;");
  client.println("                          right: 0; bottom: 0; background-color: #ccc; transition: 0.4s; border-radius: 20px; }");
  client.println("        .switch input:checked + .slider { background-color: #ff0000; }");
  client.println("        .switch .slider:before { content: ''; position: absolute; height: 32px; width: 32px;");
  client.println("                                border-radius: 50%; left: 4px; bottom: 4px; background-color: white;");
  client.println("                                transition: 0.4s; }");
  client.println("        .switch input:checked + .slider:before { transform: translateX(40px); }");
  client.println("        #status { margin-top: 10px; font-weight: bold; text-align: center; }");
  client.println("        main { position: relative; width: 100%; max-width: 1200px; height: 80vh; margin-top: 60px; }");
  // (Add the rest of the CSS here following this pattern)
  client.println("    </style>");
  client.println("</head>");
  client.println("<body>");
  client.println("    <header>");
  client.println("        <div class=\"logo\">KNH</div>");
  client.println("        <div class=\"logo_text\">kids no hit.</div>");
  client.println("    </header>");
  client.println("    <div class=\"control-panel\">");
  client.println("        <label class=\"switch\">");
  client.println("            <input type=\"checkbox\" id=\"toggleSwitch\">");
  client.println("            <span class=\"slider\"></span>");
  client.println("        </label>");
  client.println("        <p>Status: <span id=\"status\">Bus is not here!</span></p>");
  client.println("    </div>");
  client.println("    <main>");
  client.println("        <div class=\"bs_post\"></div>");
  client.println("        <div class=\"sign blue-sign\"></div>");
  client.println("        <div class=\"ys_post\"></div>");
  client.println("        <div class=\"sign yellow-sign\"></div>");
  client.println("        <div class=\"rs_post\"></div>");
  client.println("        <div class=\"container\">");
  client.println("            <div class=\"octagonWrap\">");
  client.println("                <div class=\"octagon\"></div>");
  client.println("            </div>");
  client.println("        </div>");
  // (Add any other main content elements here following this pattern)
  client.println("    </main>");
  client.println("    <script>");
  client.println("        const toggleSwitch = document.getElementById('toggleSwitch');");
  client.println("        const statusDisplay = document.getElementById('status');");
  client.println("        const busElement = document.querySelector('.bus');");
  client.println("        const childrenGroup = document.querySelector('.children-group');");
  client.println("        function updateStateFromHash() {");
  client.println("            const isHerePage = window.location.hash === '#/here';");
  client.println("            toggleSwitch.checked = isHerePage;");
  client.println("            updateUI(isHerePage);");
  client.println("        }");
  client.println("        function updateUI(isHere) {");
  client.println("            if (isHere) {");
  client.println("                statusDisplay.textContent = 'Bus is here!';");
  client.println("                busElement.classList.remove('bus-departing');");
  client.println("                busElement.classList.add('bus-arriving');");
  client.println("                childrenGroup.style.display = 'flex';");
  client.println("                fetch('/H');");
  client.println("            } else {");
  client.println("                statusDisplay.textContent = 'Bus is not here!';");
  client.println("                busElement.classList.remove('bus-arriving');");
  client.println("                busElement.classList.add('bus-departing');");
  client.println("                childrenGroup.style.display = 'none';");
  client.println("                fetch('/L');");
  client.println("            }");
  client.println("        }");
  client.println("        function handleToggleChange() {");
  client.println("            const newHash = toggleSwitch.checked ? '#/here' : '#/main';");
  client.println("            window.location.hash = newHash;");
  client.println("            updateUI(toggleSwitch.checked);");
  client.println("        }");
  client.println("        toggleSwitch.addEventListener('change', handleToggleChange);");
  client.println("        window.addEventListener('hashchange', updateStateFromHash);");
  client.println("        window.addEventListener('load', function() {");
  client.println("            if (!window.location.hash) {");
  client.println("                window.location.hash = '#/main';");
  client.println("            }");
  client.println("            updateStateFromHash();");
  client.println("        });");
  client.println("    </script>");
  client.println("</body>");
  client.println("</html>");
}