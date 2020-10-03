# Choosing compiler
CC = gcc

# compiler flags:
#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CFLAGS  = -Wall -O3
DEBUG_FLAGS  = -Wall -Wpedantic -Wextra -Og

# the build target executable:
TARGET = waitd

all: $(TARGET)

$(TARGET): $(TARGET).c
	$(CC) $(CFLAGS) -o $(TARGET) $(TARGET).c args.h args.c command.c command.h

clean:
	$(RM) $(TARGET)

debug:
	$(CC) $(DEBUG_FLAGS) -o $(TARGET) $(TARGET).c args.h args.c command.c command.h
