# ESP32 Request Repeater (HTTP)

A simple Arduino sketch for ESP32 that makes an HTTP requested to a supplied host then sleeps for a specified period before doing it again.

## Background

I wanted to learn how to use the power management capabilities of the ESP32 processor and this is the sketch I created to help me learn. I'd been doing a lot of work sending HTTP and HTTPS requests from an ESP32 device, so that made me think of creating a periodic requester sketch.

The ideas is that there's some device that needs to phone home periodically. 

## Configuration

To configure the sketch, copy the repository's `config.h.rename` to `config.h` (in the same folder) then update the contents of the new file with the settings for your particular environment.

```c
#define REMOTE_HOST "http://someremotehost.com/something"
#define SLEEP_DURATION_MINUTES 1.5
#define WIFI_SSID "MyLocalNetwork"
#define WIFI_PASSWORD "my long and complicated wi-fi password"
#define WIFI_CONNECT_LIMIT 30000  //miliseconds
```

| Config Setting         | Description |
| ---------------------- | ----------- |
| REMOTE_HOST            | The url for the system you want the sketch to trigger every time it runs. Add any query string paramaters you need as well (optional). |
| SLEEP_DURATION_MINUTES | The duration the device sleeps between requests. <br/>**Note:** Because the sketch wakes up, makes the request, then shuts down so quickly, that I wasn't able to upload sketch updates to the device. To resolve this, I added a 30 second wait at the beginning of every wake period to give me time to upload sketch updates. Be sure to subtract this 30 seconds from the value you configure for this property. |
| `WIFI_SSID`            | The Wi-Fi network name (SSID) you want the device running the sketch to connect to. |
| `WIFI_PASSWORD`        | The password for the Wi-Fi network SSID specified in `WIFI_SSID` |
| WIFI_CONNECT_LIMIT | Specifies how many milliseconds the sketch should wait before abandoning connecting to the Wi-Fi network. |

Once you've populated the `config.h` file, save your changes then build and deploy the sketch to an ESP32 device.

## Operation

When the sketch runs, it:

1. Tells you how many times it's woken and made (or tried to make) a request
2. Waits for 30 seconds so you can upload sketch updates to the device
3. Connects to the Wi-Fi network
4. Sends the request
5. Puts the device to sleep for `SLEEP_DURATION_MINUTES`

Here's an example of the sketch's output to the Serial Monitor:

```text
**************************
* ESP32 Request Repeater *
**************************
Sketch restarted 4 times
Wakeup Reason: Timer
Waiting 30 seconds to allow for sketch uploads

Connecting to MyLocalNetwork
..
WiFi connected
IP address: 
192.168.86.63
Connecting to http://someremotehost.com/something
Response: 200
Success
```

**Note:** If the device cannot connect to the Wi-Fi network for `WIFI_CONNECT_LIMIT` milliseconds, the sketch puts the device to sleep hoping that when it wakes up again the Wi-Fi network connection will succeed.

***

If this code helps you, please consider buying me a coffee.

<a href="https://www.buymeacoffee.com/johnwargo" target="_blank"><img src="https://cdn.buymeacoffee.com/buttons/default-orange.png" alt="Buy Me A Coffee" height="41" width="174"></a>
