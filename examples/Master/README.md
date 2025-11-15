# SUSI-Master Example

This example demonstrates how to use the `SUSI.h` library to implement a **SUSI Master** (transmitter).

## Use Case

The SUSI Master is the primary controller on the bus. It sends commands to one or more connected SUSI Slaves. This example repeatedly turns function 1 on and off on a target slave.

## Hardware Setup

1.  **Arduino as Master**:
    *   Connect Pin 2 of the Arduino to the **Clock line** of the SUSI bus.
    *   Connect Pin 3 of the Arduino to the **Data line** of the SUSI bus.
    *   Connect the GND of the Arduino to the GND of the Slave.

2.  **Arduino as Slave**:
    *   Upload the `Slave` example to a second Arduino.
    *   Connect the Clock, Data, and GND lines in parallel with the Master Arduino.

## How it Works

-   The code initializes the SUSI Master.
-   In the `loop()` function, the state of function 1 is changed every two seconds (from ON to OFF and vice versa).
-   The `susi.setFunction()` command is used to send the new state to the slave with address 1.
-   The serial monitor outputs which command is currently being sent and whether the transmission was successful.

This example serves as a simple template for building more complex control logic for SUSI-compatible modules.
