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

  // Example: Enable bidirectional mode for slave address 1
  Serial.print("Enabling bidirectional mode for slave 1");
  SusiMasterResult result = susi.enableBidirectionalMode(1);
  printResult(result);

  if (result == SUCCESS) {
    uint32_t unique_id;
    if (susi.getUniqueId(1, unique_id)) {
      Serial.print("  -> Unique ID: 0x");
      Serial.println(unique_id, HEX);
    } else {
      Serial.println("  -> Failed to get unique ID");
    }
  }
}

void printResult(SusiMasterResult result) {
  if (result == SUCCESS) {
    Serial.println(" -> SUCCESS");
  } else if (result == TIMEOUT) {
    Serial.println(" -> TIMEOUT");
  } else if (result == INVALID_ACK) {
    Serial.println(" -> INVALID_ACK");
  } else if (result == SLAVE_ALREADY_EXISTS) {
    Serial.println(" -> SLAVE_ALREADY_EXISTS");
  } else if (result == SLAVE_LIST_FULL) {
    Serial.println(" -> SLAVE_LIST_FULL");
  }
}

void loop() {
  SusiMasterResult result;

  // Example: Turn function 1 on for slave address 1
  Serial.print("Turning function 1 ON");
  result = susi.setFunction(1, 1, true);
  printResult(result);
  delay(2000);

  // Example: Turn function 1 off for slave address 1
  Serial.print("Turning function 1 OFF");
  result = susi.setFunction(1, 1, false);
  printResult(result);
  delay(2000);

  // Example: Write to CV 10 on slave address 1
  Serial.print("Writing 123 to CV 10");
  result = susi.writeCV(1, 10, 123);
  printResult(result);
  delay(2000);

  // Example: Read from CV 10 on slave address 1
  uint8_t cvValue;
  Serial.print("Reading from CV 10");
  result = susi.readCV(1, 10, cvValue);
  printResult(result);
  if (result == SUCCESS) {
    Serial.print("  -> CV Value: ");
    Serial.println(cvValue);
  }
  delay(2000);

  // Example: Poll for bidirectional data
  Serial.println("Polling for bidirectional data...");
  susi.pollSlaves();
  delay(2000);
}
