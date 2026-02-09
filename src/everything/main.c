#define COMMON_IMPLEMENTATION
#include "../common.h"

#include <X11/Xlib.h>
#include <X11/Xutil.h>

int main(int argc, char **argv)
{
  Display *display = XOpenDisplay(NULL);

  Window root = RootWindow(display, DefaultScreen(display));

  Window window = XCreateSimpleWindow(display, root,
                                      100, 100,
                                      800, 600,
                                      1,
                                      BlackPixel(display, DefaultScreen(display)),
                                      WhitePixel(display, DefaultScreen(display)));

  XSelectInput(display, window,
                ExposureMask |
                KeyPressMask |
                KeyReleaseMask |
                ButtonPressMask |
                ButtonReleaseMask |
                PointerMotionMask |
                StructureNotifyMask);

  XMapWindow(display, window);

  Atom wm_delete = XInternAtom(display, "WM_DELETE_WINDOW", False);
  XSetWMProtocols(display, window, &wm_delete, 1);
  b32 running = true;

  while (running)
  {
    XEvent event;
    XNextEvent(display, &event);

    switch (event.type)
    {
      case Expose:
      {
      } break;
      case ConfigureNotify:
      {
        printf("Resize: %d x %d\n",
               event.xconfigure.width,
               event.xconfigure.height);
      } break;
      case MotionNotify:
      {
        printf("Mouse move: %d %d\n",
               event.xmotion.x,
               event.xmotion.y);
      } break;
      case ButtonPress:
      {
        printf("Mouse button %d down\n",
               event.xbutton.button);
      } break;
      case KeyPress:
      {
        KeySym keysym = XLookupKeysym(&event.xkey, 0);
        printf("Key pressed: %lu\n", keysym);
        if (keysym == XK_Escape)
        {
          running = false;
        }
      } break;

      case ClientMessage:
      {
        if ((Atom)event.xclient.data.l[0] == wm_delete)
        {
          running = 0;
        }
      } break;
    }
  }

  XDestroyWindow(display, window);
  XCloseDisplay(display);
  return 0;
}
