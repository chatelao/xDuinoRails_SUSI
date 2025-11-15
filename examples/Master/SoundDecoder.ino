#include <susi_master.h>

// This example demonstrates how to use the SUSI_Master_API to find and
// control a SUSI-compatible sound decoder.

// Define the pins for the SUSI bus
const uint8_t CLOCK_PIN = 2;
const uint8_t DATA_PIN = 3;

// Create the necessary SUSI objects
SusiHAL hal(CLOCK_PIN, DATA_PIN);
SUSI_Master master(hal);
SUSI_Master_API susi(master);

// Store the address of the first sound decoder we find
uint8_t decoderAddress = 0;

void setup() {
  Serial.begin(9600);
  while (!Serial) {
    // Wait for the serial port to connect. Needed for native USB port only.
  }

  Serial.println("SUSI Sound Decoder Controller");

  // Initialize the SUSI bus
  susi.begin();

  // Perform the handshake to discover any connected SUSI slaves
  Serial.println("Searching for SUSI slaves...");
  SusiMasterResult result = susi.performHandshake();

  if (result == SUCCESS) {
    // The handshake function automatically populates an internal list of slaves.
    // We can't get the list directly, but we can assume the first one found
    // is our sound decoder. For this example, we'll try to control address 1,
    // which is the first valid slave address.
    decoderAddress = 1; // Assuming the first slave is at address 1
    Serial.print("Handshake successful. Assuming decoder is at address ");
    Serial.println(decoderAddress);
    Serial.println("\n--- Ready for commands ---");
    Serial.println("Send 'h' to toggle horn (Function 1)");
    Serial.println("Send 'b' to toggle bell (Function 2)");

  } else {
    Serial.println("Handshake failed or no slaves found. Please check wiring.");
    Serial.println("The program will not proceed.");
    while(1); // Halt execution
  }
}

void loop() {
  // Check if the user has sent a command over the Serial Monitor
  if (Serial.available() > 0) {
    char command = Serial.read();

    if (decoderAddress != 0) {
      bool current_state;
      SusiMasterResult result;

      switch (command) {
        case 'h': // Horn on/off (Function 1)
          current_state = susi.getFunction(decoderAddress, 1);
          Serial.print("Toggling Horn (F1) -> ");
          result = susi.setFunction(decoderAddress, 1, !current_state);
          if (result == SUCCESS) {
            Serial.println(!current_state ? "ON" : "OFF");
          } else {
            Serial.println("Failed!");
          }
          break;

        case 'b': // Bell on/off (Function 2)
          current_state = susi.getFunction(decoderAddress, 2);
          Serial.print("Toggling Bell (F2) -> ");
          result = susi.setFunction(decoderAddress, 2, !current_state);
          if (result == SUCCESS) {
            Serial.println(!current_state ? "ON" : "OFF");
          } else {
            Serial.println("Failed!");
          }
          break;
      }
    }
  }
}
