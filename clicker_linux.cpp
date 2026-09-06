#ifdef __linux__
#include "clicker.h"
#include <X11/Xlib.h>
#include <X11/extensions/XTest.h>

void performLeftClick() {
    Display* display = XOpenDisplay(nullptr);
    if (!display) return;
    XTestFakeButtonEvent(display, Button1, True,  CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
    XCloseDisplay(display);
}
#endif
