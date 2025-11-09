# NMRA SUSI Arduino Library Development Roadmap

This document outlines the development plan for the Arduino NMRA SUSI library, structured according to the official RailCommunity specifications.

## 1. Project Setup & Documentation

- [x] Create the `docs` directory and confirm all downloaded NMRA/RailCommunity specifications are present.
- [x] Create a `README.md` file with a brief project overview.
- [x] Create the main deliverable: `ROADMAP.md`.
- [x] Set up a basic Arduino library structure: create a `src` directory and a `library.properties` file.

## 2. Core Protocol Implementation (RCN-600)

### Master (Host) Role

- [x] **[M1]** Define a hardware abstraction layer (HAL) for pin control (Clock, Data).
- [x] **[RCN600-M1]** Implement precise microsecond-level timing for the SUSI clock signal (≥10µs high/low, ≤500µs total period).
- [x] **[RCN600-M2]** Implement timing and synchronization logic:
    - [x] Ensure bytes of a multi-byte command are sent within 7ms of each other.
    - [x] Ensure a pause of at least 9ms before a new command if the previous one ended more than 7ms ago.
    - [x] Insert a synchronization gap of at least 9ms after every 20 commands.
- [x] **[RCN600-M3]** Implement Acknowledge (ACK) handling:
    - [x] For commands requiring a response, listen for an ACK pulse.
    - [x] Correctly handle ACK timing (accept pulses from 0.5ms to 7ms).
    - [x] Implement a 20ms timeout for aborting a command if no ACK is received.
- [ ] **[M3]** Create data structures/classes to represent SUSI packets.
- [ ] **[M4]** Write a low-level function to transmit a single SUSI packet bit-by-bit.
- [ ] **[RCN600-M4]** Implement helper functions to encode and send standard SUSI commands as defined in RCN-600.
- [ ] **[M6]** Create a high-level API for sending commands to a specific slave address.
- [ ] **[M9]** Implement state management for the master (e.g., tracking last command, waiting for response).
- [ ] **[M14]** Add error handling for timeouts or invalid responses from a slave.

### Slave (Module) Role

- [ ] **[S1]** Define a HAL for the slave's clock and data pins.
- [ ] **[S2]** Implement an interrupt-driven routine to detect the falling edge of the SUSI clock.
- [ ] **[S3]** Inside the interrupt, read the data bit from the data line.
- [ ] **[S4]** Assemble the incoming bits into a complete SUSI packet in a buffer.
- [ ] **[S5]** Validate the received packet (e.g., start/stop bits).
- [ ] **[RCN600-S1]** Implement protocol timing and synchronization logic:
    - [ ] Implement an 8ms timeout to reset the packet buffer if a byte is incomplete, preventing synchronization loss.
- [ ] **[RCN600-S2]** Implement Acknowledge (ACK) generation:
    - [ ] For commands requiring a response, generate a 1-2ms ACK pulse on the data line.
- [ ] **[S6]** Check if the packet's address matches the slave's configured address.
- [ ] **[S7]** Parse the command and data bytes from the validated packet.
- [ ] **[S8]** Implement a state machine to process received commands.
- [ ] **[S12]** Implement logic to manage the state of functions (e.g., lights).
- [ ] **[S17]** Create a callback system for the user's sketch to be notified of events (e.g., `onFunctionChange`).


## 3. Bidirectional Communication (RCN-601)

### Master (Host) Role

- [ ] **[RCN601-M1]** Implement the BiDi handshake to detect and register BiDi-capable modules at startup.
- [ ] **[RCN601-M2]** Implement the periodic BiDi call (at least every 100ms) to poll registered modules for data.
- [ ] **[RCN601-M3]** After sending a BiDi call, implement the 4-5ms response window for the module to provide an ACK.
- [ ] **[RCN601-M4]** Upon receiving an ACK, generate 32 clock pulses to read the 4-byte BiDi message from the module.
- [ ] **[M8]** Create a mechanism to listen for and parse responses from slave modules.
- [ ] **[RCN601-M5]** Implement the CV Bank Reading procedure:
    - [ ] Send the CV Bank Read command (0x0C, 0x0D, or 0x0E).
    - [ ] Wait for ACK.
    - [ ] Clock in the 40 bytes of the CV bank.
    - [ ] Clock in the 2-byte CRC checksum.
    - [ ] Implement CRC checksum validation.

### Slave (Module) Role

- [ ] **[RCN601-S1]** Implement the BiDi handshake response to register with the Host.
- [ ] **[RCN601-S2]** Upon receiving a BiDi call, send an ACK within 2ms if data is available to transmit.
- [ ] **[S13]** Create a function to generate a response packet for bidirectional communication.
- [ ] **[RCN601-S3]** After sending an ACK, place the 4-byte BiDi message on the data line, synchronized with the Host's 32 clock pulses.
- [ ] **[RCN601-S4]** Implement the CV Bank Reading response:
    - [ ] On receiving a CV Bank Read command, send an ACK if the bank is available.
    - [ ] Transmit the 40 bytes of the requested CV bank.
    - [ ] Calculate and transmit the CRC checksum.
- [ ] **[RCN601-S5]** Implement logic to generate all relevant BiDi messages.


## 4. Configuration Variables (RCN-602)

### Master (Host) Role

- [ ] **[RCN602-M1]** Implement functions to read and write CVs from specific modules.
- [ ] **[RCN602-M2]** Implement support for CV banking using CV 1021.

### Slave (Module) Role

- [ ] **[S9]** Implement a mechanism to store and retrieve CV values (e.g., in an array or EEPROM).
- [ ] **[S10]** Implement the logic to handle a `Write CV` command.
- [ ] **[S11]** Implement the logic to handle a `Read CV` command.
- [ ] **[RCN602-S2]** Implement support for CV banking using CV 1021.
- [ ] **[RCN602-S3]** Implement the specific CVs defined in RCN-602:
    - [ ] CV 897: SUSI Module #
    - [ ] CV 900 / 940 / 980: Manufacturer and Hardware ID
    - [ ] CV 901 / 941 / 981: Version Numbers
    - [ ] CV 1020: Status Bits (WAIT, SLOW, HOLD, STOP)


## 5. Examples and Finalization

- [ ] Create a "Master" example sketch demonstrating key features.
- [ ] Create a "Slave" example sketch demonstrating key features.
- [ ] Write comprehensive documentation for the library's public API.
- [ ] Add a "Getting Started" guide to the main `README.md`.
