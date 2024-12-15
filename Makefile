TARGET = pingpongos

BUILD_PATH = 
# INCLUDE_PATH= ./include/

SRC = ppos-core-aux.c pingpong-semaphore.c




OBJ = $(wildcard ./$(BUILD_PATH)*.o)

LIB_NAME = $(BUILD_PATH)libppos_static.a

LIB = ./$(LIB_NAME)

LIB_FORCE = -Wl,--whole-archive $(LIB) -Wl,--no-whole-archive

# INC= -I$(INCLUDE_PATH)

CC = gcc

# Add -g flag for debugging
CFLAGS = -o $(TARGET)  -lm -Wall -O0 -g

# all: build

all:
	gcc -o pingpongos ppos-core-aux.c pingpong-mqueue.c  ppos-all.o queue.o -lm -Wall -O0 -g



build: build_lib
	$(CC)   $(SRC)  $(LIB) $(CFLAGS) $(INC)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(LIB_NAME)

build_lib: $(OBJ)
	ar rcs $(LIB_NAME) $(OBJ)

.PHONY: all build run clean
