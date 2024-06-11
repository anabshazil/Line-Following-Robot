#include <Arduino.h>
#include <WiFi.h>
#include <ThingSpeak.h>

const char* ssid = "AB";          
const char* password = "12345678";

const char* thingSpeakAddress = "api.thingspeak.com";
unsigned long channelID = 2536537;
const char* writeAPIKey = "3T0JW37E2YOXMQ33";

const int voltagePin = A7;  // Analog pin connected to the voltage sensor
const int currentPin = A6;  // Analog pin connected to the current sensor

WiFiClient client;

void init_Wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  if (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.printf("WiFi Failed!\n");
    return;
  }
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void setup() {
  Serial.begin(9600);
  init_Wifi();
  ThingSpeak.begin(client);
}

void loop() {
  // Read voltage and current values
  int voltageValue = analogRead(voltagePin);
  int currentValue = analogRead(currentPin);

  // Convert the analog readings to actual voltage and current values
  float voltage = voltageValue * (5.0 / 1023.0);  // Assuming 5V reference voltage
  float current = currentValue * (5.0 / 1023.0);  // Assuming 5V reference voltage

  // Print the values to the serial monitor
  Serial.print("Voltage: ");
  Serial.print(voltage);
  Serial.print(" V, Current: ");
  Serial.print(current);
  Serial.println(" A");

  // Send data to ThingSpeak
  ThingSpeak.writeField(channelID, 1, voltage, writeAPIKey);
  ThingSpeak.writeField(channelID, 2, current, writeAPIKey);

  delay(10000); // Wait for 10 seconds before sending next data point
}
