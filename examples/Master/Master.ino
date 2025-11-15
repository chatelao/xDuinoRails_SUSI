#include <susi_master.h>

/**
 * @file Master.ino
 * @brief SUSI-Master (TX) Example
 *
 * This example demonstrates a SUSI Master, the transmitting device on the bus.
 * It sends commands to a SUSI Slave to control its functions. In this case,
 * it toggles function 1 on and off every two seconds.
 *
 * Hardware Setup:
 * - Arduino Board (e.g., Uno, Nano)
 * - Connect the CLOCK_PIN (here Pin 2) to the SUSI bus clock line.
 * - Connect the DATA_PIN (here Pin 3) to the SUSI bus data line.
 * - Ensure a common GND connection between the master and slave.
 */

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// The address of the slave to be controlled
const uint8_t SLAVE_ADDRESS = 1;

// Create the necessary SUSI objects
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Master master(hal);
SUSI_Master_API susi(master);

// State of the function (On/Off)
bool functionOn = false;

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the serial connection

  // Initialize the SUSI Master
  susi.begin();

  Serial.println("SUSI Master Example");
  Serial.println("Controls Function 1 on the Slave.");
}

void loop() {
  // Toggle the function state
  functionOn = !functionOn;

  // Send a message about the state
  if (functionOn) {
    Serial.println("Turning Function 1 ON");
  } else {
    Serial.println("Turning Function 1 OFF");
  }

  // Send the command to the slave
  SusiMasterResult result = susi.setFunction(SLAVE_ADDRESS, 1, functionOn);

  // Check the result of the transmission
  if (result == SUCCESS) {
    Serial.println("-> Command sent successfully.");
  } else {
    Serial.println("-> Error sending command.");
  }

  // Wait 2 seconds before the next transmission
  delay(2000);
}
