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

void printResult(SusiMasterResult result) {
  if (result == SUCCESS) {
    Serial.println(" -> SUCCESS");
  } else if (result == TIMEOUT) {
    Serial.println(" -> TIMEOUT");
  } else if (result == INVALID_ACK) {
    Serial.println(" -> INVALID_ACK");
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
}
