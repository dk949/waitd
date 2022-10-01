VERSION = 2.1.0

PREFIX := /usr

CXX := c++

CPPFLAGS :=
CPPFLAGS += -DVERSION=\"$(VERSION)\"

CFLAGS :=
CFLAGS += -s -Wall -Wextra -Wshadow -Wpedantic -O3


LDFLAGS :=
LDFLAGS +=-lXss -lX11
