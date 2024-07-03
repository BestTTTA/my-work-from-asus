#include <SoftwareSerial.h>
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <PubSubClient.h>
#include <ESP8266WebServer.h>
#include <WiFiClient.h>
#include <ESP8266HTTPUpdateServer.h>

// WiFi and MQTT credentials
const char* mqtt_server = "119.59.103.144";
const char* mqtt_user = "mqtt";
const char* mqtt_password = "12345678";
const char* ssid = "TTTA@DOM";
const char* password = "Ttta@2021";
const char* ap_ssid = "admin";
const char* ap_password = "12345678";

WiFiServer telnetServer(23);
WiFiClient telnetClient;


#define RX_PIN D1
#define TX_PIN D2

SoftwareSerial mySerial(RX_PIN, TX_PIN);

WiFiClient espClient;
PubSubClient client(espClient);
ESP8266WebServer server(80);
ESP8266HTTPUpdateServer httpUpdater;
const long BAUDRATE = 2400;  // Set this to match your device's baud rate

void setup() {
  Serial.begin(115200);      // Start the hardware serial to communicate with the PC
  mySerial.begin(BAUDRATE);  // Start software serial communication
  Serial.println("Serial communication started.");

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);


  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
  }
  Serial.println("mDNS responder started");

  httpUpdater.setup(&server);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update_wifi", HTTP_POST, handleUpdateWifi);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);

  int wifi_attempts = 0;
  while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {
    delay(500);
    Serial.println("Connecting to WiFi...");
    wifi_attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("Connected to WiFi!");

    // Print IP Address
    IPAddress ip = WiFi.localIP();
    Serial.print("IP Address: ");
    Serial.println(ip);

    startOTA();
  } else {
    Serial.println("Failed to connect to WiFi, starting AP mode...");
    startAPMode();
  }

  telnetServer.begin();
  telnetServer.setNoDelay(true);

  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("Connected to MQTT!");
      client.subscribe("sensor/esp8266/buzzer");
    } else {
      Serial.print("Failed, rc=");
      Serial.print(client.state());
      Serial.println(" Trying again in 5 seconds.");
      delay(5000);
    }
  }
}

void loop() {
  server.handleClient();
  MDNS.update();
  handleTelnet();
  if (!client.connected()) {
    reconnect();
  }
  client.loop();

  String command = "5150494753B7A90D";
  // 5150494753B7A90D // Example hexadecimal command
  sendCommandViaSerial(command);  // Send the command
  delay(10000);                   // Wait for 10 seconds before sending the next command
}

void sendCommandViaSerial(String command) {
  Serial.println("Sending command to device...");
  hexStringToBytes(command);  // Convert and send hex command as bytes

  unsigned long startTime = millis();
  while (!mySerial.available()) {
    if (millis() - startTime > 10000) {  // 10-second timeout
      Serial.println("Error: Response timeout");
      return;  // Exit the function if a timeout occurs
    }
  }

  // String response = "";
  String response = mySerial.readStringUntil('\n');
  while (mySerial.available()) {
    char c = mySerial.read();
    response += c;
  }

  response.trim();  // Remove any leading/trailing whitespace
  Serial.print("Received: ");
  Serial.println(response);


  if (command == "504561D0700D") {
    client.publish("sensor/esp8266/buzzer/state", "ON");
  } else if (command == "504461E3410D") {
    client.publish("sensor/esp8266/buzzer/state", "OFF");
  }

  // Process response
  if (response.length() > 0) {
    processResponse(response);
  }
}

// Function to convert a hex string to a byte array and send it directly
void hexStringToBytes(String hexString) {
  int len = hexString.length();
  for (int i = 0; i < len; i += 2) {
    String byteString = hexString.substring(i, i + 2);
    byte b = (byte)strtol(byteString.c_str(), NULL, 16);
    mySerial.write(b);
  }
  mySerial.write('\r');  // Assuming carriage return is needed
}

void handleTelnet() {
  if (telnetServer.hasClient()) {
    if (!telnetClient || !telnetClient.connected()) {
      if (telnetClient) telnetClient.stop();
      telnetClient = telnetServer.available();
    } else {
      telnetServer.available().stop();
    }
  }
}

void serialPrint(const String& message) {
  Serial.print(message);
  if (telnetClient && telnetClient.connected()) {
    telnetClient.print(message);
  }
}


