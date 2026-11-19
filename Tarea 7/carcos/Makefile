CC       ?= gcc
CFLAGS   ?= -std=c11 -Wall -Wextra -O2
GTKFLAGS  = $(shell pkg-config --cflags gtk+-3.0)
GTKLIBS   = $(shell pkg-config --libs gtk+-3.0)

all: viaje viaje_gui

viaje: viaje.c
	$(CC) $(CFLAGS) -o $@ $<

viaje_gui: viaje_gui.c
	$(CC) $(CFLAGS) $(GTKFLAGS) -o $@ $< $(GTKLIBS)

run: viaje
	./viaje

gui: viaje_gui
	./viaje_gui

clean:
	rm -f viaje viaje_gui reporte_viaje.txt

.PHONY: all run gui clean
