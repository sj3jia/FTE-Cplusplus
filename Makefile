CXX = g++
CXXFLAGS = -Wall -Wextra -std=c++20 -Isrc

# Build targets
TARGET = build/gbm_calibrator
TESTS = build/test_gbm

# Source files
SRC_FILES = $(wildcard src/*.cpp)
TEST_FILES = $(wildcard test/*.cpp)
LIB_FILES = $(filter-out src/main.cpp, $(SRC_FILES))

# Object files
SRC_OBJS = $(SRC_FILES:src/%.cpp=build/%.o)
TEST_OBJS = $(TEST_FILES:test/%.cpp=build/%.o)
LIB_OBJS = $(LIB_FILES:src/%.cpp=build/%.o)

# Default target - build everything, run tests, output program
all: $(TARGET) $(TESTS)
	@echo "Running tests to check..."
	@$(TESTS)
	@echo "SUCCESS -- All tests passed!"
	@echo "Build complete! Program available at: $(TARGET)"

# Build main program
$(TARGET): $(SRC_OBJS) | build
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC_OBJS)

# Build test executable	
$(TESTS): $(TEST_OBJS) $(LIB_OBJS) | build
	$(CXX) $(CXXFLAGS) -o $(TESTS) $(TEST_OBJS) $(LIB_OBJS)

# Compile source files
build/%.o: src/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Compile test files
build/%.o: test/%.cpp | build
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create build directory
build:
	mkdir -p build

# Clean build artifacts
clean:
	rm -rf build

.PHONY: all clean 