void processResponse(String response) {
  Serial.println("Response: " + response);

  // Converting response to a char array
  char buffer[200];
  response.toCharArray(buffer, 200);

  // Extract values based on known positions in the buffer array
  float grid_voltage = atof(String(buffer).substring(1, 6).c_str());
  float grid_frequency = atof(String(buffer).substring(7, 11).c_str());
  float ac_output_voltage = atof(String(buffer).substring(12, 17).c_str());
  float ac_output_frequency = atof(String(buffer).substring(18, 22).c_str());
  int ac_output_apparent_power = atoi(String(buffer).substring(23, 27).c_str());
  int ac_output_active_power = atoi(String(buffer).substring(28, 32).c_str());
  int output_load_percent = atoi(String(buffer).substring(33, 36).c_str());
  int bus_voltage = atoi(String(buffer).substring(37, 40).c_str());
  float battery_voltage = atof(String(buffer).substring(41, 46).c_str());
  int battery_charging_current = atoi(String(buffer).substring(47, 50).c_str());
  int battery_capacity = atoi(String(buffer).substring(51, 54).c_str());
  int inverter_heat_sink_temperature = atoi(String(buffer).substring(55, 59).c_str());
  int pv_input_current_for_battery = atoi(String(buffer).substring(60, 64).c_str());
  float pv_input_voltage_1 = atof(String(buffer).substring(65, 70).c_str());
  float battery_voltage_from_scc = atof(String(buffer).substring(71, 76).c_str());
  float battery_discharge_current = atof(String(buffer).substring(77, 82).c_str());
  int device_status = strtol(String(buffer).substring(83, 91).c_str(), NULL, 2);
  int battery_voltage_offset = atoi(String(buffer).substring(92, 94).c_str());
  int eeprom_version = atoi(String(buffer).substring(95, 97).c_str());
  int pv_charging_power = atoi(String(buffer).substring(98, 103).c_str());
  int device_status_2 = strtol(String(buffer).substring(104, 107).c_str(), NULL, 2);

  // Calculate battery energy based on battery capacity
  float battery_energy = (14 / 100.0) * battery_capacity;

  float grid_energy;
  if (battery_charging_current > 0) {
    grid_energy = grid_voltage * battery_charging_current;
  } else {
    grid_energy = 0;
  }

  float battery_comsumtion = grid_energy + pv_charging_power;

  // Print extracted values
  Serial.println("Grid Voltage: " + String(grid_voltage));
  Serial.println("Grid Frequency: " + String(grid_frequency));
  Serial.println("AC Output Voltage: " + String(ac_output_voltage));
  Serial.println("AC Output Frequency: " + String(ac_output_frequency));
  Serial.println("AC Output Apparent Power: " + String(ac_output_apparent_power));
  Serial.println("AC Output Active Power: " + String(ac_output_active_power));
  Serial.println("Output Load Percent: " + String(output_load_percent));
  Serial.println("Bus Voltage: " + String(bus_voltage));
  Serial.println("Battery Voltage: " + String(battery_voltage));
  Serial.println("Battery Charging Current: " + String(battery_charging_current));
  Serial.println("Battery Capacity: " + String(battery_capacity));
  Serial.println("Inverter Heat Sink Temperature: " + String(inverter_heat_sink_temperature));
  Serial.println("PV Input Current for Battery: " + String(pv_input_current_for_battery));
  Serial.println("PV Input Voltage 1: " + String(pv_input_voltage_1));
  Serial.println("Battery Voltage from SCC: " + String(battery_voltage_from_scc));
  Serial.println("Battery Discharge Current: " + String(battery_discharge_current));
  Serial.println("Device Status: " + String(device_status, BIN));
  Serial.println("Battery Voltage Offset: " + String(battery_voltage_offset));
  Serial.println("EEPROM Version: " + String(eeprom_version));
  Serial.println("PV Charging Power: " + String(pv_charging_power));
  Serial.println("Device Status 2: " + String(device_status_2, BIN));
  Serial.println("Battery Energy: " + String(battery_energy));
  Serial.println("Grid Energy: " + String(grid_energy));
  Serial.println("Battery Consumtion: " + String(battery_comsumtion));

  // Publish each value to separate MQTT topics
  publishMQTT("sensor/esp8266/grid_voltage", grid_voltage);
  publishMQTT("sensor/esp8266/grid_frequency", grid_frequency);
  publishMQTT("sensor/esp8266/ac_output_voltage", ac_output_voltage);
  publishMQTT("sensor/esp8266/ac_output_frequency", ac_output_frequency);
  publishMQTT("sensor/esp8266/ac_output_apparent_power", ac_output_apparent_power);
  publishMQTT("sensor/esp8266/ac_output_active_power", ac_output_active_power);
  publishMQTT("sensor/esp8266/output_load_percent", output_load_percent);
  publishMQTT("sensor/esp8266/bus_voltage", bus_voltage);
  publishMQTT("sensor/esp8266/battery_voltage", battery_voltage);
  publishMQTT("sensor/esp8266/battery_charging_current", battery_charging_current);
  publishMQTT("sensor/esp8266/battery_capacity", battery_capacity);
  publishMQTT("sensor/esp8266/inverter_heat_sink_temperature", inverter_heat_sink_temperature);
  publishMQTT("sensor/esp8266/pv_input_current_for_battery", pv_input_current_for_battery);
  publishMQTT("sensor/esp8266/pv_input_voltage_1", pv_input_voltage_1);
  publishMQTT("sensor/esp8266/battery_voltage_from_scc", battery_voltage_from_scc);
  publishMQTT("sensor/esp8266/battery_discharge_current", battery_discharge_current);
  publishMQTT("sensor/esp8266/device_status", device_status);
  publishMQTT("sensor/esp8266/battery_voltage_offset", battery_voltage_offset);
  publishMQTT("sensor/esp8266/eeprom_version", eeprom_version);
  publishMQTT("sensor/esp8266/pv_charging_power", pv_charging_power);
  publishMQTT("sensor/esp8266/device_status_2", device_status_2);
  publishMQTT("sensor/esp8266/battery_energy", battery_energy);
  publishMQTT("sensor/esp8266/grid_energy", grid_energy);
  publishMQTT("sensor/esp8266/battery_consumtion", battery_comsumtion);
}


