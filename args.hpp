#ifndef args_h
#define args_h
#include <cmath>

struct Args {
    double wait = NAN;
    double pollRate = 2.0;
    char *execOnWait = nullptr;
    [[nodiscard]]
    static Args parse(char **argv) noexcept;
};

#endif  // args_h
