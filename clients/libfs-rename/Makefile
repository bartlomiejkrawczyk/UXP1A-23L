BIN_NAME=libfs-rename

OBJ_DIR=obj

C_SOURCE_FILES=$(wildcard src/*.c)
CXX_SOURCE_FILES=$(wildcard src/*.cpp)

OBJECT_FILES=$(patsubst src/%.c, $(OBJ_DIR)/%.o, $(C_SOURCE_FILES))
OBJECT_FILES+=$(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(CXX_SOURCE_FILES))

INCLUDE_FILES=$(wildcard ../../libfs-core/include/*.h) $(wildcard ../../libfs-core/include/*.hpp)
INCLUDE_FILES+=$(wildcard ../../libfs/include/*.h) $(wildcard ../../libfs/include/*.hpp)

INCLUDE_PATHS=-I. -I../../libfs-core/include -I../../libfs/include

CFLAGS=-g -std=c11 -fdiagnostics-color=always
CXXFLAGS=-g -std=c++17 -fdiagnostics-color=always

LDFLAGS=-L../../libfs -l:libfs.a -L../../libfs-core -l:libfs-core.a

WARN_FLAGS=-Wall -Wextra -Wpedantic -Werror -Wconversion -Wno-unused-function

.PHONY: clean

all: $(BIN_NAME)

$(OBJ_DIR)/%.o: src/%.c $(INCLUDE_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/%.cpp $(INCLUDE_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(BIN_NAME): $(OBJECT_FILES) $(INCLUDE_FILES)
	$(CXX) $(CXXFLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -o $@ $(OBJECT_FILES) $(LDFLAGS)

clean:
	rm -f ./$(BIN_NAME)
	rm -rf ./$(OBJ_DIR)

