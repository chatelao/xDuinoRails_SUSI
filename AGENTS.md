# Agent Instructions

This document provides instructions for AI agents working on this repository.

## RCN Specification Mapping

This section maps the implemented functions and test cases to the relevant chapters in the RailCommunity specifications.

### Function to RCN Mapping

| Function | File | RCN Document & Chapter |
|---|---|---|
| `generate_clock_pulse` | `susi_hal.h` | RCN-600, Chapter 2.1 (Timing) |
| `waitForAck` | `susi_hal.h` | RCN-600, Chapter 2.3 (Acknowledge) |
| `sendAckPulse` | `susi_hal.h` | RCN-600, Chapter 2.3 (Acknowledge) |
| `SUSI_Master::sendPacket` | `susi_master.h` | RCN-600, Chapter 2.2 (Data Transfer) |
| `SUSI_Master_API::setFunction`| `susi_master.h` | RCN-600, Chapter 3.2 (Function Control) |
| `SUSI_Master_API::setSpeed` | `susi_master.h` | RCN-600, Chapter 3.1 (Speed Control) |
| `SUSI_Master_API::writeCV` | `susi_master.h` | RCN-602, Chapter 2.2 (Write CV) |
| `SUSI_Master_API::readCV` | `susi_master.h` | RCN-602, Chapter 2.1 (Read CV) |
| `SUSI_Slave::read` | `susi_slave.h` | RCN-600, Chapter 2.2 (Data Transfer) |
| `SUSI_Slave::getSpeed` | `susi_slave.h` | RCN-600, Chapter 3.1 (Speed Control) |
| `SUSI_Slave::getDirection` | `susi_slave.h` | RCN-600, Chapter 3.1 (Speed Control) |
| `SUSI_Slave::getFunction` | `susi_slave.h` | RCN-600, Chapter 3.2 (Function Control) |
| `SUSI_Slave::readCV` | `susi_slave.h` | RCN-602, Chapter 2.1 (Read CV) |

### Test Case to RCN Mapping

| Test Case | File | RCN Document & Chapter |
|---|---|---|
| `SusiHALTest.WaitForAck_Timeout` | `test/unit_tests.cpp` | RCN-600, Chapter 2.3 (Acknowledge) |
| `SusiHALTest.WaitForAck_Success` | `test/unit_tests.cpp` | RCN-600, Chapter 2.3 (Acknowledge) |
| `SusiHALTest.WaitForAck_TooShort` | `test/unit_tests.cpp` | RCN-600, Chapter 2.3 (Acknowledge) |
| `SusiHALTest.WaitForAck_TooLong` | `test/unit_tests.cpp` | RCN-600, Chapter 2.3 (Acknowledge) |
| `SUSIMasterAPITest.SetFunction` | `test/unit_tests.cpp` | RCN-600, Chapter 3.2 (Function Control) |
| `SUSIMasterAPITest.SetSpeed` | `test/unit_tests.cpp` | RCN-600, Chapter 3.1 (Speed Control) |
| `SUSIMasterAPITest.WriteCV` | `test/unit_tests.cpp` | RCN-602, Chapter 2.2 (Write CV) |
| `SUSIMasterAPITest.GetFunction` | `test/unit_tests.cpp` | RCN-600, Chapter 3.2 (Function Control) |
| `SUSIMasterAPITest.ReadCV` | `test/unit_tests.cpp` | RCN-602, Chapter 2.1 (Read CV) |
| `SUSISlaveTest.SetSpeedPacketReception`| `test/unit_tests.cpp` | RCN-600, Chapter 3.1 (Speed Control) |
| `SUSISlaveTest.PacketTimeout` | `test/unit_tests.cpp` | RCN-600, Chapter 2.1 (Timing) |
| `SUSISlaveTest.InvalidStopBit` | `test/unit_tests.cpp` | RCN-600, Chapter 2.2 (Data Transfer) |
| `SUSISlaveTest.FunctionCallback` | `test/unit_tests.cpp` | RCN-600, Chapter 3.2 (Function Control) |
| `EndToEndTest.TrueEndToEnd_SendAndReceiveSetSpeedPacket` | `test/e2e_tests.cpp` | RCN-600, Chapter 3.1 (Speed Control) |