void publishMQTT(const char* topic, float value) {
  StaticJsonDocument<100> doc;
  doc["value"] = value;
  char jsonBuffer[100];
  size_t n = serializeJson(doc, jsonBuffer);
  client.publish(topic, jsonBuffer, n);
}

void callback(char* topic, byte* payload, unsigned int length) {
  String message;
  for (unsigned int i = 0; i < length; i++) {
    message += (char)payload[i];
  }
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  Serial.println(message);

  // Handle buzzer control
  if (String(topic) == "sensor/esp8266/buzzer") {
    if (message == "ON") {
      sendCommandViaSerial("504561D0700D");  // Turn on buzzer
    } else if (message == "OFF") {
      sendCommandViaSerial("504461E3410D");  // Turn off buzzer
    }
  }
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqtt_user, mqtt_password)) {
      Serial.println("connected");
      client.subscribe("sensor/esp8266/buzzer");  // Subscribe to buzzer topic
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void handleRoot() {
  String html = "<html><body>";
  html += "<h1>ESP8266 Control Panel</h1>";
  html += "<h2>WiFi Settings</h2>";
  html += "<form action='/update_wifi' method='post'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Update WiFi'>";
  html += "</form>";
  html += "<h2>OTA Update</h2>";
  html += "<p>Go to http://" + WiFi.localIP().toString() + "/update to perform an OTA update</p>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void startOTA() {
  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }
    Serial.println("Start updating " + type);
  });
  ArduinoOTA.onEnd([]() {
    Serial.println("\nEnd");
  });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
    Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
  });
  ArduinoOTA.onError([](ota_error_t error) {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) {
      Serial.println("Auth Failed");
    } else if (error == OTA_BEGIN_ERROR) {
      Serial.println("Begin Failed");
    } else if (error == OTA_CONNECT_ERROR) {
      Serial.println("Connect Failed");
    } else if (error == OTA_RECEIVE_ERROR) {
      Serial.println("Receive Failed");
    } else if (error == OTA_END_ERROR) {
      Serial.println("End Failed");
    }
  });
  ArduinoOTA.begin();
}

void startAPMode() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ap_ssid, ap_password);

  IPAddress myIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(myIP);

  if (!MDNS.begin("esp8266")) {
    Serial.println("Error setting up MDNS responder!");
  }

  httpUpdater.setup(&server);
  server.on("/", HTTP_GET, handleRoot);
  server.on("/update_wifi", HTTP_POST, handleUpdateWifi);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");

  MDNS.addService("http", "tcp", 80);
}

void handleUpdateWifi() {
  String new_ssid = server.arg("ssid");
  String new_password = server.arg("password");

  if (new_ssid.length() > 0 && new_password.length() > 0) {
    Serial.println("Updating WiFi credentials...");
    Serial.println("New SSID: " + new_ssid);
    Serial.println("New Password: " + new_password);

    // Save new SSID and Password (implement your own method to save these permanently if needed)
    WiFi.begin(new_ssid.c_str(), new_password.c_str());

    int wifi_attempts = 0;
    while (WiFi.status() != WL_CONNECTED && wifi_attempts < 20) {
      delay(500);
      Serial.println("Connecting to new WiFi...");
      wifi_attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected to new WiFi!");
      server.send(200, "text/html", "Connected to new WiFi! Please reconnect to the device.");
      startOTA();  // Restart OTA with new WiFi
    } else {
      Serial.println("Failed to connect to new WiFi.");
      server.send(200, "text/html", "Failed to connect to new WiFi.");
    }
  } else {
    server.send(400, "text/html", "Invalid SSID or Password.");
  }
}

void handleNotFound() {
  server.send(404, "text/plain", "404: Not found");
}