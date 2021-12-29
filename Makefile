SRC_DIR := src
OBJ_DIR := obj
BIN_DIR := bin

CPPFLAGS := -Iinclude -Isrc/component -MP -MMD -Wall -Wextra -Werror

all: CPPFLAGS += -O3
debug: CPPFLAGS += -DDEBUG -g

.PHONY: all clean

EXE := $(BIN_DIR)/sim

SRC := $(wildcard $(SRC_DIR)/*.cpp) $(wildcard $(SRC_DIR)/*/*.cpp)

$(info Fource source files $(SRC))

OBJ := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))
$(info OBJ is $(OBJ))

all: $(EXE)

$(EXE): $(OBJ) | $(BIN_DIR)
	$(CXX) $^ -o $@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CXX) $(CPPFLAGS) -c $< -o $@

$(OBJ_DIR):
	mkdir -p $@ $@/simulation

$(BIN_DIR):
	mkdir -p $@
clean:
	@$(RM) -rv $(OBJ_DIR)

debug: $(EXE)
