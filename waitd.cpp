#include "args.hpp"
#include "util.hpp"

#include <X11/extensions/scrnsaver.h>
#include <X11/Xlib.h>

#include <chrono>
#include <thread>

namespace chr = std::chrono;
using namespace std::chrono_literals;

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

    void wait(chr::milliseconds threshold, chr::milliseconds pollRate) {
        XResetScreenSaver(dpy);
        while (1) {
            std::this_thread::sleep_for(pollRate);
            XScreenSaverQueryInfo(dpy, win, info);
            auto const idle = chr::milliseconds(int64_t(info->idle));
            if (idle > threshold) return;
        }
    }
};

Context c;

int main(int, char **argv) {
    c.init();
    auto const args = Args::parse(argv);
    while (1) {
        c.wait(chr::milliseconds(int64_t(args.wait * 1000)), chr::milliseconds(int64_t(args.pollRate * 1000)));
        [[maybe_unused]]
        int ret = std::system(args.execOnWait);
    }
}
