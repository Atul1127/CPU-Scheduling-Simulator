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
	@$(PYTHON) -c "import matplotlib" 2>/dev/null || (echo "Error: Matplotlib is required. Install it with the MSYS2 package manager."; exit 1)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

clean:
	rm -rf $(OBJ_DIR) $(TARGET)
