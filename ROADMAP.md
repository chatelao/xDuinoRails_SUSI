# NMRA SUSI Arduino Library Development Roadmap

This document outlines the development plan for the Arduino NMRA SUSI library.

## 1. Project Setup & Documentation

- [x] Create the `docs` directory and confirm all downloaded NMRA/RailCommunity specifications are present.
- [x] Create a `README.md` file with a brief project overview.
- [x] Create the main deliverable: `ROADMAP.md`. This file will contain the detailed checklists for the Master and Slave implementations.
- [ ] Set up a basic Arduino library structure: create a `src` directory and a `library.properties` file.

## 2. Master (Decoder) Role Implementation

- [x] **[M1]** Define a hardware abstraction layer (HAL) for pin control (Clock, Data).
- [ ] **[M2]** Implement precise microsecond-level timing functions for the SUSI clock signal.
- [ ] **[M3]** Create data structures/classes to represent SUSI packets.
- [ ] **[M4]** Write a low-level function to transmit a single SUSI packet bit-by-bit.
- [ ] **[M5]** Implement helper functions to encode standard SUSI commands (e.g., Set Function, Set CV).
- [ ] **[M6]** Create a high-level API for sending commands to a specific slave address.
- [ ] **[M7]** Implement a low-level function to read data from the data line for bidirectional communication.
- [ ] **[M8]** Create a mechanism to listen for and parse responses from slave modules.
- [ ] **[M9]** Implement state management for the master (e.g., tracking last command, waiting for response).
- [ ] **[M10]** Implement the specific command sequence for writing to a slave's Configuration Variable (CV).
- [ ] **[M11]** Implement the specific command sequence for reading a slave's CV.
- [ ] **[M12]** Implement the initial handshake to determine if a slave supports bidirectional communication (RCN-601).
- [ ] **[M13]** Implement the protocol for reading larger data blocks from a bidirectional slave (RCN-601).
- [ ] **[M14]** Add error handling for timeouts or invalid responses from a slave.

## 3. Slave (Module) Role Implementation

- [ ] **[S1]** Define a HAL for the slave's clock and data pins.
- [ ] **[S2]** Implement an interrupt-driven routine to detect the falling edge of the SUSI clock.
- [ ] **[S3]** Inside the interrupt, read the data bit from the data line.
- [ ] **[S4]** Assemble the incoming bits into a complete SUSI packet in a buffer.
- [ ] **[S5]** Validate the received packet (e.g., start/stop bits).
- [ ] **[S6]** Check if the packet's address matches the slave's configured address.
- [ ] **[S7]** Parse the command and data bytes from the validated packet.
- [ ] **[S8]** Implement a state machine to process received commands.
- [ ] **[S9]** Implement a mechanism to store and retrieve CV values (e.g., in an array or EEPROM).
- [ ] **[S10]** Implement the logic to handle a `Write CV` command.
- [ ] **[S11]** Implement the logic to handle a `Read CV` command.
- [ ] **[S12]** Implement logic to manage the state of functions (e.g., lights).
- [ ] **[S13]** Create a function to generate a response packet for bidirectional communication.
- [ ] **[S14]** Implement the logic to transmit the response packet back to the master (RCN-601).
- [ ] **[S15]** Implement the slave's side of the bidirectional handshake (RCN-601).
- [ ] **[S16]** Implement the slave's logic for sending larger data blocks to the master (RCN-601).
- [ ] **[S17]** Create a callback system for the user's sketch to be notified of events (e.g., `onFunctionChange`).

## 4. Examples and Finalization

- [ ] Create a "Master" example sketch demonstrating key features.
- [ ] Create a "Slave" example sketch demonstrating key features.
- [ ] Write comprehensive documentation for the library's public API.
- [ ] Add a "Getting Started" guide to the main `README.md`.
