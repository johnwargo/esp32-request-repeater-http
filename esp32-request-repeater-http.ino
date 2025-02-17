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

#include "config.h"

// store the credentials in the project's config.h file
const char* ssid = WIFI_SSID;
const char* password = WIFI_PASSWORD;
const long minutes2Microseconds = 60000000;

// Create a restart counter to track how many times the sketch restarts,
// store in the RTC (realtime clock) storage area
RTC_DATA_ATTR unsigned int restartCounter = 0;

HTTPClient http;

void setup() {

  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("*******************************");
  Serial.println("* ESP32 Request Repeater HTTP *");
  Serial.println("*******************************");

  // Check to make sure we have Wi-Fi credentials
  // before trying to use them
  if (String(ssid).isEmpty() || String(password).isEmpty()) {
    Serial.println("\nMissing Wi-Fi credentials");
    for (;;) {}
  }

  restartCounter += 1;
  if (restartCounter > 1) Serial.printf("Sketch restarted %d times\n", restartCounter);
  displayWakeupReason();

  // Wait 30 seconds to provide some time to deploy updates to the sketch otherwise it will
  // disconnect as soon as its done and you won't be able to save updates to the device
  Serial.println("Waiting 30 seconds to allow for sketch uploads");
  delay(30000);

  esp_sleep_enable_timer_wakeup(SLEEP_DURATION_MINUTES * minutes2Microseconds);
  if (connectToNetwork()) callRemoteHost();
  esp_deep_sleep_start();
}

void loop() {
  // nothing to do here, its all done in setup()
}

bool connectToNetwork() {
  unsigned long connectionStart;
  int counter = 0;

  Serial.printf("\nConnecting to %s\n", ssid);
  connectionStart = millis();
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
    // How long have we been trying to connect to wi-fi?
    if (millis() - connectionStart > WIFI_CONNECT_LIMIT) {
      Serial.println("\nUnable to connect to network, aborting");
      return false;
    }
    counter += 1;
    if (counter > 25) {
      counter = 0;
      Serial.println();
    }
  }
  Serial.println();
  Serial.println("WiFi connected\nIP address: ");
  Serial.println(WiFi.localIP());
  return true;
}

void callRemoteHost() {
  Serial.printf("Connecting to %s\n", REMOTE_HOST);
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

// docs: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/sleep_modes.html#_CPPv418esp_sleep_source_t
void displayWakeupReason() {

  esp_sleep_wakeup_cause_t wakeupReason;
  String msg;

  wakeupReason = esp_sleep_get_wakeup_cause();
  switch (wakeupReason) {
    case ESP_SLEEP_WAKEUP_BT: msg = "BT (light sleep only)"; break;  // added for completeness
    case ESP_SLEEP_WAKEUP_COCPU: msg = "COCPU int"; break;
    case ESP_SLEEP_WAKEUP_COCPU_TRAP_TRIG: msg = "COCPU crash"; break;
    case ESP_SLEEP_WAKEUP_EXT0: msg = "External signal using RTC_IO"; break;
    case ESP_SLEEP_WAKEUP_EXT1: msg = "External signal using RTC_CNTL"; break;  // added for completeness
    case ESP_SLEEP_WAKEUP_GPIO: msg = "GPIO (light sleep only on ESP32, S2 and S3)"; break;
    case ESP_SLEEP_WAKEUP_TIMER: msg = "Timer"; break;  // this is the one we care about
    case ESP_SLEEP_WAKEUP_TOUCHPAD: msg = "Touchpad"; break;
    case ESP_SLEEP_WAKEUP_UART: msg = "UART (light sleep only)"; break;  // added for completeness
    case ESP_SLEEP_WAKEUP_ULP: msg = "ULP program"; break;
    // means sketch started because a new version of the sketch was loaded or powered up not from sleep (fresh start)
    case ESP_SLEEP_WAKEUP_UNDEFINED: msg = "Not caused by exit from deep sleep"; break;
    // this one's in the docs, but not in the library apparently
    // case ESP_SLEEP_WAKEUP_VAD: msg = "Wakeup caused by VAD"; break;
    case ESP_SLEEP_WAKEUP_WIFI: msg = "WIFI (light sleep only)"; break;  // added for completeness
    default: msg = "Unknown (" + String(wakeupReason) + ")"; break;
  }
  Serial.print("Wakeup Reason: ");
  Serial.println(msg);
}