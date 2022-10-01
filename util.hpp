#ifndef UTIL_H
#define UTIL_H

#include <cerrno>
#include <cstring>
#include <iostream>

template<typename... Ts>
[[noreturn]] void die(Ts &&...args) {
    std::cerr << "waitd: ";
    (std::cerr << ... << args);
    if (errno) std::cerr << ": " << std::strerror(errno) << '\n';
    //exit(1);
    std::terminate();
}

int msleep(long mseconds);

#endif  // UTIL_H
