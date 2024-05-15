# Pre-compiler and Compiler flags
CXX_FLAGS := -Wall -Wextra -std=c++17 -ggdb -w -I /opt/homebrew/opt/boost@1.85/include 
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
TEST_3 := $(BIN)/iris-mlp-test
TEST_4 := $(BIN)/iris-kmeans-test
TEST_5 := $(BIN)/toy-kmeans-test
TEST_6 := $(BIN)/iris-mlp-train
TEST_7 := $(BIN)/credit-score-mlp-train
TEST_8 := $(BIN)/credit-score-mlp-test
TEST_9 := $(BIN)/mnist-kmeans-test
TEST_10 := $(BIN)/mnist-kmeans-maintain-query-test
BUILD := build

# Library search directories and flags
EXT_LIB :=
LDFLAGS :=
# LDPATHS := $(addprefix -L,$(LIB) $(EXT_LIB))
LDPATHS := 

# Include directories
INC_DIRS := $(INC) $(shell find $(SRC) -type d) 
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Construct build output and dependency filenames
SRCS := $(shell find $(SRC) -name *.cpp)
# SRCS := $(wildcard src/*.cpp src/*/*.cpp)
OBJS := build/CProvGraph.o build/CKMeansProvGraph.o build/KMeans.o  build/ApproximateSubGraph.o build/Clique.o build/Grader.o build/Load.o build/LoopyBP.o build/LoopyBPProv.o build/MLN.o build/Parser.o build/mcsat.o build/BPInfluence.o build/MLP.o 
DEPS := $(OBJS:.o=.d)

ARGS := -o ./data/hypertext-class/sample7/sample71.obs -p ./data/hypertext-class/sample7/prov/sample71.txt -q topic_Department_29 -i all 

# Run task
run: build
	@echo "🚀 Executing..."
	./$(TARGET) $(ARGS)

# Build task
build: clean all

# Main task
all: $(TARGET) $(TEST_1) $(TEST_2) $(TEST_3) $(TEST_4) $(TEST_5) $(TEST_6) $(TEST_7) $(TEST_8) $(TEST_9) $(TEST_10)

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

$(TEST_3): $(OBJS) build/test/iris-mlp-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/iris-mlp-test.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_4): $(OBJS) build/test/iris-kmeans-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/iris-kmeans-test.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_5): $(OBJS) build/test/toy-kmeans-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/toy-kmeans-test.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_6): $(OBJS) build/test/iris-mlp-train.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/iris-mlp-train.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_7): $(OBJS) build/test/credit-score-mlp-train.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/credit-score-mlp-train.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_8): $(OBJS) build/test/credit-score-mlp-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/credit-score-mlp-test.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_9): $(OBJS) build/test/mnist-kmeans-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/mnist-kmeans-test.o -o $@ $(LDPATHS) $(LDFLAGS)

$(TEST_10): $(OBJS) build/test/mnist-kmeans-maintain-query-test.o
	@echo "🚧 Building..."
	mkdir -p $(dir $@)
	$(CXX) $(OBJS) build/test/mnist-kmeans-maintain-query-test.o -o $@ $(LDPATHS) $(LDFLAGS)

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