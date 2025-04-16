# Makefile for Main C++ program

# Compiler to use
CXX = g++

# Compiler flags
# -Wall: Enable all warnings
# -g:  Include debugging information
CXXFLAGS = -g -std=c++20 -I${ICU_ROOT}/source/common -I${ICU_ROOT}/source/i18n
LDFLAGS = -L${ICU_ROOT}/source/lib -licui18n -licuuc -licudata

# Source file
SRC = main.cpp

# Output executable name
TARGET = main

# Default rule:  Build the executable
all: $(TARGET)

# Rule to compile the source file into an object file
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC) $(LDFLAGS)

# Clean rule:  Remove the executable and any object files
clean:
	rm -f $(TARGET)

