#include <susi_slave.h>

/**
 * @file Slave.ino
 * @brief SUSI-Slave (RX) Example
 *
 * This example demonstrates a SUSI Slave, the receiving device on the bus.
 * It receives commands from a SUSI Master and reacts to them. A callback
 * function is used to respond to function changes.
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

// Define the address for this slave module
const uint8_t SLAVE_ADDRESS = 1;

// Create the necessary SUSI objects
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Slave susi(hal);

/**
 * @brief Callback function that is called when a function changes.
 *
 * This function is automatically called by the SUSI library when the master
 * sends the command to change a function.
 * @param function The number of the function (1-28) that was changed.
 * @param on true if the function is turned on, false if it is turned off.
 */
void functionChangedCallback(uint8_t function, bool on) {
  Serial.print("Function ");
  Serial.print(function);
  Serial.print(" was turned ");
  if (on) {
    Serial.println("ON.");
  } else {
    Serial.println("OFF.");
  }
}

void setup() {
  Serial.begin(9600);
  while (!Serial); // Wait for the serial connection

  // Initialize the SUSI Slave with its address
  susi.begin(SLAVE_ADDRESS);

  // Register the callback function
  susi.onFunctionChange(functionChangedCallback);

  Serial.println("SUSI Slave Example");
  Serial.println("Waiting for commands from the Master...");
}

void loop() {
  // The `susi.available()` function continuously checks for new commands.
  // If a command is received, the registered callback function is called automatically.
  susi.available();
}
