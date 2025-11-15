# SUSI-Slave Example

This example demonstrates how to use the `SUSI.h` library to implement a **SUSI Slave** (receiver).

## Use Case

The SUSI Slave is a peripheral device that listens for commands from the SUSI Master. This example waits for commands and prints a message to the serial monitor when the state of a function is changed.

## Hardware Setup

1.  **Arduino as Slave**:
    *   Connect Pin 2 of the Arduino to the **Clock line** of the SUSI bus.
    *   Connect Pin 3 of the Arduino to the **Data line** of the SUSI bus.
    *   Connect the GND of the Arduino to the GND of the Master.

2.  **Arduino as Master**:
    *   Upload the `Master` example to a second Arduino.
    *   Connect the Clock, Data, and GND lines in parallel with the Slave Arduino.

## How it Works

-   The code initializes the SUSI Slave with a fixed address (here `1`).
-   A **callback function** (`functionChangedCallback`) is registered. This function is automatically called whenever the master sends a command to change a function.
-   In the `loop()` function, `susi.available()` continuously checks for new data from the master. When a function command is received, the library triggers the call to the callback function.
-   The serial monitor shows which function was turned on or off.

This example serves as a foundation for your own SUSI modules that need to react to commands from a central unit or decoder.
