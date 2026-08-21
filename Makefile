 NAME = hellos

 CC = clang
 CFLAGS = -Wall -Wextra -O2 -I.
 PREFIX ?= /usr/local

 BUILD_DIR = target/binares
 TARGET = $(BUILD_DIR)/$(NAME)

 SDK := $(HOME)/Projects/openwrt-sdk-18.06.9-ar71xx-tiny_gcc-7.3.0_musl.Linux-x86_64
 OPENWRT_CC := $(SDK)/staging_dir/toolchain-mips_24kc_gcc-7.3.0_musl/bin/mips-openwrt-linux-musl-gcc
 OPENWRT_TARGET := $(BUILD_DIR)/$(NAME)-openwrt

 SRCS = src/main.c src/lib/getInfo.c
 OBJS = $(BUILD_DIR)/main.o $(BUILD_DIR)/getInfo.o

 all: $(TARGET)

 $(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(TARGET)

 $(BUILD_DIR)/%.o: src/%.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

 $(BUILD_DIR)/getInfo.o: src/lib/getInfo.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

 install: all
	install -d $(DESTDIR)$(PREFIX)/bin
	install -m 755 $(TARGET) $(DESTDIR)$(PREFIX)/bin/$(NAME)

 uninstall:
	rm -f $(DESTDIR)$(PREFIX)/bin/$(NAME)

 clean:
	rm -rf target

 openwrt:
	STAGING_DIR="$(SDK)/staging_dir" $(OPENWRT_CC) \
	 $(CFLAGS) $(SRCS) -o $(OPENWRT_TARGET)

 .PHONY: all install uninstall clean
