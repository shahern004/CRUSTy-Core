CC = x86_64-w64-mingw32-gcc
CXX = x86_64-w64-mingw32-g++
CFLAGS = -Wall -Iinclude
LDFLAGS = -static

all: prep main.exe

prep:
	cargo build

main.exe: main.c
	$(CC) $(CFLAGS) -o $@ $< -L./target/debug -lmy_rust_lib $(LDFLAGS)

clean:
	cargo clean
	del main.exe

.PHONY: all prep clean