VERSION = "$(shell git describe)"

DESTDIR ?= /usr
PREFIX  ?= /usr

CXX ?= c++

CPPFLAGS = -DVERSION=\"$(VERSION)\"

CFLAGS = -s -Wall -Wextra -Wshadow -Wpedantic -O3


LDFLAGS =-lXss -lX11
