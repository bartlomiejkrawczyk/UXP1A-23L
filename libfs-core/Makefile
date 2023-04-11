LIB_NAME=libfs-core

OBJ_DIR=obj

C_SOURCE_FILES=$(wildcard src/*.c)
CXX_SOURCE_FILES=$(wildcard src/*.cpp)

OBJECT_FILES=$(patsubst src/%.c, $(OBJ_DIR)/%.o, $(C_SOURCE_FILES))
OBJECT_FILES+=$(patsubst src/%.cpp, $(OBJ_DIR)/%.o, $(CXX_SOURCE_FILES))

INCLUDE_FILES=$(wildcard include/*.h) $(wildcard include/*.hpp)

INCLUDE_PATHS=-Iinclude -I.

CFLAGS=-g -std=c11
CXXFLAGS=-g -std=c++17

WARN_FLAGS=-Wall -Wextra -Wpedantic -Werror -Wconversion -Wno-unused-function

.PHONY: clean

all: $(LIB_NAME).a

$(OBJ_DIR)/%.o: src/%.c $(INCLUDE_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(OBJ_DIR)/%.o: src/%.cpp $(INCLUDE_FILES)
	@mkdir -p $(OBJ_DIR)
	$(CXX) $(CXXFLAGS) $(WARN_FLAGS) $(INCLUDE_PATHS) -c -o $@ $<

$(LIB_NAME).a: $(OBJECT_FILES) $(INCLUDE_FILES)
	ar rcs $@ $^

clean:
	rm -f ./$(LIB_NAME).a
	rm -rf ./$(OBJ_DIR)
