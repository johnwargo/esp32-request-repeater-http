/*********************************************************
* ESP32 Request Repeater
* 
* ESP32 sketch that wakes up periodically and sends a 
* HTTP request to a remote host. Why? Because I wanted
* to figure out how to do it.
*
* By John M. Wargo
* https://johnwargo.com
**********************************************************/

// https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html

#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_sleep.h"
#include "esp_wifi.h"

#include "config.h"

// store the credentials in the project's config.h file
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;

HTTPClient http;

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();

  Serial.println("**************************");
  Serial.println("* ESP32 Request Repeater *");
  Serial.println("* By John M. Wargo       *");
  Serial.println("**************************");

  // Check to make sure we have Wi-Fi credentials
  // before trying to use them
  if (String(ssid).isEmpty() || String(password).isEmpty()) {
    Serial.println("\nMissing Wi-Fi credentials");
    for (;;) {}
  }
}

void loop() {
  connectToNetwork();
  callRemoteHost();
  gotoSleep();
  // delay(10000);
}

void connectToNetwork() {
  int counter = 0;

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
}

void callRemoteHost() {
  Serial.print("Connecting to ");
  Serial.println(REMOTE_HOST);

  http.begin(REMOTE_HOST);  //HTTP
  int httpCode = http.GET();
  // httpCode will be negative on error
  if (httpCode > 0) {
    Serial.printf("Response: %d\n", httpCode);
    if (httpCode == HTTP_CODE_OK) {
      Serial.println("Success");
    }
    String payload = http.getString();
    Serial.println(payload);
  } else {
    Serial.printf("[HTTP] GET failed, error: %s\n", http.errorToString(httpCode).c_str());
  }
  http.end();
}

void gotoSleep() {
  Serial.println("Going to sleep");
  if (esp_sleep_enable_timer_wakeup(SLEEP_DURATION) == ESP_OK) {
    if (esp_wifi_stop() == ESP_OK)
      esp_deep_sleep_start();
  } else {
    Serial.println("Unable to set sleep timer wakeup");
  }
}