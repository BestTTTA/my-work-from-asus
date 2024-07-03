#include <SoftwareSerial.h>

// RS232 pins for ESP8266
#define RX_PIN D1
#define TX_PIN D2

SoftwareSerial mySerial(RX_PIN, TX_PIN);

const long BAUDRATE = 2400; // Set this to match your device's baud rate

void setup() {
  Serial.begin(115200); // Start the hardware serial to communicate with the PC
  mySerial.begin(BAUDRATE); // Start software serial communication
  Serial.println("Serial communication started.");
}

void loop() {
  String command = "5150494753"; // Hexadecimal representation of "QPIGS"
  sendCommandViaSerial(command); // Send the command
  delay(10000); // Wait for 10 seconds before sending the next command
}

void sendCommandViaSerial(String command) {
  Serial.println("Sending command to device...");
  hexStringToBytes(command); // Convert and send hex command as bytes

  unsigned long startTime = millis();
  while (!mySerial.available()) {
    if (millis() - startTime > 10000) { // 10-second timeout
      Serial.println("Error: Response timeout");
      return; // Exit the function if a timeout occurs
    }
  }

  // String response = "";
   String response = mySerial.readStringUntil('\n');
  while (mySerial.available()) {
    char c = mySerial.read();
    response += c;
  }

  response.trim(); // Remove any leading/trailing whitespace
  Serial.print("Received: ");
  Serial.println(response);

  // Process response
  if (response.length() > 0) {
    processResponse(response);
  }
}

// Function to convert a hex string to a byte array and send it directly
void hexStringToBytes(String hexString) {
  int len = hexString.length();
  for (int i = 0; i < len; i += 2) {
    String byteString = hexString.substring(i, i+2);
    byte b = (byte) strtol(byteString.c_str(), NULL, 16);
    mySerial.write(b);
  }
  mySerial.write('\r'); // Assuming carriage return is needed
}

void processResponse(String response) {
  Serial.println("Response: " + response);

  // Example response processing, needs to be adjusted according to actual response format
  char buffer[200];
  response.toCharArray(buffer, 200);



}
