PREFIX ?= /usr/local
MANPREFIX ?= $(PREFIX)/share/man

CC = cc
LD = $(CC)

CFLAGS = -std=c99 -pedantic -Wall -Os
LDFLAGS = -lxcb -lwm

BIN = xfocusd
SRC = xfocusd.c
MAN = xfocusd.1

all: $(BIN)

xfocusd: $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(BIN)

install: $(BIN) $(MAN)
	test -d $(DESTDIR)$(PREFIX)/bin || mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -pm 755 $(BIN) $(DESTDIR)$(PREFIX)/bin
	test -d $(DESTDIR)$(MANPREFIX)/man1 || mkdir -p $(DESTDIR)$(MANPREFIX)/man1
	install -pm 644 $(MAN) $(DESTDIR)$(MANPREFIX)/man1/$(MAN)

clean:
	rm -rf $(BIN)