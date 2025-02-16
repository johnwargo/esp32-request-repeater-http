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
const long minutes2Microseconds = 60000000;

HTTPClient http;

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("**************************");
  Serial.println("* ESP32 Request Repeater *");
  Serial.println("**************************");

  // Check to make sure we have Wi-Fi credentials
  // before trying to use them
  if (String(ssid).isEmpty() || String(password).isEmpty()) {
    Serial.println("\nMissing Wi-Fi credentials");
    for (;;) {}
  }

  print_wakeup_reason();
  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_MINUTES * minutes2Microseconds);
  if (connectToNetwork()) callRemoteHost();
  esp_deep_sleep_start();
}

void loop() {
  // nothing to do here, its all done in setup()
}

bool connectToNetwork() {
  int counter = 0;

  Serial.print("\nConnecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    counter += 1;
    // Check the max counter, should we keep
    // trying to connect to Wi-Fi?
    if (xcounter > WIFI_CONNECT_LIMIT) {
      Serial.println("Unable to connect to network, aborting");
      return false;
    }
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
  return true;
}

void callRemoteHost() {
  Serial.print("Connecting to ");
  Serial.println(REMOTE_HOST);

  http.begin(REMOTE_HOST);
  int httpCode = http.GET();
  if (httpCode > 0) {  // httpCode will be negative on error
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

// From https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
void print_wakeup_reason() {
  esp_sleep_wakeup_cause_t wakeupReason;

  wakeupReason = esp_sleep_get_wakeup_cause();
  switch (wakeupReason) {
    case ESP_SLEEP_WAKEUP_EXT0: Serial.println("Wakeup caused by external signal using RTC_IO"); break;
    case ESP_SLEEP_WAKEUP_EXT1: Serial.println("Wakeup caused by external signal using RTC_CNTL"); break;
    case ESP_SLEEP_WAKEUP_TIMER: Serial.println("Wakeup caused by timer"); break;
    case ESP_SLEEP_WAKEUP_TOUCHPAD: Serial.println("Wakeup caused by touchpad"); break;
    case ESP_SLEEP_WAKEUP_ULP: Serial.println("Wakeup caused by ULP program"); break;
    default: Serial.printf("Wakeup not caused by deep sleep: %d\n", wakeupReason); break;
  }
}