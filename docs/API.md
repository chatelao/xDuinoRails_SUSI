# NMRA SUSI Arduino Library API Reference

This document provides a reference for the public API of the NMRA SUSI Arduino library.

## SUSI_Master_API Class

The `SUSI_Master_API` class provides a high-level API for interacting with SUSI slave devices.

### `SUSI_Master_API(SUSI_Master& master)`

Constructor for the `SUSI_Master_API` class.

- `master`: A reference to a `SUSI_Master` object.

### `void begin()`

Initializes the SUSI master API. This should be called in your `setup()` function.

### `void reset()`

Resets the state of the SUSI master API.

### `SusiMasterResult setFunction(uint8_t address, uint8_t function, bool on)`

Sets a function on a SUSI slave device.

- `address`: The address of the slave module (1-255).
- `function`: The function number to set (0-31).
- `on`: `true` to turn the function on, `false` to turn it off.
- `return`: A `SusiMasterResult` code indicating the status of the operation.

### `bool getFunction(uint8_t address, uint8_t function)`

Gets the state of a function on a SUSI slave device.

- `address`: The address of the slave module (1-255).
- `function`: The function number to get (0-31).
- `return`: The state of the function (`true` for on, `false` for off).

### `SusiMasterResult setSpeed(uint8_t address, uint8_t speed, bool forward)`

Sets the speed of a SUSI slave device.

- `address`: The address of the slave module (1-255).
- `speed`: The speed to set (0-127).
- `forward`: The direction of travel (`true` for forward, `false` for reverse).
- `return`: A `SusiMasterResult` code indicating the status of the operation.

### `SusiMasterResult writeCV(uint8_t address, uint16_t cv, uint8_t value)`

Writes a value to a Configuration Variable (CV) on a SUSI slave module.

- `address`: The address of the slave module (1-255).
- `cv`: The CV number to write to (1-1024).
- `value`: The value to write to the CV (0-255).
- `return`: A `SusiMasterResult` code indicating the status of the operation.

### `SusiMasterResult readCV(uint8_t address, uint16_t cv, uint8_t& value)`

Reads a value from a Configuration Variable (CV) on a SUSI slave module.

- `address`: The address of the slave module (1-255).
- `cv`: The CV number to read from (1-1024).
- `value`: A reference to a byte to store the value in.
- `return`: A `SusiMasterResult` code indicating the status of the operation.

### `SusiMasterResult enableBidirectionalMode(uint8_t address)`

Enables bidirectional communication with a SUSI slave device.

- `address`: The address of the slave module (1-255).
- `return`: A `SusiMasterResult` code indicating the status of the operation.

### `bool getUniqueId(uint8_t address, uint32_t& unique_id)`

Gets the unique ID of a SUSI slave device.

- `address`: The address of the slave module (1-255).
- `unique_id`: A reference to a 32-bit integer to store the unique ID in.
- `return`: `true` if the unique ID was successfully retrieved, `false` otherwise.

## SUSI_Slave Class

The `SUSI_Slave` class is used to create a SUSI slave module that can be controlled by a SUSI master.

### `SUSI_Slave(uint8_t clockPin, uint8_t dataPin, uint32_t unique_id)`

Constructor for the `SUSI_Slave` class.

- `clockPin`: The Arduino pin connected to the SUSI CLOCK line.
- `dataPin`: The Arduino pin connected to the SUSI DATA line.
- `unique_id`: The unique ID of the slave.

### `void begin(uint8_t address)`

Initializes the SUSI slave. This should be called in your `setup()` function.

- `address`: The address of this slave module (1-255).

### `bool available()`

Checks if a SUSI packet is available to be read.

- `return`: `true` if a packet is available, `false` otherwise.

### `SUSI_Packet read()`

Reads a SUSI packet from the slave.

- `return`: The `SUSI_Packet` that was read.

### `void onFunctionChange(FunctionCallback callback)`

Sets a callback function that is called when a function is changed.

- `callback`: The callback function.

### `uint8_t getSpeed() const`

Gets the current speed of the slave.

- `return`: The current speed.

### `bool getDirection() const`

Gets the current direction of the slave.

- `return`: The current direction (`true` for forward, `false` for reverse).

### `bool getFunction(uint8_t function) const`

Gets the state of a function.

- `function`: The function to get the state of.
- `return`: The state of the function (`true` for on, `false` for off).

### `uint8_t readCV(uint16_t cv)`

Reads the value of a CV.

- `cv`: The CV to read.
- `return`: The value of the CV.
