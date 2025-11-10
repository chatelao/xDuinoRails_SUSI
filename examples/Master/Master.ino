#include <susi_master.h>

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// Create the necessary SUSI objects
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Master master(hal);
SUSI_Master_API susi(master);

void setup() {
  Serial.begin(9600);
  while (!Serial);

  susi.begin();

  Serial.println("SUSI Master Example");
}

void loop() {
  // Example: Turn function 1 on for slave address 1
  Serial.println("Turning function 1 ON");
  susi.setFunction(1, 1, true);
  delay(2000);

  // Example: Turn function 1 off for slave address 1
  Serial.println("Turning function 1 OFF");
  susi.setFunction(1, 1, false);
  delay(2000);

  // Example: Write to CV 10 on slave address 1
  Serial.println("Writing 123 to CV 10");
  susi.writeCV(1, 10, 123);
  delay(2000);
}
