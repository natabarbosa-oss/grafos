CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c11
LDLIBS = -lm
TARGET = main
ifeq ($(OS),Windows_NT)
TARGET = main.exe
endif
SRC = $(wildcard *.c)

all: $(TARGET)

$(TARGET): $(SRC) agm.h
	$(CC) $(CFLAGS) $(SRC) -o $@ $(LDLIBS)

run: $(TARGET)
ifeq ($(OS),Windows_NT)
	$(TARGET)
else
	./$(TARGET)
endif

test: run

clean:
ifeq ($(OS),Windows_NT)
	if exist main.exe del /Q main.exe
else
	rm -f main *.o
endif

.PHONY: all run test clean
