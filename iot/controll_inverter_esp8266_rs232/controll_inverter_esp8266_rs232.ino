#include <SoftwareSerial.h>

// Define pins for Software Serial
#define RX_PIN D1  // Adjust as per your ESP8266 pin layout
#define TX_PIN D2  // Adjust as per your ESP8266 pin layout

// Create a SoftwareSerial object
SoftwareSerial mySerial(RX_PIN, TX_PIN);

void setup() {
  Serial.begin(115200);  // Start the hardware serial to communicate with the PC
  mySerial.begin(2400);  // Start software serial communication at 9600 baud rate
  Serial.println("Serial communication started.");
}

void loop() {
  const char* commandOn = "504561D070"; // Command to turn on the buzzer "PEa<CRC16> <CR>"
  //  const char* commandOn = "504461E3410D"; // command to turn off the buzzer "PDa<CRC16> <CR>"
  sendCommand(commandOn, 10);  // Send the command to turn on the buzzer
  delay(10000); // Delay between commands to allow time for a response and avoid flooding
}

void sendCommand(const char* command, int length) {
  Serial.println("Sending command to device...");
  for (int i = 0; i < length; i += 2) {
    char hexPair[3] = {command[i], command[i+1], 0};  // Extract two hex digits
    byte dataByte = strtoul(hexPair, NULL, 16);  // Convert hex pair to a byte
    mySerial.write(dataByte);  // Send the byte
  }
  mySerial.write('\r'); // Send the carriage return as specified

  Serial.println("Command sent, waiting for response...");
  readResponse();
}

void readResponse() {
  unsigned long startTime = millis();
  String response = "";  // Initialize response string
  while (millis() - startTime < 5000) {  // 5-second timeout
    if (mySerial.available()) {
      char c = mySerial.read();  // Read a character
      if (c == '\r') break;  // Break if carriage return is detected
      response += c;  // Append character to response
    }
  }

  if (response.length() > 0) {
    Serial.print("Received: ");
    Serial.println(response);
  } else {
    Serial.println("Error: Response timeout or no data received.");
  }
}
