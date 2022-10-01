#ifndef args_h
#define args_h
#include <cmath>

struct Args {
    double wait = NAN;
    double pollRate = 2.0;
    char *execOnWait = nullptr;
    static Args parse(char **argv);
};

#endif  // args_h
