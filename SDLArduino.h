#ifndef ARDUINO_SIMULATION_H
#define ARDUINO_SIMULATION_H

#include <stdint.h>
#include <cstdlib> // For std::rand
#include <cmath> 

// SDL2 specific includes
#include <SDL2/SDL.h>
#include <SDL2/SDL_mutex.h>

// TODO: Remove me
#include <cstdio>
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


// This is what makes the GPIO change state when 
//  keyboard buttons are pressed,
//  controller is twiddled, etc. 
void processSDLInputEvent(SDL_Event& event);
// This is used to signal shutdown. Delay is no longer waited.
void signalSDLwindowClosed();

// Pin mapping (really make no sense here, but this makes it compile if somebody use these)
#define digitalPinToPort(pin)    (pin / 8) // Example mapping
#define digitalPinToBitMask(pin) (1 << (pin % 8))
#define analogPinToChannel(pin) (pin)

// No PROGMEM on PC, just read if from regular mem
//#define pgm_read_byte(x) (*(x))
#define pgm_read_byte(x) (*(const uint8_t*)(x))

#ifdef __cplusplus
extern "C" {
#endif

// Function prototypes
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t value);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);
void analogWrite(uint8_t pin, int value);
void delay(unsigned long ms);
#ifdef __cplusplus
}
#endif

// Arduino kind of random
void randomSeed(unsigned int seed);
long arduinoRandom(long min, long max);
long arduinoRandom(long max);

void setup();
void loop();

#endif // ARDUINO_SIMULATION_H
