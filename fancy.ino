#include <WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h>  // Library for ultrasonic sensor
#include <WiFiManager.h>

// MQTT broker
const char* mqtt_server = "192.168.0.87";
const int mqtt_port = 1883;

// MQTT topics
const char* distance_topic = "ultrasonic/distance_sensor4";
const char* alive_topic = "ultrasonic/alive_sensor4";

// Ultrasonic sensor parameters
#define TRIGGER_PIN 9
#define ECHO_PIN 10
#define MAX_DISTANCE 200  // Maximum distance to measure in centimeters

// IR sensor digital output pin
#define IR_PIN 2

// WiFi client and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Ultrasonic sensor instance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// Last alive message time
unsigned long lastAliveMessage = 0;
const unsigned long aliveInterval = 900000;  // 15 minutes in milliseconds

void setup() {
  Serial.begin(115200);

  // Initialize IR sensor pin
  pinMode(IR_PIN, INPUT);

  // Create an instance of WiFiManager
  WiFiManager wifiManager;

  // Uncomment the following line if you want to reset WiFi settings
  // wifiManager.resetSettings();

  // Start WiFiManager
  // If WiFiManager does not detect saved WiFi credentials, it will start an access point
  // You can connect to this access point and configure WiFi settings
  wifiManager.autoConnect("sensor4");

  Serial.println("Connected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqtt_server, mqtt_port);
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  unsigned long currentMillis = millis();

  if (digitalRead(IR_PIN) == HIGH) {
    // Measure distance using ultrasonic sensor
    unsigned int uS = sonar.ping();
    unsigned int distance = uS / US_ROUNDTRIP_CM;

    // Print distance to serial monitor
    Serial.print("Distance: ");
    Serial.print(distance);
    Serial.println(" cm");

    // Convert distance to string
    char payload[10];
    sprintf(payload, "%d", distance);

    // Publish distance to MQTT broker
    if (client.publish(distance_topic, payload)) {
      Serial.println("Distance message sent to MQTT broker!");
    } else {
      Serial.println("Failed to send distance message to MQTT broker!");
    }

    delay(500);  // Adjust the delay time as needed
  }

  // Send alive message every 15 minutes
  if (currentMillis - lastAliveMessage > aliveInterval) {
    lastAliveMessage = currentMillis;
    if (client.publish(alive_topic, "alive")) {
      Serial.println("Alive message sent to MQTT broker!");
    } else {
      Serial.println("Failed to send alive message to MQTT broker!");
    }
  }
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client")) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
