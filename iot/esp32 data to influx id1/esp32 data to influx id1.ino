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
#include <HardwareSerial.h>

ModbusMaster node;
#define modbusaddr 1




void preTransmission()
{
  delayMicroseconds(200);
}

void postTransmission()
{
  delay(2);
}
#define WIFI_SSID "TTTA@DOM"
#define WIFI_PASSWORD "Ttta@2021"
#define INFLUXDB_URL "http://192.168.1.148:8086"
#define INFLUXDB_TOKEN "0jqTz39LNU_sJSHBoKko9JyE94uL5Z6rd_fUji5on1OEciW2JcBFQW8fzP8F87RttbMPRtz4p5279zpzt0RVtg=="
#define INFLUXDB_ORG "The tiger team academy"
#define INFLUXDB_BUCKET "esp32 main"
#define TZ_INFO "<+07>-7"

InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point sensor("Test");

void setup() {
  Serial.begin(115200);
  Serial2.begin(9600, SERIAL_8N1, 15, 4); 

   WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

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
   node.begin(modbusaddr, Serial2);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}


float reform_uint16_2_float32(uint16_t u1, uint16_t u2)
{  
  uint32_t num = ((uint32_t)u1 & 0xFFFF) << 16 | ((uint32_t)u2 & 0xFFFF);
  float numf;
  memcpy(&numf, &num, 4);
  return numf;
}

float getRTU(uint16_t m_startAddress){
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
sensor.addField("Currents", Cur);
sensor.addField("Total Active Energies", TotalWh);
sensor.addField("Frequencies", Fre);
sensor.addField("Pf", PF);

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


