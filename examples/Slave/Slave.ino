#include <susi_slave.h>

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// Define the address for this slave
const uint8_t SLAVE_ADDRESS = 1;

// Create a SUSI_Slave instance
const uint32_t SLAVE_UNIQUE_ID = 0x12345678;
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Slave susi(hal, SLAVE_UNIQUE_ID);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  susi.begin(SLAVE_ADDRESS);

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
