#include "args.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXEC_ON_WAIT "--exec-on-wait"
#define EXEC_ON_WAIT_SHORT "-e"
#define EXEC_ON_RESUME "--exec-on-resume"
#define EXEC_ON_RESUME_SHORT "-E"
#define WAIT "--wait"
#define WAIT_SHORT "-w"
#define NO_CHECK "--no-check"
#define NO_CHECK_SHORT "-n"
#define HELP "--help"
#define HELP_SHORT "-h"


int getExecOnWait(int argc, char **argv, char **execOnWait);
int getExecOnResume(int argc, char **argv, char **execOnResume);
int getWait(int argc, char **argv, int *wait);
int getNoCheck(int argc, char **argv, int *noCheck);
int getHelp(int argc, char **argv);

int parseArgs(int argc, char **argv, char **execOnWait, char **execOnResume, int *wait, int *noCheck) {
    int ret;
    // This monstrosity needs a rewrite
    if (!getHelp(argc, argv)) {
        errno = 0;
        return 3;
    }

    if (geteuid() != 0) {
        errno = EACCES;
        return 2;
    }

    ret = getExecOnWait(argc, argv, execOnWait);
    if (ret != 0) {
        if (ret == -1) {
            execOnWait = NULL;
        } else {
            return 1;
        }
    }

    ret = getExecOnResume(argc, argv, execOnResume);
    if (ret != 0) {
        if (ret == -1) {
            execOnResume = NULL;
        } else {
            return 1;
        }
    }

    ret = getWait(argc, argv, wait);
    if (ret != 0) {
        if (ret == -1) {
            return 4;
        } else {
            return 1;
        }
    }

    getNoCheck(argc, argv, noCheck);

    return 0;
}

int getExecOnWait(int argc, char **argv, char **execOnWait) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], EXEC_ON_WAIT) == 0 || strcmp(argv[i], EXEC_ON_WAIT_SHORT) == 0) {
            if ((i + 1) < argc) {
                *execOnWait = argv[i + 1];
                return 0;
            } else {
                errno = EINVAL;
                return 1;
            }
        }
    }
    return -1;
}

int getExecOnResume(int argc, char **argv, char **execOnResume) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], EXEC_ON_RESUME) == 0 || strcmp(argv[i], EXEC_ON_RESUME_SHORT) == 0) {
            if ((i + 1) < argc) {
                *execOnResume = argv[i + 1];
                return 0;
            } else {
                errno = EINVAL;
                return 1;
            }
        }
    }
    return -1;
}

int getWait(int argc, char **argv, int *wait) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], WAIT) == 0 || strcmp(argv[i], WAIT_SHORT) == 0) {
            if ((i + 1) < argc) {
                *wait = atoi(argv[i + 1]);
                if (*wait == 0) {
                    errno = EINVAL;
                    return 1;
                }
                return 0;
            } else {
                errno = EINVAL;
                return 1;
            }
        }
    }

    errno = EINVAL;
    return -1;
}

int getNoCheck(int argc, char **argv, int *noCheck) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], NO_CHECK) == 0 || strcmp(argv[i], NO_CHECK_SHORT) == 0) {
            *noCheck = 0;
            return 0;
        }
    }
    return -1;
}


char *helpMessage =
    "Usage: waitd [OPTION]...\n"
    "waits for a lack of input and runs a command when no input has been detected for some time"
    "\n"
    "Mandatory arguments to long options are mandatory for short options too.\n"
    "  -w, --wait TIME              how long to wait for until running the first command\n"
    "  -e, --exec-on-wait COMMAND   command to be executed after the period of not receiving "
    "input\n"
    "  -E, --exec-on-resume COMMAND command to be executed upon finally receiving input\n"
    "  -n, --no-check               do not check if the command exists\n"
    "\n"
    "  -h, --help     display this help and exit\n"
    "\n"
    "Exit status:\n"
    "0  if OK,\n"
    "1  if a simple problem has occurred (permission denied  or option needs arguments\n"
    "2  if command could not be found\n"
    "\n"
    "note:\n"
    "   the --no-check option is only there to run commands not in PATH)\n"
    "   it does not make the application faster/more efficient (the check is only performed once)\n"
    "\n"
    "contact dkatz488@gmail.com for bug reports\n"
    "man page will be available at some point\n";

int getHelp(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], HELP) == 0 || strcmp(argv[i], HELP_SHORT) == 0) {
            printf(helpMessage);
            return 0;
        }
    }
    return -1;
}
