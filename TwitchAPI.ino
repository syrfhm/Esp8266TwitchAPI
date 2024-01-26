#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>
#include <string>
#include "FastLED.h"

// Initialize network parameters
const char ssid[] = "syrfhm";
const char password[] = "Me031097";
const int led = 2;

#define NUM_LEDS 8
#define DATA_PIN 13

CRGB leds[NUM_LEDS];

// Declare websocket client class variable
WebSocketsClient webSocket;

// Parameters for Twitch channel
const char twitch_oauth_token[] = "oauth:is0tlm2c1bsjuuqw5bw03bx57pnm35";
const char twitch_nick[] = "asyraf9797";
const char twitch_channel[] = "#asyraf9797";

void setup() {
  // Connect to WiFi
  pinMode(led, OUTPUT);
  WiFi.begin(ssid, password);
  Serial.begin(115200);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println();
  Serial.print("IP Address: "); Serial.println(WiFi.localIP());

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(20);
  leds[0].r = 66;
  leds[0].g = 245;
  leds[0].b = 173;
  FastLED.show();

  // Server address, port, and URL path
  webSocket.begin("irc-ws.chat.twitch.tv", 80, "/");
  // Event handler
  webSocket.onEvent(webSocketEvent);
  // Try every 5000ms if connection has failed
  webSocket.setReconnectInterval(5000);
}

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
  switch (type) {
    // If the websocket connection is succesful, try to join the IRC server
    case WStype_CONNECTED:
      Serial.printf("[WSc] Connected to: %s\n", payload);
      webSocket.sendTXT("PASS " + String(twitch_oauth_token) + "\r\n");
      webSocket.sendTXT("NICK " + String(twitch_nick) + "\r\n");
      webSocket.sendTXT("JOIN " + String(twitch_channel) + "\r\n");
      break;

    // If we get a response, print it to Serial
    case WStype_TEXT: {
        String payload_str = String((char*) payload);
        //Serial.print(payload_str);
        receivedMessage(payload_str);
        break;
      }

    // Handle disconnecting from the websocket
    case WStype_DISCONNECTED:
      Serial.printf("[WSc] Disconnected!\n");
      webSocket.sendTXT("PART " + String(twitch_channel) + "\r\n");
      break;
  }
}

void receivedMessage(String payload_str) {

  int start_pos, end_pos;
  String message;

  //seperate #asyraf9797 from whole message
  end_pos = payload_str.length();
  start_pos = payload_str.indexOf('#');
  message = payload_str.substring(start_pos, end_pos);

  //seperate message from #asyraf9797
  end_pos = message.length();
  start_pos = message.indexOf(':');
  message = message.substring(start_pos + 1, end_pos - 2);

  setColorLED(message);

}

void setColorLED(String message) {

  //seperate RGB
  int val_start, val_end;
  String i, r, g, b;
  int i2, r2, g2, b2;
  int marker = 0;

  for (int index = 0; index < message.length(); index++) {
    if (message[index] == ',') {
      marker++;
    }

    if (marker == 0) {
      i += message[index];
    } else if (marker == 1) {
      r += message[index];
    } else if (marker == 2) {
      g += message[index];
    } else if (marker == 3) {
      b += message[index];
    }
  }

  marker = 0;

  i2 = i.toInt();
  Serial.println(i2);

  val_start = r.indexOf(',');
  val_end = r.length();
  r = r.substring(val_start + 1, val_end);
  r2 = r.toInt();
  Serial.println(r2);

  val_start = g.indexOf(',');
  val_end = g.length();
  g = g.substring(val_start + 1, val_end);
  g2 = g.toInt();
  Serial.println(g2);

  val_start = b.indexOf(',');
  val_end = b.length();
  b = b.substring(val_start + 1, val_end);
  b2 = b.toInt();
  Serial.println(b2);

  leds[i2].r = r2;
  leds[i2].g = g2;
  leds[i2].b = b2;
  FastLED.show();
}

void loop() {
  webSocket.loop();
}
