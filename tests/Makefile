CC = gcc
LINK = gcc
CHECK_DIR = /usr/local/Cellar/check/0.12.0
CFLAGS = -g -O1 -Wno-unused-variable -Werror
LFLAGS = -L$(CHECK_DIR)/lib -lcheck -lsubunit -pthread -lrt -lm
INCLUDES = -I$(CHECK_DIR)/include -I../
SRC = $(wildcard *.c)
SRC := $(filter-out test_macro.c, $(SRC))
OBJ = $(patsubst %.c, %.o, $(SRC))

TARGET = test

first: all

%.o: %.c
	$(CC) $(INCLUDES) -c $(CFLAGS) -o $@ $<

all: $(TARGET)
$(TARGET): $(OBJ)
	$(LINK) $(OBJ) $(CFLAGS) $(LFLAGS) -o $(TARGET)
	./$(TARGET)

.PHONY: clean
clean:
	rm -f $(TARGET) $(OBJ)
