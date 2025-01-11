#ifndef ARDUINO_IO_EMULATION_H
#define ARDUINO_IO_EMULATION_H

#include <stdint.h>
#include <cstdlib> // For std::rand
#include <cmath> 
#undef random

// Typical macros and constants
#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW 0
#endif

#ifndef INPUT
#define INPUT 0
#endif

#ifndef OUTPUT
#define OUTPUT 1
#endif

#ifndef INPUT_PULLUP
#define INPUT_PULLUP 2
#endif

// Pin definitions (Adjust as per your platform)
#ifndef NUM_DIGITAL_PINS
#define NUM_DIGITAL_PINS 20 // Example for a generic platform
#endif

#ifndef NUM_ANALOG_INPUTS
#define NUM_ANALOG_INPUTS 6 // Example for a generic platform
#endif

// Pin mapping (override as needed)
#define digitalPinToPort(pin)    (pin / 8) // Example mapping
#define digitalPinToBitMask(pin) (1 << (pin % 8))
#define analogPinToChannel(pin) (pin)

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int value);
unsigned char pgm_read_byte(const unsigned char* data);

#ifdef __cplusplus
}
#endif



// Arduino kind of random
void randomSeed(unsigned int seed);
long arduinoRandom(long min, long max);
long arduinoRandom(long max);

#endif // ARDUINO_IO_EMULATION_H
