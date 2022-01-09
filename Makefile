SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

CPPFLAGS := -std=c++14 -Iinclude -MP -MMD -Wall -Wextra -Werror -Wconversion -fopenmp
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -pthread -lboost_program_options

# playing with some parallelization here... but performance is mixed
parallel: CPPFLAGS += -O2 -fopenmp -DPARALLELIZE_FOR_LOOPS
parallel: LDFLAGS += -fopenmp
all: CPPFLAGS += -O2
sim: CPPFLAGS += -O2
test: CPPFLAGS += -O2
debug: CPPFLAGS += -DDEBUG -Og -g -fno-access-control

.PHONY: all clean clena debug

EXE := $(BIN_DIR)/sim

DEBUG := $(BIN_DIR)/debug

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)

$(info Found source files $(SRC))

OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

$(info OBJ is $(OBJ))

DEBUG_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.debug.o, $(SRC))

ROOTS := include/util/roots.h

all: $(EXE) test

sim: $(EXE)

parallel: $(EXE)

debug: $(DEBUG)

# I make this typo constantly
clena: clean

clean:
	@$(RM) -rv $(OBJ_DIR) $(BIN_DIR)
	@$(RM) -rv ./*.o
	echo "Removing CMake artifacts..."
	@$(RM) -r tst/CMakeCache.txt
	@$(RM) -r tst/CMakeFiles/
	@$(RM) -r tst/CTestTestfile.cmake
	@$(RM) -r tst/Makefile
	@$(RM) -r tst/_deps/
	@$(RM) -r tst/bin/
	@$(RM) -r tst/build/
	@$(RM) -r tst/cmake_install.cmake
	@$(RM) -r tst/lib/
	@$(RM) -r tst/*_include.cmake
	@$(RM) include/util/roots.h

# Hacky, but they're tests...
test: $(OBJ)
	(cd tst && cmake -S . -B build)
	(cd tst && cmake --build build)
	tst/build/test_sim
	tst/build/test_vector
	tst/build/test_particle

$(DEBUG): $(DEBUG_OBJ) | $(BIN_DIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ $(LDFLAGS) -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(ROOTS)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.debug.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR) $(ROOTS)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@ $@/component $@/simulation $@/graphics $@/cli $@/physics $@/graphics $@/util $@/demo

$(BIN_DIR):
	mkdir -p $@

$(ROOTS): include/util/root.py
	python3 include/util/root.py