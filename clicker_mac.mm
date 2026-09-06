#ifdef __APPLE__
#include "clicker.h"
#include <ApplicationServices/ApplicationServices.h>

void performLeftClick() {
    CGEventRef probe = CGEventCreate(nullptr);
    CGPoint pos = CGEventGetLocation(probe);
    CFRelease(probe);

    CGEventRef down = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseDown, pos, kCGMouseButtonLeft);
    CGEventRef up   = CGEventCreateMouseEvent(nullptr, kCGEventLeftMouseUp,   pos, kCGMouseButtonLeft);
    CGEventPost(kCGHIDEventTap, down);
    CGEventPost(kCGHIDEventTap, up);
    CFRelease(down);
    CFRelease(up);
}
#endif
