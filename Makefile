# Pre-compiler and Compiler flags
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb -w
PRE_FLAGS := -MMD -MP

# Project directory structure
BIN := bin
SRC := src
LIB := lib
INC := include
MAINFILE := $(SRC)/main.cpp

# Build directories and output
TARGET := $(BIN)/pxai
TEST_1 := $(BIN)/cprov-approx-ice-counter-overhead
TEST_2 := $(BIN)/cprov-maintain-query-overhead
BUILD := build

# Library search directories and flags
EXT_LIB :=
LDFLAGS :=
LDPATHS := $(addprefix -L,$(LIB) $(EXT_LIB))

# Include directories
INC_DIRS := $(INC) $(shell find $(SRC) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Construct build output and dependency filenames
SRCS := $(shell find $(SRC) -name *.cpp)
# SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := build/ApproximateSubGraph.o build/Clique.o build/Grader.o build/Load.o build/LoopyBP.o build/LoopyBPProv.o build/MLN.o build/Parser.o build/mcsat.o build/BPInfluence.o 
DEPS := $(OBJS:.o=.d)

ARGS := ./data/hypertext-class/sample7/sample71.obs -p ./data/hypertext-class/sample7/prov/sample71.txt -q topic_Department_29 -m lbp -r 1 -i all -rule_name 1 -delta 1

# Run task
run: build
	@echo "🚀 Executing..."
	./$(TARGET) $(ARGS)

# Build task
build: clean all

# Main task
all: $(TARGET) $(TEST_1) $(TEST_2)

# Task producing target from built files
$(TARGET): $(OBJS) build/test/main.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/main.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_1): $(OBJS) build/test/cprov-approx-ice-counter-overhead.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/cprov-approx-ice-counter-overhead.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_2): $(OBJS) build/test/cprov-maintain-query-overhead.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/cprov-maintain-query-overhead.o -o $@ $(LDPATHS) $(LDFLAGS)

# Compile all cpp files
$(BUILD)/%.o: $(SRC)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXX_FLAGS) $(PRE_FLAGS) $(INC_FLAGS) -c -o $@ $< $(LDPATHS) $(LDFLAGS)

# Clean task
.PHONY: clean
clean:
	@echo "🧹 Clearing..."
	rm -rf build

# Include all dependencies
-include $(DEPS)