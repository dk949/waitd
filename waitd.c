#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <glob.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/select.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int getGlob(glob_t *glob_result) {
    return glob("/dev/input/event[0-9]*", GLOB_ERR | GLOB_NOSORT | GLOB_NOESCAPE, NULL, glob_result);
}


int main(int argc, char *argv[]) {
    int *fileDescriptors, ret, i;
    glob_t glob_result;


    /* getGlob will return a non 0 value if glob() fails
     * This will most likely be due to lack of privilege
     * given to the program*/
    if (getGlob(&glob_result))
        err(EXIT_FAILURE, "glob");


    /* allocate array for opened file descriptors */
    fileDescriptors = malloc(sizeof(*fileDescriptors) * (glob_result.gl_pathc + 1));

    /* This is very unlikely to happen, but malloc can fail
     * due to lack of free memory */
    if (fileDescriptors == NULL)
        err(EXIT_FAILURE, "malloc");

    /* open devices */
    for (i = 0; i < glob_result.gl_pathc; i++) {
        fileDescriptors[i] = open(glob_result.gl_pathv[i], O_RDONLY | O_NONBLOCK);
        /* open() can fail if the device stopped existing
         * between being discovered with glob() and being
         * accessed here */
        if (fileDescriptors[i] == -1)
            err(EXIT_FAILURE, "open `%s'", glob_result.gl_pathv[i]);
    }

    fileDescriptors[i] = -1; /* end of array */

    for (;;) {
        char buf[512];
        struct timeval timeout;
        fd_set readFileDescriptors;
        int nfds = -1;

        FD_ZERO(&readFileDescriptors);

        /* select(2) might alter the fdset, thus freshly set it
           on every iteration */
        for (i = 0; fileDescriptors[i] != -1; i++) {
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
        timeout.tv_sec = 5; /* FIXME */
        timeout.tv_usec = 0;

        ret = select(nfds, &readFileDescriptors, NULL, NULL, &timeout);
        if (ret == -1)
            err(EXIT_FAILURE, "select");
        if (ret == 0) {
            printf("Timeout: start first script\n");
        } else {
            printf("No timeout: start second script\n");
        }
    }

    free(fileDescriptors);
    return 0;
}
