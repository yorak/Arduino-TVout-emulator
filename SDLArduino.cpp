
#include <SDL2/SDL.h>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#else
#include <time.h>
#endif


// SDL2 and Input Handling
static std::unordered_map<uint8_t, uint8_t> digitalPinState;
static std::unordered_map<uint8_t, int> analogPinState;
static SDL_GameController *gameController = nullptr;
static bool gamepadInitialized = false;

// Keyboard mappings
static const SDL_Scancode digitalKeys[] = {
    SDL_SCANCODE_1, SDL_SCANCODE_2, SDL_SCANCODE_3, SDL_SCANCODE_4,
    SDL_SCANCODE_5, SDL_SCANCODE_6, SDL_SCANCODE_7, SDL_SCANCODE_8,
    SDL_SCANCODE_9, SDL_SCANCODE_0
};

static const SDL_Scancode analogKeys[] = {
    SDL_SCANCODE_Q, SDL_SCANCODE_W, SDL_SCANCODE_E, SDL_SCANCODE_R,
    SDL_SCANCODE_T, SDL_SCANCODE_Y, SDL_SCANCODE_U, SDL_SCANCODE_I,
    SDL_SCANCODE_O, SDL_SCANCODE_P
};

// Gamepad mappings
#define LEFT_PIN 2
#define RIGHT_PIN 3
#define FIRE_PIN 4

void processSDLInputEvent(SDL_Event& event) {
    switch (event.type) {
        case SDL_KEYDOWN:
        case SDL_KEYUP: {
            bool isPressed = (event.type == SDL_KEYDOWN);
            for (uint8_t i = 0; i < sizeof(digitalKeys) / sizeof(digitalKeys[0]); ++i) {
                if (event.key.keysym.scancode == digitalKeys[i]) {
                    digitalPinState[i] = isPressed ? HIGH : LOW;
                }
            }
            for (uint8_t i = 0; i < sizeof(analogKeys) / sizeof(analogKeys[0]); ++i) {
                if (event.key.keysym.scancode == analogKeys[i]) {
                    analogPinState[i] = isPressed ? 1023 : 0; // Simulate analog range
                }
            }
            break;
        }
        case SDL_CONTROLLERDEVICEADDED:
            if (!gamepadInitialized && SDL_IsGameController(event.cdevice.which)) {
                gameController = SDL_GameControllerOpen(event.cdevice.which);
                gamepadInitialized = (gameController != nullptr);
            }
            break;
        case SDL_CONTROLLERDEVICEREMOVED:
            if (gameController && event.cdevice.which == SDL_JoystickInstanceID(SDL_GameControllerGetJoystick(gameController))) {
                SDL_GameControllerClose(gameController);
                gameController = nullptr;
                gamepadInitialized = false;
            }
            break;
        case SDL_CONTROLLERBUTTONDOWN:
        case SDL_CONTROLLERBUTTONUP: {
            bool isPressed = (event.type == SDL_CONTROLLERBUTTONDOWN);
            if (gameController) {
                if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_LEFT) {
                    digitalPinState[LEFT_PIN] = isPressed ? HIGH : LOW;
                } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_DPAD_RIGHT) {
                    digitalPinState[RIGHT_PIN] = isPressed ? HIGH : LOW;
                } else if (event.cbutton.button == SDL_CONTROLLER_BUTTON_A) { // Button A as FIRE
                    digitalPinState[FIRE_PIN] = isPressed ? HIGH : LOW;
                }
            }
            break;
        }
    }
}

void pinMode(uint8_t /*pin*/, uint8_t /*mode*/) {
    // No specific action required for SDL input emulation
}

void digitalWrite(uint8_t /*pin*/, uint8_t /*value*/) {
    // SDL handles only input; output is not simulated
}

int digitalRead(uint8_t pin) {
    return digitalPinState[pin];
}

int analogRead(uint8_t pin) {
    return analogPinState[pin];
}

void analogWrite(uint8_t /*pin*/, int /*value*/) {
    // SDL handles only input; output is not simulated
}

void randomSeed(unsigned int seed) {
    srand(seed);
}

long arduinoRandom(long min, long max) {
    if (min >= max) return min;
    return min + rand() % (max - min);
}

long arduinoRandom(long max) {
    return arduinoRandom(0, max);
}

void delay(unsigned long ms) {
#ifdef _WIN32
    Sleep(ms);  // Sleep for `ms` milliseconds on Windows
#else
    struct timespec ts;
    ts.tv_sec = ms / 1000;              // Convert milliseconds to seconds
    ts.tv_nsec = (ms % 1000) * 1000000; // Convert remainder to nanoseconds
    nanosleep(&ts, NULL);               // Sleep for the specified time
#endif
}

