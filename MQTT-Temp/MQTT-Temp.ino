#include <WiFi.h>
#include <Wire.h>
#include <PubSubClient.h>

#define LM73_ADDR 0x4D

#define WIFI_STA_NAME "ADSL-TOT"
#define WIFI_STA_PASS ""

#define MQTT_SERVER   "61.47.6.118"
#define MQTT_PORT     1883

WiFiClient client;
PubSubClient mqtt(client);

float readTemperature() {
  Wire1.beginTransmission(LM73_ADDR);
  Wire1.write(0x00); // Temperature Data Register
  Wire1.endTransmission();
  
  uint8_t count = Wire1.requestFrom(LM73_ADDR, 2);
  float temp = 0.0;
  if (count == 2) {
    byte buff[2];
    buff[0] = Wire1.read();
    buff[1] = Wire1.read();
    temp += (int)(buff[0]<<1);
    if (buff[1]&0b10000000) temp += 1.0;
    if (buff[1]&0b01000000) temp += 0.5;
    if (buff[1]&0b00100000) temp += 0.25;
    if (buff[0]&0b10000000) temp *= -1.0;
  }
  return temp;
}

void setup() {
  delay(100);
  Wire1.begin(4, 5);
  
  Serial.begin(115200);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_STA_NAME);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_STA_NAME, WIFI_STA_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  mqtt.setServer(MQTT_SERVER, MQTT_PORT);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
 
    if (mqtt.connect("ESP32Client")) {
      Serial.println("connected");
 
    } else {
 
      Serial.print("failed with state ");
      Serial.print(mqtt.state());
      delay(2000);
 
    }
  }
}

void loop() {
  char tempString[8];
  dtostrf(readTemperature(), 1, 2, tempString);
  
  mqtt.publish("t2", "Sensor 1");
  mqtt.publish("t1", tempString);
  
  Serial.print("Published Temperature: ");
  Serial.println(readTemperature(), 2);
  
  delay(1000);
}
