#include "args.h"
#include "command.h"

#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define GOT_NO_INPUT 1
#define GOT_INPUT    2

void getFiles(const glob_t *glob_result, int *fileDescriptors);
void getGlob(glob_t *glob_result);
void mainLoop(int *fileDescriptors, int time, char *execOnWait, char *execOnResume);
void waitFunc(int f, char *execOnWait, char *execOnResume);


void waitFunc(int f, char *execOnWait, char *execOnResume) {
    static int which = 0;
    if (which != f) {
        which = f;
        if (f == GOT_INPUT) {
            execCommand(execOnResume);
        } else {
            execCommand(execOnWait);
        }
    }
}

void getGlob(glob_t *glob_result) {
    int ret = glob("/dev/input/event[0-9]*", GLOB_ERR | GLOB_NOSORT | GLOB_NOESCAPE, NULL, glob_result);

    /* getGlob will return a non 0 value if glob() fails
     * This will most likely be due to lack of privilege
     * given to the program*/
    if (ret)
        err(EXIT_FAILURE, "glob");
}

void getFiles(const glob_t *const glob_result, int *fileDescriptors) {
    int i;
    /* This is very unlikely to happen, but malloc can fail
     * due to lack of free memory */
    if (fileDescriptors == NULL)
        err(EXIT_FAILURE, "malloc");

    /* open devices */
    for (i = 0; i < glob_result->gl_pathc; i++) {
        fileDescriptors[i] = open(glob_result->gl_pathv[i], O_RDONLY | O_NONBLOCK);
        /* open() can fail if the device stopped existing
         * between being discovered with glob() and being
         * accessed here */
        if (fileDescriptors[i] == -1)
            err(EXIT_FAILURE, "open `%s'", glob_result->gl_pathv[i]);
    }

    fileDescriptors[i] = -1; /* end of array */
}

void mainLoop(int *fileDescriptors, int time, char *execOnWait, char *execOnResume) {
    long int ret;
    for (;;) {
        char buf[512];
        struct timeval timeout;
        fd_set readFileDescriptors;
        int nfds = -1;

        FD_ZERO(&readFileDescriptors);

        /* select(2) might alter the fdset, thus freshly set it
           on every iteration */
        for (int i = 0; fileDescriptors[i] != -1; i++) {
            FD_SET(fileDescriptors[i], &readFileDescriptors);
            nfds = fileDescriptors[i] >= nfds ? fileDescriptors[i] + 1 : nfds;

            /* read everything what's available on this fd */
            while ((ret = read(fileDescriptors[i], buf, sizeof(buf))) > 0)
                continue; /* read away input */

            /* This is triggered if the read fails for any
             * reason other then the file not having anymore data */
            if (ret == -1 && errno != EAGAIN)
                err(EXIT_FAILURE, "read");
        }

        /* same for timeout, 5 seconds here */
        timeout.tv_sec = time;

        ret = select(nfds, &readFileDescriptors, NULL, NULL, &timeout);
        if (ret == -1)
            err(EXIT_FAILURE, "select");
        if (ret == 0) {
            waitFunc(GOT_NO_INPUT, execOnWait, execOnResume);
        } else {
            waitFunc(GOT_INPUT, execOnWait, execOnResume);
        }
    }
}

int main(int argc, char **argv) {
    char *execOnWait = NULL;
    char *execOnResume = NULL;
    int wait = 0;

    int ret = parseArgs(argc, argv, &execOnWait, &execOnResume, &wait);
    switch (ret) {
        case 1:
            err(EXIT_FAILURE, "Flag requires an argument");
        case 2:
            err(EXIT_FAILURE, "Cannot access event devices");
        case 3:
            exit(0);
        case 4:
            err(EXIT_FAILURE, "-w or --wait option required to set the timeout");
        default:
            break;
    }

    if (checkCommand(execOnWait) != 0) {
        errno = ENOENT;
        char buf[128];
        sliceCommand(execOnWait, buf, strlen(execOnWait), ' ');
        char command[161];
        strcpy(command, buf);
        strcat(command, ": command not found");
        err(2, command);
    }
    if (checkCommand(execOnResume) != 0) {
        errno = ENOENT;
        char buf[128];
        sliceCommand(execOnResume, buf, strlen(execOnResume), ' ');
        char command[161];
        strcpy(command, buf);
        strcat(command, ": command not found");
        err(2, command);
    }


    int *fileDescriptors;
    glob_t glob_result;
    getGlob(&glob_result);

    /* allocate array for opened file descriptors */
    fileDescriptors = malloc(sizeof(*fileDescriptors) * (glob_result.gl_pathc + 1));
    getFiles(&glob_result, fileDescriptors);

    mainLoop(fileDescriptors, wait, execOnWait, execOnResume);
    return 0;
}
