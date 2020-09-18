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
    int *fds, ret, i;
    glob_t glob_result;

    ret = getGlob(&glob_result);

    if (ret)
        err(EXIT_FAILURE, "glob");

    /* allocate array for opened file descriptors */
    fds = malloc(sizeof(*fds) * (glob_result.gl_pathc + 1));

    if (fds == NULL)
        err(EXIT_FAILURE, "malloc");

    /* open devices */
    for (i = 0; i < glob_result.gl_pathc; i++) {
        fds[i] = open(glob_result.gl_pathv[i], O_RDONLY | O_NONBLOCK);
        if (fds[i] == -1)
            err(EXIT_FAILURE, "open `%s'", glob_result.gl_pathv[i]);
    }

    fds[i] = -1; /* end of array */

    for (;;) {
        char buf[512];
        struct timeval timeout;
        fd_set readfds;
        int nfds = -1;

        FD_ZERO(&readfds);

        /* select(2) might alter the fdset, thus freshly set it
           on every iteration */
        for (i = 0; fds[i] != -1; i++) {
            FD_SET(fds[i], &readfds);
            nfds = fds[i] >= nfds ? fds[i] + 1 : nfds;

            /* read everything what's available on this fd */
            while ((ret = read(fds[i], buf, sizeof(buf))) > 0)
                continue; /* read away input */
            if (ret == -1 && errno != EAGAIN)
                err(EXIT_FAILURE, "read");
        }

        /* same for timeout, 5 seconds here */
        timeout.tv_sec = 5; /* FIXME */
        timeout.tv_usec = 0;

        ret = select(nfds, &readfds, NULL, NULL, &timeout);
        if (ret == -1)
            err(EXIT_FAILURE, "select");
        if (ret == 0) {
            printf("Timeout: start first script\n");
        } else {
            printf("No timeout: start second script\n");
        }
    }

    return 0;
}
