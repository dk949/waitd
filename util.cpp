#include "util.hpp"

#include <unistd.h>

#include <cstdlib>
#include <iostream>

int msleep(long mseconds) {
    return usleep(mseconds * 1000);
}
