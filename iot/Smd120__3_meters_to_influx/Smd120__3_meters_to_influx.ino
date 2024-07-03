#include <InfluxDbClient.h>
#include <WiFiMulti.h>
#include <InfluxDbCloud.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>

#ifdef ESP32
#include <WiFi.h>
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#else  // For ESP8266
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#endif

WiFiMulti wifiMulti;

// Modbus addresses
#define MODBUS_ID1 1
#define MODBUS_ID2 2
#define MODBUS_ID3 3

// WiFi credentials and InfluxDB settings
#define WIFI_SSID "TTTA@DOM"
#define WIFI_PASSWORD "Ttta@2021"
#define INFLUXDB_URL "http://119.59.103.144:8086"
#define INFLUXDB_TOKEN "WpvK5OCW4Nxdhr69aeKpaqOiq_Qs1w7-7nJHpV50cQ7mVvuV6KEciyynxtudqZo7fO0DaxoCN8RhA2USk6jRtw=="
#define INFLUXDB_ORG "TTTA"
#define INFLUXDB_BUCKET "Energy-TTTA"
#define TZ_INFO "<+07>-7"

// OTA password
#define OTA_PASSWORD "12345678"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);
Point sensor("Read SMD120 3 meters");

// Pins for SoftwareSerial
#define RX_PIN 15  // Example pin number for RX
#define TX_PIN 4   // Example pin number for TX

SoftwareSerial modbusSerial(RX_PIN, TX_PIN);
ModbusMaster node1, node2, node3;

void preTransmission() {
  delayMicroseconds(200);
}

void postTransmission() {
  delayMicroseconds(200);
}

void setup() {
  Serial.begin(115200);
  modbusSerial.begin(9600);
  node1.begin(MODBUS_ID1, modbusSerial);
  node2.begin(MODBUS_ID2, modbusSerial);
  node3.begin(MODBUS_ID3, modbusSerial);
  node1.preTransmission(preTransmission);
  node1.postTransmission(postTransmission);
  node2.preTransmission(preTransmission);
  node2.postTransmission(postTransmission);
  node3.preTransmission(preTransmission);
  node3.postTransmission(postTransmission);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println("Test upload via ip address");
  Serial.println("Connected to WiFi");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Initialize OTA updates with password protection
  ArduinoOTA.setHostname("myesp32");  // Set a hostname for the device
  ArduinoOTA.setPassword(OTA_PASSWORD);

  ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else {  // U_SPIFFS
      type = "filesystem";
    }
    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
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

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

float reform_uint16_2_float32(uint16_t u1, uint16_t u2) {
  uint32_t num = ((uint32_t)u1 << 16) | u2;
  float numf;
  memcpy(&numf, &num, sizeof(numf));
  return numf;
}

float getRTU(ModbusMaster& node, uint16_t m_startAddress) {
  node.clearResponseBuffer();
  uint16_t result = node.readInputRegisters(m_startAddress, 2);
  if (result == node.ku8MBSuccess) {
    return reform_uint16_2_float32(node.getResponseBuffer(0), node.getResponseBuffer(1));
  }
  return 0;  // Default return value in case of failure
}

void readAndLogEnergyData(ModbusMaster& node, const String& deviceId) {
  float voltage = getRTU(node, 0x0000);
  float current = getRTU(node, 0x0006);
  float power = getRTU(node, 0x000C);
  float energy = getRTU(node, 0x0156);
  float pf = getRTU(node, 0x001E);
  float freq = getRTU(node, 0x0046);

  sensor.clearFields();
  sensor.addField("Voltage" + deviceId, voltage);
  sensor.addField("Current" + deviceId, current);
  sensor.addField("Power" + deviceId, power);
  sensor.addField("Energy" + deviceId, energy);
  sensor.addField("PowerFactor" + deviceId, pf);
  sensor.addField("Frequency" + deviceId, freq);

  Serial.print("Device " + deviceId + ": ");
  Serial.print("V: ");
  Serial.print(voltage);
  Serial.print(" A: ");
  Serial.print(current);
  Serial.print(" W: ");
  Serial.print(power);
  Serial.print(" kWh: ");
  Serial.print(energy);
  Serial.print(" PF: ");
  Serial.print(pf);
  Serial.print(" Hz: ");
  Serial.println(freq);

  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  ArduinoOTA.handle();  // Handle OTA updates

  readAndLogEnergyData(node1, "1");
  readAndLogEnergyData(node2, "2");
  readAndLogEnergyData(node3, "3");
  delay(5000);  // Ensure there's a delay to prevent spamming the loop
}
