# Compiler to use
CXX = g++

# Compiler flags:
# -std=c++17: Use the C++17 standard
# -Wall: Enable all standard warnings
# -Wextra: Enable extra warnings
# -O2: Optimization level 2 for release builds
CXXFLAGS = -std=c++17 -Wall -Wextra -O2

# The source file
SRC = huffman.cpp

# The name of the target executable
TARGET = huffman

# Default target, executed when you just run `make`
all: $(TARGET)

# Rule to build the target executable
$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

# Rule to clean up the build directory
clean:
	rm -f $(TARGET)

# Phony targets are not actual files.
# This prevents `make` from getting confused if a file named `clean` or `all` exists.
.PHONY: all clean
