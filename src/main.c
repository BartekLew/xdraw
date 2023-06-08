#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/XKBlib.h>
#include <X11/Xft/Xft.h>

#include "../include/window.h"
#include "../include/graphics.h"
#include "../include/colors.h"

#define PROG_NAME "paint programn"
#define LOG(msg, ...) do {						\
		char str[1024];						\
		sprintf(str, msg __VA_OPT__(,) __VA_ARGS__);		\
		fprintf(stdout, "[INFO] %s\n", str);			\
	} while (0)

#define SWAP(a, b) do {\
		typeof(a) temp = b;			\
		b = a;					\
		a = temp;				\
	} while (0)

#define RESIZE_PIXMAP 0


Window main_win;

void run(void)
{
	XEvent ev;
	int holding = 0, prev_x = 0, prev_y = 0, erase = 0;
	enum IND_COLORS curr = DEFAULT_INITIAL_COLOR;
	enum IND_COLORS swap = DEFAULT_SWAP_COLOR;
	
	assert(dp != NULL && "Need a display to run the app");
	assert(src != -1 && "Need a display to run the app");
	
	KeySym keysym;
#if RESIZE_PIXMAP
	Pixmap pixm;
	int first = 0;
#else
	
#endif
	
	while (XNextEvent(dp, &ev) == 0) {
		int width, height;
		win_size(main_win, &width, &height);
		
		switch (ev.type) {
		case ButtonPress:
			if (ev.xbutton.button == Button1) {
				LOG("Drawing %i - %i", ev.xbutton.x, ev.xbutton.y);
				XDrawPoint(dp, ev.xbutton.window, gc, ev.xbutton.x, ev.xbutton.y);
			}
			holding = 1;
			break;
			
		case ButtonRelease:
			holding = 0;
			break;
			
		case MotionNotify:
			if (holding)
				XDrawLine(dp, ev.xbutton.window, gc, prev_x, prev_y,
					  ev.xbutton.x, ev.xbutton.y);
			prev_x = ev.xbutton.x;
			prev_y = ev.xbutton.y;
			break;

		case LeaveNotify:
			LOG("User leave for the moment...");
			break;

#if RESIZE_PIXMAP
		case ConfigureNotify:
			LOG("Resizing %i - %i", width, height);
			if (!first)
				XCopyArea(dp, pixm, main_win, gc, 0, 0, width, height, 0, 0);
			XSetBackground(dp, gc, WhitePixel(dp, src));
			break;
#else
		case ResizeRequest:
			if (width != ev.xresizerequest.width || height != ev.xresizerequest.height) {
				LOG("Resizing %i - %i", ev.xresizerequest.width, ev.xresizerequest.height);
				// XResizeWindow(dp, main_win, ev.xresizerequest.width, ev.xresizerequest.height);
				XWindowChanges changes = {
					.width= ev.xresizerequest.width,
					.height = ev.xresizerequest.height
				};
				XConfigureWindow(dp, main_win, CWWidth | CWHeight, &changes);
				win_size(main_win, &width, &height);
				LOG("Updated size %i - %i", width, height);
			}
			break;
#endif
			
		case KeyPress:
			/* TODO: Create a switch here */
			keysym = XkbKeycodeToKeysym(dp, ev.xkey.keycode, 0, 0);
			if (keysym == XK_equal) {
				if (gc_vals.line_width < 100) {
					int lw = gc_vals.line_width;
					XFreeGC(dp, gc);
					create_graphics_contex(&COLORS[curr], lw + 1);
					LOG("Increasing pincel size %i", gc_vals.line_width);
				}
			} else if (keysym == XK_minus) {
				if (gc_vals.line_width > 0) {
					int lw = gc_vals.line_width;
					XFreeGC(dp, gc);
					create_graphics_contex(&COLORS[curr], lw - 1);
					LOG("Decreasing pincel size %i", gc_vals.line_width);
				}
			} else if (keysym == XK_c) {
				XClearWindow(dp, main_win);
				LOG("Cleaing the screen");
			} else if (keysym == XK_s) {
				LOG("Saving draw");
				
			} else if (keysym == XK_x) {
				LOG("Toglecolor color to %s", color_name(swap));
				XSetForeground(dp, gc, COLORS[swap].pixel);
				SWAP(curr, swap);
			} else if (keysym == XK_e) {
				int lw = gc_vals.line_width;
				if (erase) {
					LOG("Erase mode deactive");
					XFreeGC(dp, gc);
					create_graphics_contex(&COLORS[curr], lw / 4);
					
				} else {
					LOG("Erase mode active");
					XFreeGC(dp, gc);
					create_graphics_contex(&COLORS[WHITE], lw * 4);
				}
				
				erase = !erase;
			} else if (keysym == XK_q)
				return;
		}

#if RESIZE_PIXMAP
		if (!first)
			XFreePixmap(dp, pixm); /* Drop the prev */
		/* Catch the new */
		pixm = XCreatePixmap(dp, main_win, width, height, DefaultDepth(dp, src));
		XCopyArea(dp, main_win, pixm, gc, 0, 0, width, height, 0, 0);
		first = 0;
#endif
	
	}
}


int main(void)
{
	open_display();
	
	LOG("Display opened");
	main_win = create_window(DEFAULT_POSX, DEFAULT_POSY, DEFAULT_MIN_WIDTH,
				 DEFAULT_MIN_HEIGHT, DEFAULT_BORDER_WIDTH, PROG_NAME);
	LOG("Window id %lu", (unsigned long)  main_win);
	LOG("Window created");
	create_colors();

	create_graphics_contex(&COLORS[DEFAULT_INITIAL_COLOR], DEFAULT_LINE_WIDTH);
	LOG("Creating Graphics and Colors");
	LOG("Running the app");
	run();
	
	LOG("Finishing closing everything");
	
	XFreeGC(dp, gc);
	free_colors();
	close_display(main_win);
	LOG("Programn finished");
	
	return 0;
}



