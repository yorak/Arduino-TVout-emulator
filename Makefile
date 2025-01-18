# Compiler and flags
CXX = g++

# Directory structure
BIN_DIR = bin

# Common flags
COMMON_FLAGS = -Wall -Wextra -Wno-narrowing -std=c++17 -fpermissive -Wno-parentheses `sdl2-config --cflags`
# Release flags
CXXFLAGS = $(COMMON_FLAGS) -O2
# Debug flags
DEBUG_FLAGS = $(COMMON_FLAGS) -g -O0 -DDEBUG
LDFLAGS = `sdl2-config --libs` -lpthread

# Target executable
TARGET = $(BIN_DIR)/tvout_simulator
DEBUG_TARGET = $(BIN_DIR)/$(notdir $(TARGET))_debug

# Arduino-style app file
ARDUINO_APP = min_demo
COMMON_FLAGS += -include SDLArduino.h

# Source files
CORE_SRCS = TVout.cpp SDLArduino.cpp sdl2_video_gen.cpp ArduinoTVoutMain.cpp
SRCS = $(CORE_SRCS) $(ARDUINO_APP).cpp
OBJS = $(SRCS:%.cpp=$(BIN_DIR)/%.o)
DEBUG_OBJS = $(SRCS:%.cpp=$(BIN_DIR)/%.debug.o)

# Build rules
all: $(BIN_DIR) $(TARGET)

debug: CXXFLAGS = $(DEBUG_FLAGS)
debug: $(BIN_DIR) $(DEBUG_TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

$(DEBUG_TARGET): $(DEBUG_OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Create bin directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Main depends on the generated header
$(BIN_DIR)/ArduinoMain.o: ArduinoMain.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/ArduinoMain.debug.o: ArduinoMain.cpp
	$(CXX) $(DEBUG_FLAGS) -c $< -o $@

# Other object files
$(BIN_DIR)/%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BIN_DIR)/%.debug.o: %.cpp
	$(CXX) $(DEBUG_FLAGS) -c $< -o $@

# Clean rule
clean:
	rm -rf $(BIN_DIR)

# Phony targets
.PHONY: all debug clean