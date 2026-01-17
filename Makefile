# Makefile for cliphist-panel-plugin

PLUGIN_NAME = cliphist-panel-plugin
PLUGIN_SO = lib$(PLUGIN_NAME).so

# Get pkg-config flags
CFLAGS = -fPIC -Wall -O2 \
	$(shell pkg-config --cflags libxfce4panel-2.0 gtk+-3.0 libxfce4util-1.0)

LDFLAGS = -shared \
	$(shell pkg-config --libs libxfce4panel-2.0 gtk+-3.0 libxfce4util-1.0)

# Installation directories
PREFIX ?= /usr
LIBDIR = $(PREFIX)/lib/x86_64-linux-gnu/xfce4/panel/plugins
DATADIR = $(PREFIX)/share/xfce4/panel/plugins
ICONDIR = $(PREFIX)/share/pixmaps

# Source files
SOURCES = cliphist-plugin.c cliphist-button.c cliphist-client.c cliphist-entry.c
OBJECTS = $(SOURCES:.c=.o)

.PHONY: all clean install uninstall

all: $(PLUGIN_SO)

$(PLUGIN_SO): $(OBJECTS)
	$(CC) $(OBJECTS) $(LDFLAGS) -o $(PLUGIN_SO)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

install: $(PLUGIN_SO)
	install -d $(DESTDIR)$(LIBDIR)
	install -m 755 $(PLUGIN_SO) $(DESTDIR)$(LIBDIR)/
	install -d $(DESTDIR)$(DATADIR)
	install -m 644 $(PLUGIN_NAME).desktop $(DESTDIR)$(DATADIR)/
	install -d $(DESTDIR)$(ICONDIR)
	install -m 644 clipboard-icon.svg $(DESTDIR)$(ICONDIR)/cliphist-icon.svg

uninstall:
	rm -f $(DESTDIR)$(LIBDIR)/$(PLUGIN_SO)
	rm -f $(DESTDIR)$(DATADIR)/$(PLUGIN_NAME).desktop
	rm -f $(DESTDIR)$(ICONDIR)/cliphist-icon.svg

clean:
	rm -f $(OBJECTS) $(PLUGIN_SO)
