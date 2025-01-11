# Compiler and flags
CXX = g++
CXXFLAGS = -Wall -Wextra -Wno-narrowing -std=c++17 `sdl2-config --cflags`
LDFLAGS = `sdl2-config --libs`

# Target executable
TARGET = tvout_simulator

# Arduino-style app file (change this to your app file name without extension)
ARDUINO_APP = demo

# Source files
CORE_SRCS = TVout.cpp SDLArduino.cpp ArduinoMain.cpp
SRCS = $(CORE_SRCS) $(ARDUINO_APP).cpp
OBJS = $(SRCS:.cpp=.o)

# Auto-generated header for ARDUINO_APP
GENERATED_HEADER = ArduinoGenerated.h

# Build rules
all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS)

# Rule to generate header from app file
$(GENERATED_HEADER): $(ARDUINO_APP).cpp
	@echo "#ifndef ARDUINO_GENERATED_H" > $@
	@echo "#define ARDUINO_GENERATED_H" >> $@
	@echo "" >> $@
	@grep -h "^[[:space:]]*void[[:space:]]*setup\|^[[:space:]]*void[[:space:]]*loop" $< | grep -v "{" | sed 's/{.*/;/' >> $@
	@echo "" >> $@
	@echo "#endif // ARDUINO_GENERATED_H" >> $@

# Main depends on the generated header
ArduinoMain.o: ArduinoMain.cpp $(GENERATED_HEADER)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Other object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean rule
clean:
	rm -f $(OBJS) $(TARGET) $(GENERATED_HEADER)

# Phony targets
.PHONY: all clean