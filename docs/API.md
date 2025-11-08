# NMRA SUSI Arduino Library API Reference

This document provides a reference for the public API of the NMRA SUSI Arduino library.

## SUSI_Master Class

The `SUSI_Master` class is used to control SUSI slave modules from a master device (e.g., a DCC decoder).

### `SUSI_Master(uint8_t clockPin, uint8_t dataPin)`

Constructor for the `SUSI_Master` class.

- `clockPin`: The Arduino pin connected to the SUSI CLOCK line.
- `dataPin`: The Arduino pin connected to the SUSI DATA line.

### `void begin()`

Initializes the SUSI master. This should be called in your `setup()` function.

### `void setFunction(uint8_t address, uint8_t function, bool on)`

Sets a function on a SUSI slave module.

- `address`: The address of the slave module (1-255).
- `function`: The function number to set (0-31).
- `on`: `true` to turn the function on, `false` to turn it off.

### `void writeCV(uint8_t address, uint16_t cv, uint8_t value)`

Writes a value to a Configuration Variable (CV) on a SUSI slave module.

- `address`: The address of the slave module (1-255).
- `cv`: The CV number to write to (1-1024).
- `value`: The value to write to the CV (0-255).

## SUSI_Slave Class

The `SUSI_Slave` class is used to create a SUSI slave module that can be controlled by a SUSI master.

### `SUSI_Slave(uint8_t clockPin, uint8_t dataPin, uint8_t address)`

Constructor for the `SUSI_Slave` class.

- `clockPin`: The Arduino pin connected to the SUSI CLOCK line.
- `dataPin`: The Arduino pin connected to the SUSI DATA line.
- `address`: The address of this slave module (1-255).

### `void begin()`

Initializes the SUSI slave. This should be called in your `setup()` function.

### `void loop()`

This function should be called repeatedly in your main `loop()` function. It processes incoming SUSI packets.
