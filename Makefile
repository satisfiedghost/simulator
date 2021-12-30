SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

CPPFLAGS := -Iinclude -Isrc/component -MP -MMD -Wall -Wextra -Werror
LDFLAGS := -lsfml-graphics -lsfml-window -lsfml-system -pthread

all: CPPFLAGS += -O3
debug: CPPFLAGS += -DDEBUG -g

.PHONY: all clean clena debug test

EXE := $(BIN_DIR)/sim

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)

$(info Found source files $(SRC))

OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
$(info OBJ is $(OBJ))

all: $(EXE) test

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ $(LDFLAGS) -o $@ 

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@ $@/simulation $@/graphics

$(BIN_DIR):
	mkdir -p $@

clean:
	@$(RM) -rv $(OBJ_DIR) $(BIN_DIR)

# I make this typo constantly
clena: clean

debug: $(EXE)

# Hacky, but they're tests...
test:
	(cd tst && cmake -S . -B build)
	(cd tst && cmake --build build)
	tst/build/test_sim
	tst/build/test_vector
