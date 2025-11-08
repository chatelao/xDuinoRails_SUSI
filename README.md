# Arduino NMRA SUSI Library

This repository contains an Arduino library for the NMRA/RailCommunity SUSI (Serial User Standard Interface) protocol. The library will support both Master (decoder) and Slave (module) roles, as well as the bidirectional communication extension (RCN-601).

The development roadmap is outlined in the `ROADMAP.md` file.

## Getting Started

To use this library, you will need an Arduino board and a basic understanding of the SUSI protocol.

### Master (Decoder) Role

To use the library as a SUSI master, you will need to:

1.  Include the `SUSI_Master.h` header file.
2.  Create a `SUSI_Master` object, passing the clock and data pins to the constructor.
3.  Call the `begin()` method in your `setup()` function.
4.  Use the `setFunction()` and `writeCV()` methods to send commands to your SUSI slave modules.

See the `examples/Master/Master.ino` sketch for a complete example.

### Slave (Module) Role

To use the library as a SUSI slave, you will need to:

1.  Include the `SUSI_Slave.h` header file.
2.  Create a `SUSI_Slave` object, passing the clock pin, data pin, and slave address to the constructor.
3.  Call the `begin()` method in your `setup()` function.
4.  Call the `loop()` method repeatedly in your main `loop()` function to process incoming commands.

See the `examples/Slave/Slave.ino` sketch for a complete example.
