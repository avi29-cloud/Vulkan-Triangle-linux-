# Compiler settings
CXX = g++
CXXFLAGS = -std=c++17 -O2

# Linker flags provided
LDFLAGS = -lglfw -lvulkan -ldl -lpthread -lX11 -lXxf86vm -lXrandr -lXi

# Project files
SRC = main.cpp
TARGET = VulkanTest

# Default target
all: $(TARGET)

# Build rules
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Phony targets
.PHONY: test clean

# Run the program
test: $(TARGET)
	./$(TARGET)

# Clean up build files
clean:
	rm -f $(TARGET)