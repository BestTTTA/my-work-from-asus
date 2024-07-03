#include <PubSubClient.h>
#include <ESP8266WiFi.h>
#include <ArduinoJson.h> 
// WiFi and MQTT credentials
const char* mqtt_server = "119.59.102.68";
const char* mqtt_user = "admin";
const char* mqtt_password = "Ttta@2024";
const char* ssid = "TTTA@DOM";
const char* password = "Ttta@2021";


// Pin for MQ-7 sensor
#define MQ7_PIN A0

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.println("Connected to WiFi!");

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT!");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds.");
      delay(5000);
    }
  }
}

void loop() {
  int sensorValue = analogRead(MQ7_PIN);

  // Convert the analog reading to a CO concentration in ppm (parts per million)
  float voltage = sensorValue * (3.3 / 1023.0);  // ESP8266 has a 10-bit ADC
  float CO_ppm = voltageToPPM(voltage);

  if (isnan(CO_ppm)) {
    Serial.println("Failed to read data from MQ-7 sensor!");
    delay(2000);
    return;
  }

  Serial.print("CO Concentration: ");
  Serial.print(CO_ppm);
  Serial.println(" ppm");

  // Create a JSON object and populate it
  StaticJsonDocument<200> doc;
  doc["CO Concentration"] = CO_ppm;
  char buffer[200];
  size_t n = serializeJson(doc, buffer);

  // Publish the JSON object
  client.publish("sensor/esp8266", buffer, n);

  delay(5000); // Publish data every 5 seconds
}

float voltageToPPM(float voltage) {
  // Placeholder function to convert voltage to CO concentration in ppm
  // Replace this with the actual conversion based on your sensor's datasheet
  // and calibration data. The MQ-7 sensor has a nonlinear response, and you
  // may need to perform a more complex calculation.
  // For now, we return a dummy value for demonstration purposes.
  return voltage * 100;  // Example conversion factor
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle incoming MQTT messages, if needed
}