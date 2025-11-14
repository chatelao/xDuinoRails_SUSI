# Agent Instructions

This document provides instructions for AI agents working on this repository.

## Documentation

All code must be documented using Doxygen-style comments. Where applicable, comments should reference the specific RCN document and chapter that the code implements.

## RCN Specification Mapping

This section maps the implemented test cases to the relevant chapters in the RailCommunity specifications.

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
| `LegacySusiE2ETest.readCV_BiDi` | `test/test_susi_e2e.cpp` | RCN-601, Chapter 5 (BiDi Messages) |
| `LegacySusiE2ETest.readSpecialCVs` | `test/test_susi_e2e.cpp` | RCN-602, Chapter 3 (Configuration Variables) |
