#ifndef MOCK_EEPROM_H
#define MOCK_EEPROM_H

#include <cstdint>
#include <vector>
#include <cstring> // For memcpy

class EEPROMClass {
public:
    EEPROMClass() : _data(512, 0xFF) {} // Simulate 512 bytes of EEPROM, initialized to 0xFF

    uint8_t read(int address) {
        return _data[address];
    }

    void write(int address, uint8_t value) {
        _data[address] = value;
    }

    // Overload for different data types
    template<typename T>
    const T& put(int address, const T& value) {
        const uint8_t* ptr = (const uint8_t*)&value;
        for (size_t i = 0; i < sizeof(T); ++i) {
            _data[address + i] = ptr[i];
        }
        return value;
    }

    template<typename T>
    T& get(int address, T& value) {
        uint8_t* ptr = (uint8_t*)&value;
        for (size_t i = 0; i < sizeof(T); ++i) {
            ptr[i] = _data[address + i];
        }
        return value;
    }

private:
    std::vector<uint8_t> _data;
};

extern EEPROMClass EEPROM;

#endif // MOCK_EEPROM_H
