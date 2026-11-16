CC      ?= gcc
CFLAGS  ?= -std=c11 -Wall -Wextra -O2
TARGET   = viaje

all: $(TARGET)

$(TARGET): viaje.c
	$(CC) $(CFLAGS) -o $(TARGET) viaje.c

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET) vehiculos.dat

.PHONY: all run clean
