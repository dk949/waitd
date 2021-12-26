#ifndef command_h
#define command_h


int checkCommand(char *commandName);
int execCommand(char *commandName);
int sliceCommand(const char *str, char *buffer, unsigned long end, char endChar);

#endif  // command_h
