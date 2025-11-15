#include <susi_slave.h>

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// Define the address for this slave
const uint8_t SLAVE_ADDRESS = 1;

// Create a SUSI_Slave instance
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Slave susi(hal);

void functionCallback(uint8_t function, bool on) {
  Serial.print("Function ");
  Serial.print(function);
  Serial.print(" was turned ");
  Serial.println(on ? "ON" : "OFF");
}

void setup() {
  Serial.begin(9600);
  while (!Serial);

  susi.begin(SLAVE_ADDRESS);
  susi.onFunctionChange(functionCallback);

  // Set the manufacturer and hardware IDs for this slave
  susi.setManufacturerID(0x1234);
  susi.setHardwareID(0x5678);
  susi.setVersionNumber(0x0102);

  // Enable bidirectional communication
  susi.enableBidirectionalMode();

  Serial.println("SUSI Slave Example");
}

void loop() {
  // The loop() function will check for and process incoming packets
  if (susi.available()) {
    SUSI_Packet packet = susi.read();
    Serial.print("Received packet: ");
    Serial.print(packet.address);
    Serial.print(" ");
    Serial.print(packet.command);
    Serial.print(" ");
    Serial.println(packet.data);
  }
}
