NAME = hellos

CC = gcc
CFLAGS = -Wall -Wextra -O2
PREFIX ?= /usr/local

BUILD_DIR = target/binares
TARGET = $(BUILD_DIR)/$(NAME)

SRC = src/main.c

all: $(TARGET)

$(TARGET): $(SRC)
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(NAME)

uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)

clean:
	rm -rf target

.PHONY: all install uninstall clean
