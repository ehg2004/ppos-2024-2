TARGET = pingpongos

BUILD_PATH = 
# INCLUDE_PATH= ./include/

SRC = ppos-disk-manager.c pingpong-disco1.c




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
# gcc -o pingpongos ppos-core-aux.c pingpong-mqueue.c  ppos-all.o queue.o -lm -Wall -O0 -g
	gcc -o pingpongos_DISK ppos-disk-manager.c ppos-all.o queue.o disk-driver.c ppos-core-aux.c pingpong-disco1.c -lrt -Wall


build: build_lib
	$(CC)   $(SRC)  $(LIB) $(CFLAGS) $(INC)

run: build
	./$(TARGET)

clean:
	rm -f $(TARGET) $(LIB_NAME)

build_lib: $(OBJ)
	ar rcs $(LIB_NAME) $(OBJ)

.PHONY: all build run clean
