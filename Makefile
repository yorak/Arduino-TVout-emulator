# Compiler and flags
CXX = g++
# Common flags
COMMON_FLAGS = -Wall -Wextra -Wno-narrowing -std=c++17 -fpermissive -Wno-parentheses `sdl2-config --cflags`
# Release flags
CXXFLAGS = $(COMMON_FLAGS) -O2
# Debug flags
DEBUG_FLAGS = $(COMMON_FLAGS) -g -O0 -DDEBUG
LDFLAGS = `sdl2-config --libs`

# Target executable
TARGET = tvout_simulator
DEBUG_TARGET = $(TARGET)_debug

# Arduino-style app file
ARDUINO_APP = demo
COMMON_FLAGS += -include ArduinoEmulation.h

# Source files
CORE_SRCS = TVout.cpp SDLArduino.cpp sdl2_video_gen.cpp ArduinoTVoutMain.cpp
SRCS = $(CORE_SRCS) $(ARDUINO_APP).cpp
OBJS = $(SRCS:.cpp=.o)
DEBUG_OBJS = $(SRCS:.cpp=.debug.o)

# Build rules
all: $(TARGET)

debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Main depends on the generated header
ArduinoMain.o: ArduinoMain.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

ArduinoMain.debug.o: ArduinoMain.cpp
	$(CXX) $(DEBUG_FLAGS) -c $< -o $@

# Other object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.debug.o: %.cpp
	$(CXX) $(DEBUG_FLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(DEBUG_OBJS) $(TARGET) $(DEBUG_TARGET)

# Phony targets
.PHONY: all debug clean