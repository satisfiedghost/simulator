SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

CPPFLAGS := -Iinclude -MP -MMD -Wall -Wextra -Werror -Wconversion -fopenmp
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -pthread -lboost_program_options -fopenmp

# playing with some parallelization here... but performance is mixed
parallel: CPPFLAGS += -O2 -fopenmp -DPARALLELIZE_FOR_LOOPS
parallel: LDFLAGS += -fopenmp
all: CPPFLAGS += -O2
sim: CPPFLAGS += -O2
test: CPPFLAGS += -O2
debug: CPPFLAGS += -DDEBUG -Og -g

.PHONY: all clean clena debug

EXE := $(BIN_DIR)/sim

DEBUG := $(BIN_DIR)/debug

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)

$(info Found source files $(SRC))

OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

$(info OBJ is $(OBJ))

DEBUG_OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.debug.o, $(SRC))

all: $(EXE) test

sim: $(EXE)

parallel: $(EXE)

debug: $(DEBUG)

# I make this typo constantly
clena: clean

clean:
	@$(RM) -rv $(OBJ_DIR) $(BIN_DIR)
	@$(RM) -rv ./*.o
	@$(RM) -rv tst/CMakeCache.txt
	@$(RM) -rv tst/CMakeFiles/
	@$(RM) -rv tst/CTestTestfile.cmake
	@$(RM) -rv tst/Makefile
	@$(RM) -rv tst/_deps/
	@$(RM) -rv tst/bin/
	@$(RM) -rv tst/build/
	@$(RM) -rv tst/cmake_install.cmake
	@$(RM) -rv tst/lib/
	@$(RM) -rv tst/*_include.cmake

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

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR)/%.debug.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@ $@/component $@/simulation $@/graphics $@/cli $@/phys $@/graphics

$(BIN_DIR):
	mkdir -p $@
