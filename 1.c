#include <stdio.h>
#include <X11/Xlib.h>

void x11_demo(void) {
	Display *display = XOpenDisplay(NULL);
	if (display == NULL) {
		fprintf(stderr, "%s\n", "XOpenDisplay() failed");
		return;
	}
	XSetWindowAttributes attributes;
	for (size_t i = 0; i < sizeof(attributes); i++) {
		*((unsigned char *)(&attributes)+i) = 0x00;
	}
	Window window = XCreateWindow(
		display, 
		DefaultRootWindow(display), 
		0, 
		0, 
		500u, 
		500u, 
		0u,				// border width
		CopyFromParent,	// depth
		InputOutput,	// class
		CopyFromParent,	// visual
		0ul, 
		NULL);
	if (BadWindow == XMapWindow(display, window)) {
		fprintf(stderr, "%s\n", "XMapWindow() failed");
	}
	if (BadWindow == XSelectInput(display, window, 
		KeyPressMask | KeyReleaseMask |
		ButtonPressMask | ButtonReleaseMask |
		PointerMotionMask)) {
			fprintf(stderr, "%s\n", "XSelectInput() failed");
	}
	
	XEvent xevent;
	for (size_t i = 0; i < sizeof(xevent); i++) {
		*((unsigned char *)&xevent) = 0x00;
	}
	
	// === DO STUFF HERE ===
	while (BadWindow != XNextEvent(display, &xevent)) {
		fprintf(stderr, "Pointer coordinates: %d, %d\n",
			xevent.xmotion.x_root,
			xevent.xmotion.y_root);
		if (KeyRelease == xevent.xkey.type &&
			24 == xevent.xkey.keycode) {  // q
			break;
		}
	}
	// === END DO STUFF ===
	
	XDestroyWindow(display, window);
	int unused = XCloseDisplay(display);
	unused++;
}

int main(int argc, char **argv) {
	argc += 0;
	argv[0][0] += 0;
	x11_demo();
}
