/*********************************************************
* ESP32 Parrot Server
* 
* Web server running on the ESP32 that simply parrots 
* backl the received request
*
* By John M. Wargo
* https://johnwargo.com
**********************************************************/

#include <WiFi.h>
#include <ESPmDNS.h>
#include <WebServer.h>

#include "config.h"

WebServer server(80);

// store the credentials in the project's constants.h file
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

TaskHandle_t Task0;
TaskHandle_t Task1;

void setup() {

  int counter = 0;

  Serial.begin(115200);
  delay(1000);
  Serial.println();

  Serial.println("\n***********************");
  Serial.println("* ESP32 Parrot Server * ");
  Serial.println("* By John M. Wargo.   * ");
  Serial.println("***********************");

  // Check to make sure we have Wi-Fi credentials
  // before trying to use them
  if (String(ssid).isEmpty() || String(password).isEmpty()) {
    Serial.println("\nMissing Wi-Fi credentials");
    for (;;) {}
  }

  // connect to the Wi-Fi network
  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter += 1;
    if (counter > 25) {
      counter = 0;
      Serial.println();
    }
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  if (MDNS.begin(HOSTNAME)) {
    displayMessage("MDNS responder started");
    MDNS.addService("http", "tcp", 80);
  } else {
    displayMessage("Error setting up MDNS responder!");
    while (1) {
      delay(1000);
    }
  }

  server.enableCORS();
  server.on("/", handleRoot);
  server.onNotFound(handleRequest);
  server.begin();
  displayMessage("HTTP server started");
}

void loop() {
  server.handleClient();
  delay(25);
}

void handleRoot() {
  displayMessage("Root (/)\n");
  handleRequest();
}

void handleRequest() {
  displayMessage("Request: " + server.uri());
  String message = "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(200, "text/plain", message);
}

void displayMessage(String msg) {
  Serial.print("Web server: ");
  Serial.println(msg);
}
