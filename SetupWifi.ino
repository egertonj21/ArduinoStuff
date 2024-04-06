#include <WiFi.h>
#include <WiFiManager.h> // Make sure to install the WiFiManager library

void setup() {
  Serial.begin(115200);
  
  // Create an instance of WiFiManager
  WiFiManager wifiManager;
  
  // Uncomment the following line if you want to reset WiFi settings
  // wifiManager.resetSettings();

  // Start WiFiManager
  // If WiFiManager does not detect saved WiFi credentials, it will start an access point
  // You can connect to this access point and configure WiFi settings
  wifiManager.autoConnect("ESP8266AP");

  // Once WiFi is configured, print the assigned IP address
  Serial.println("Connected to WiFi!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Your existing code for starting the softAP can be moved here if needed
  // WiFi.softAP("ESP_AP", "123456789");
}

void loop() {
  Serial.print("Host Name:");
  Serial.println(WiFi.softAPgetHostname());
  Serial.print("Host IP:");
  Serial.println(WiFi.softAPIP());
  Serial.print("Host IPV6:");
  Serial.println(WiFi.softAPIPv6());
  Serial.print("Host SSID:");
  Serial.println(WiFi.SSID());
  Serial.print("Host Broadcast IP:");
  Serial.println(WiFi.softAPBroadcastIP());
  Serial.print("Host mac Address:");
  Serial.println(WiFi.softAPmacAddress());
  Serial.print("Number of Host Connections:");
  Serial.println(WiFi.softAPgetStationNum());
  Serial.print("Host Network ID:");
  Serial.println(WiFi.softAPNetworkID());
  Serial.print("Host Status:");
  Serial.println(WiFi.status());
  delay(1000);
}
