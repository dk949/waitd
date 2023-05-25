#include "args.hpp"
#include "util.hpp"

#include <X11/extensions/scrnsaver.h>
#include <X11/Xlib.h>

struct Context {
    Window win;
    Display *dpy;
    XScreenSaverInfo *info;

    void init() {
        dpy = XOpenDisplay(NULL);
        if (!dpy) die("Could not open display");
        win = DefaultRootWindow(dpy);
        info = XScreenSaverAllocInfo();
    }

    ~Context() {

        XFree(info);
        XCloseDisplay(dpy);
    }

    void wait(unsigned long threshold, long pollRate) {
        XResetScreenSaver(dpy);
        while (1) {
            msleep(pollRate);
            XScreenSaverQueryInfo(dpy, win, info);
            unsigned long idle = info->idle;
            if (idle > threshold) return;
        }
    }
};

Context c;

int main(int, char **argv) {
    c.init();
    auto const args = Args::parse(argv);
    while (1) {
        c.wait(args.wait * 1000, args.pollRate * 1000);
        [[maybe_unused]] int ret = std::system(args.execOnWait);
    }
}
