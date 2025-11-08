#include <SUSI_Slave.h>

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// Define the address for this slave
const uint8_t SLAVE_ADDRESS = 1;

// Create a SUSI_Slave instance
SUSI_Slave susi(CLOCK_PIN, DATA_PIN, SLAVE_ADDRESS);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  susi.begin();

  Serial.println("SUSI Slave Example");
}

void loop() {
  // The loop() function will check for and process incoming packets
  susi.loop();
}
