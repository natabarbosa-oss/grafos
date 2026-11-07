CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDLIBS = -lm
TARGET = main
SRC = $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRC) bellman_ford.h
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

run: $(TARGET)
	./$(TARGET)

test: run

clean:
	rm -f $(TARGET) $(TARGET).exe *.o

.PHONY: all run test clean
