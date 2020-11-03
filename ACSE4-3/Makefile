CXX = g++
CXXFLAGS = -Wall -std=c++17
LDFLAGS =
SOURCE_DIR = src
INCLUDE_DIR = includes
TEST_DIR = tests

BUILD_DIR = src
BIN_DIR = bin
ALL_BUILD_DIR = $(BUILD_DIR) $(BIN_DIR)

TEST_BUILD_DIR = $(TEST_DIR)/build
TEST_BIN_DIR = $(TEST_DIR)/bin
ALL_TEST_BUILD_DIR = $(TEST_BUILD_DIR) $(TEST_BIN_DIR)

all: SPH_2D

SPH_2D: $(BIN_DIR)/SPH_2D

.PHONY: SPH_2D all clean

.PHONY: directories test_directories
