#include <InfluxDbClient.h>

#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include <ModbusMaster.h>
#include <SoftwareSerial.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <AsyncElegantOTA.h>

float V=0;
float A=0;
float W=0;
float Wh=0;
float PF=0;
float F=0;

#define modbusaddr 1
#define WIFI_SSID "TTTA@DOM"                                                                                                   
#define WIFI_PASSWORD "Ttta@2021"                                                                                              
#define INFLUXDB_URL "http://202.151.182.220:8086"
#define INFLUXDB_TOKEN "Pd9gfPjLZKvUTpUyksUFGCNls7PQ1Gtua0JjgxZI_4lEb3m6JqmnGAbQ_XRSANYIyDaRV_Af5WzuWVC7tqw2lg=="
#define INFLUXDB_ORG "the tiger team academy"
#define INFLUXDB_BUCKET "ttta"
#define TZ_INFO "<+07>-7"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("ESP8266GRID");

float reform_uint16_2_float32(uint16_t u1, uint16_t u2)
{
uint32_t num = ((uint32_t)u1 & 0xFFFF) << 16 | ((uint32_t)u2 & 0xFFFF);
float numf;
memcpy(&numf, &num, 4);
return numf;
}

float getRTU(ModbusMaster& node, uint16_t m_startAddress)
{
uint8_t m_length = 2;
uint16_t result;

node.preTransmission({});
node.postTransmission({});
result = node.readInputRegisters(m_startAddress, m_length);

if (result == node.ku8MBSuccess)
{
return reform_uint16_2_float32(node.getResponseBuffer(0), node.getResponseBuffer(1));
}

return 0;
}

const char* ssid = "TTTA@DOM";
const char* password = "Ttta@2021";

AsyncWebServer server(80);

void setup(void) {
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);
  Serial.println("");

    Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

   timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
    request->send(200, "text/plain", "add /update) back ip address");
  });

  AsyncElegantOTA.begin(&server);  
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
SoftwareSerial SerialMod1(D1, D2);
ModbusMaster node1;
SerialMod1.begin(9600);
node1.begin(1, SerialMod1);


float V1 = getRTU(node1, 0x0000);
float A1 = getRTU(node1, 0x0006);
float W1 = getRTU(node1, 0x000C);
float Wh1 = getRTU(node1, 0x0156);
float PF1 = getRTU(node1, 0x001E);
float F1 = getRTU(node1, 0x0046);


sensor.clearFields();


   sensor.addField("Voltage1",V1);
   delay(500);
   sensor.addField("Watt1", W1);
   delay(500);
   sensor.addField("Current1",A1);
   delay(500);
   sensor.addField("Total Active Energy1",Wh1);
   delay(500);
   sensor.addField("Frequency1",F1);
   delay(500);
   sensor.addField("Power Factor1",PF1);
   delay(500);


Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  if ((WiFi.RSSI() == 0) && (wifiMulti.run() != WL_CONNECTED)) {
    Serial.println("Wifi connection lost");
  }

 
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  
  Serial.println("Wait 1s");  
  delay(1000);
}