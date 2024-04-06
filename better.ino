#include <WiFi.h>
#include <PubSubClient.h>
#include <NewPing.h> // Library for ultrasonic sensor
#include <WiFiManager.h>

// MQTT broker
const char* mqtt_server = "192.168.0.37";
const int mqtt_port = 1883;
const char* mqtt_username = "abc";
const char* mqtt_password = "1234";

// MQTT topic
const char* distance_topic = "ultrasonic/distance";

// Ultrasonic sensor parameters
#define TRIGGER_PIN 10
#define ECHO_PIN 9
#define MAX_DISTANCE 200 // Maximum distance to measure in centimeters

// Threshold distance for sending data to MQTT broker
const int threshold_distance = 50; // Change this value according to your requirement

// WiFi client and MQTT client
WiFiClient espClient;
PubSubClient client(espClient);

// Ultrasonic sensor instance
NewPing sonar(TRIGGER_PIN, ECHO_PIN, MAX_DISTANCE);

// LED pin
const int LED_PIN = D6; // Assuming you're using built-in LED pin on ESP32

void setup() {
  Serial.begin(115200);

  // Configure LED pin
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW); // Initially turn off LED

  // Create an instance of WiFiManager
  WiFiManager wifiManager;

  // Uncomment the following line if you want to reset WiFi settings
  wifiManager.resetSettings();

  // Start WiFiManager
  // If WiFiManager does not detect saved WiFi credentials, it will start an access point
  // You can connect to this access point and configure WiFi settings
  wifiManager.autoConnect("ESP32AP");

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

  // Measure distance using ultrasonic sensor
  unsigned int uS = sonar.ping();
  unsigned int distance = uS / US_ROUNDTRIP_CM;

  // Print distance to serial monitor
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // Debug LED status
  Serial.print("LED Status: ");
  Serial.println(digitalRead(LED_PIN));

  // Control LED based on distance
  if (distance <= threshold_distance) {
    // If distance is less than or equal to threshold, turn on LED
    digitalWrite(LED_PIN, HIGH);
  } else {
    // If distance exceeds threshold, turn off LED
    digitalWrite(LED_PIN, LOW);
  }

  // Check if distance is less than threshold
  if (distance < threshold_distance) {
    // Convert distance to string
    char payload[10];
    sprintf(payload, "%d", distance);

    // Publish distance to MQTT broker
    if (client.publish(distance_topic, payload)) {
      Serial.println("Message sent to MQTT broker!");
    } else {
      Serial.println("Failed to send message to MQTT broker!");
    }
  }

  delay(2000); // Adjust the delay time as needed
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP32Client", mqtt_username, mqtt_password)) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

