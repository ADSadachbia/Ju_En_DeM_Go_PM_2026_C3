CC      = gcc
CFLAGS  = -std=c11 -Wall -Wextra -Wpedantic
LDLIBS  = -lm
TARGET  = prestamos
OBJS    = main.o negocio.o json.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS) $(LDLIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all run clean
