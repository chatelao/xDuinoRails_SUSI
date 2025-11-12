# Arduino NMRA SUSI Library

This repository contains an Arduino library for the NMRA/RailCommunity SUSI (Serial User Standard Interface) protocol. The library supports both Master (decoder) and Slave (module) roles, as well as the bidirectional communication extension (RCN-601).

The development roadmap is outlined in the `ROADMAP.md` file.

## Getting Started

To use this library, you will need an Arduino board and a basic understanding of the SUSI protocol.

### Master (Decoder) Role

To use the library as a SUSI master, you will need to:

1.  Include the `susi_master.h` header file.
2.  Create a `SusiHAL` object, passing the clock and data pins to the constructor.
3.  Create a `SUSI_Master` object, passing the `SusiHAL` object to the constructor.
4.  Create a `SUSI_Master_API` object, passing the `SUSI_Master` object to the constructor.
5.  Call the `begin()` method on your `SUSI_Master_API` object in your `setup()` function.
6.  Use the methods of the `SUSI_Master_API` object to send commands to your SUSI slave modules.

See the `examples/Master/Master.ino` sketch for a complete example.

### Slave (Module) Role

To use the library as a SUSI slave, you will need to:

1.  Include the `susi_slave.h` header file.
2.  Create a `SUSI_Slave` object, passing the clock pin, data pin, and a unique ID to the constructor.
3.  Call the `begin()` method in your `setup()` function, passing the slave address.
4.  Call the `available()` method repeatedly in your main `loop()` function to check for incoming packets.
5.  If a packet is available, call the `read()` method to process it.

See the `examples/Slave/Slave.ino` sketch for a complete example.
