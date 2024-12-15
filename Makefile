TARGET = pingpongos

BUILD_PATH = build/
INCLUDE_PATH= ./include/

SRC = ./src/ppos-core-aux.c ./src/pingpong-mqueue.c

OBJ = $(wildcard ./$(BUILD_PATH)*.o)

LIB_NAME = $(BUILD_PATH)libppos_static.a

LIB = ./$(LIB_NAME)

LIB_FORCE = -Wl,--whole-archive $(LIB) -Wl,--no-whole-archive

INC= -I$(INCLUDE_PATH)

CC = gcc

# Add -g flag for debugging
CFLAGS = -o $(TARGET) -Wall -O0 -g

all: build

build: build_lib
	$(CC)   $(SRC)  $(LIB) $(CFLAGS) $(INC)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(LIB_NAME)

build_lib: $(OBJ)
	ar rcs $(LIB_NAME) $(OBJ)

.PHONY: all build run clean
