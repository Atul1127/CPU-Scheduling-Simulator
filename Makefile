CXX := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -O2
PYTHON := python3

SRC_DIR := src
OBJ_DIR := obj
SRCS := $(wildcard $(SRC_DIR)/*.cpp)
OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRCS))
TARGET := scheduler

.PHONY: all clean check

all: check $(TARGET)

check:
	@$(CXX) --version >/dev/null 2>&1 || (echo "Error: GCC/g++ is required."; exit 1)
	@$(PYTHON) --version >/dev/null 2>&1 || (echo "Error: Python 3 is required for graphical Gantt charts."; exit 1)
	@$(PYTHON) -c "import numpy, matplotlib" 2>/dev/null || (echo "Error: NumPy and Matplotlib are required for graphical Gantt charts."; exit 1)
	@echo "Environment check passed."

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^
	@echo "Build complete: ./$(TARGET)"

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
	@echo "Build artifacts removed."
