#include "args.hpp"

#include "util.hpp"

#include <string_view>

#include <cstdlib>
#include <iostream>
#include <optional>

std::optional<double> toNum(std::string_view sv) {
    char *end;
    auto const out = std::strtod(sv.data(), &end);
    if (end != sv.data() + sv.length())
        return std::nullopt;
    else
        return out;
}

auto helpMessage = R"(Usage: waitd [OPTION]...
Run a command if there has been no input for some time.

    -w, --wait TIME                     how long to wait for until running the first command, in seconds
    -e, --exec-on-wait COMMAND          command to be executed after the period of not receiving input
    -p, --poll-rate TIME                [optional] how long to wait before checking if there has been any input

    -h, --help                          display this message and exit
    -v, --version                       print version and exit

Use https://github.com/dk949/waitd/issues for bug reports
man page will be available at some point)";

[[noreturn]] void help() {
    puts(helpMessage);
    exit(0);
}

[[noreturn]] void version() {
    puts("v" VERSION);
    exit(0);
}

template<typename T = void>
[[noreturn]] T failedToParse(char const *msg) {
    fputs(msg, stderr);
    fputc('\n', stderr);
    exit(1);
}

Args Args::parse(char **argv) {
    Args out;
    char **argIt = argv;
    for (char const *a = *(++argIt); *argIt; a = *(++argIt)) {
        if (a == nullptr) break;
        std::string_view arg = a;
        if (arg == "-h" || arg == "--help") help();
        if (arg == "-v" || arg == "--version") version();
        if (arg == "-w" || arg == "--wait") {
            auto const i = toNum(
                (*(++argIt)) ? *argIt : failedToParse<char *>("Could not parse -w, argument is required"));
            if (i)
                out.wait = *i;
            else
                failedToParse("Could not parse -w, argument has to be a number");
        }
        if (arg == "-e" || arg == "--exec-on-wait")
            out.execOnWait = (*(++argIt)) ? *argIt : failedToParse<char *>("Could not parse -e, argument is required");
        if (arg == "-p" || arg == "--poll-rate") {
            auto const i = toNum(
                (*(++argIt)) ? *argIt : failedToParse<char *>("Could not parse -p, argument is required"));
            if (i)
                out.pollRate = *i;
            else
                failedToParse("Could not parse -p, argument has to be a number");
        }
    }
    if (std::isnan(out.wait)) failedToParse("-w is a required option");
    if (out.execOnWait == nullptr) failedToParse("-e is a required option");
    return out;
}
