CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
PYTHON := python3

PYTHON_INC := $(shell $(PYTHON)-config --includes)
NUMPY_INC := -I$(shell $(PYTHON) -c "import numpy; print(numpy.get_include())")
PYTHON_LIB := $(shell $(PYTHON)-config --embed --ldflags 2>/dev/null || $(PYTHON)-config --ldflags)

SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET := scheduler

.PHONY: all clean check

all: check $(TARGET)

check:
	@$(PYTHON) -c "import numpy" 2>/dev/null || (echo "Error: NumPy is required. Install it with the MSYS2 package manager."; exit 1)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(PYTHON_INC) $(NUMPY_INC) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(PYTHON_LIB)

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
