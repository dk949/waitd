#include "command.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int sliceCommand(const char *str, char *buffer, size_t end, const char endChar) {
    int j = 0;
    int i = 0;
    while (str[i] != endChar || i < end) {
        buffer[j++] = str[i++];
    }
    buffer[j] = 0;
    return 0;
}

int checkCommand(char *commandName) {
    if (commandName) {
        char buf[128];
        sliceCommand(commandName, buf, strlen(commandName), ' ');
        char command[512];
        strcpy(command, "which ");
        strcat(command, buf);
        strcat(command, " 2> /dev/null 1> /dev/null\0");
        return system(command);
    } else {
        return 0;
    }
}


int execCommand(char *commandName) { return system(commandName); }
