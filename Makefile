PREFIX = /usr/local

CC = cc
LD = $(CC)

CFLAGS = -std=c99 -pedantic -Wall -Os
LDFLAGS = -lxcb -lwm

BIN = xfocusd
SRC = xfocusd.c

all: $(BIN)

xfocusd: $(SRC)
	$(CC) $(SRC) $(CFLAGS) $(LDFLAGS) -o $(BIN)

install: $(BIN)
	test -d $(DESTDIR)$(PREFIX)/bin || mkdir -p $(DESTDIR)$(PREFIX)/bin
	install -pm 755 $(BIN) $(DESTDIR)$(PREFIX)/bin

clean:
	rm -rf $(BIN)