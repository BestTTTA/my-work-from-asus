#include <WiFiMulti.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <ModbusMaster.h>
#include <HardwareSerial.h>
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>


const char* ssid = "TTTA@DOM";
const char* password = "Ttta@2021";

AsyncWebServer server(80);

void preTransmission() {
  delayMicroseconds(200);
}

void postTransmission() {
  delayMicroseconds(200);
}


#define WIFI_SSID "TTTA@DOM"
#define WIFI_PASSWORD "Ttta@2021"

#define INFLUXDB_URL "http://119.59.103.144:8086"
#define INFLUXDB_TOKEN "yjRt6g6ItriIEfBZJSnvnDJCVmlVgQPaTBSzFUaPRKaK1hCC9x_E7x5a2Cuv8tTQGXmypg7dv2UEuJ2CWE97_g=="
#define INFLUXDB_ORG "TTTA"
#define INFLUXDB_BUCKET "DOM"

#define TZ_INFO "<+07>-7"

#define modbusaddr 1
WiFiMulti wifiMulti;
ModbusMaster node;

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("Grid_input_inverter_1");

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 15, 4);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(WIFI_SSID);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/plain", "Hi! I am ESP32.");
  });

  AsyncElegantOTA.begin(&server);
  server.begin();
  Serial.println("HTTP server started");

  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  node.begin(modbusaddr, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

float reform_uint16_2_float32(uint16_t u1, uint16_t u2) {
  uint32_t num = ((uint32_t)u1 & 0xFFFF) << 16 | ((uint32_t)u2 & 0xFFFF);
  float numf;
  memcpy(&numf, &num, 4);
  return numf;
}

float getRTU(uint16_t m_startAddress) {
  uint8_t m_length = 2;
  uint16_t result;
  float x;

  node.clearResponseBuffer();

  result = node.readInputRegisters(m_startAddress, m_length);
  if (result == node.ku8MBSuccess) {
    return reform_uint16_2_float32(node.getResponseBuffer(0), node.getResponseBuffer(1));
  }
}

void loop() {
  float Vol = getRTU(0x0000);
  float Wat = getRTU(0x000C);
  float Cur = getRTU(0x0006);
  float Fre = getRTU(0x0046);
  float TotalWh = getRTU(0x0156);
  float PF = getRTU(0x001E);

  sensor.clearFields();

  sensor.addField("Volt", Vol);
  sensor.addField("Watts", Wat);
  sensor.addField("Amp", Cur);
  sensor.addField("kWh", TotalWh);
  sensor.addField("HZ", Fre);
  sensor.addField("PF", PF);

  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Wait 5s");
  delay(5000);
}