#include "args.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define EXEC_ON_WAIT "--exec-on-wait"
#define EXEC_ON_WAIT_SHORT "-e"
#define EXEC_ON_RESUME "--exec-on-resume"
#define EXEC_ON_RESUME_SHORT "-E"
#define HELP "--help"
#define HELP_SHORT "-h"


int getExecOnWait(int argc, char **argv, char **execOnWait);
int getExecOnResume(int argc, char **argv, char **execOnResume);
int getHelp(int argc, char **argv);

int parseArgs(int argc, char **argv, char **execOnWait, char **execOnResume) {
    int ret;

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


int getHelp(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        if (strcmp(argv[i], HELP) == 0 || strcmp(argv[i], HELP_SHORT) == 0) {
            puts("Help!!!");
            return 0;
        }
    }
    return -1;
}
