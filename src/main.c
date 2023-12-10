#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdbool.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>

#include "../include/window.h"
#include "../include/graphics.h"
#include "../include/colors.h"
#include "../include/tools.h"
#include "../include/canvas.h"
#include "../include/utils.h"
#include "../include/kbinds.h"

extern bool accept_event(Tool *tool, XEvent *ev);

void main_loop(void)
{
	XEvent event;
	Tool tool = {0};
	
	assert(dp != NULL && "Need a display to run the app");
	assert(src != -1 && "Need a display to run the app");
	
	init_tool(&tool);
	sel_tool(&tool, DEFAULT_INITIAL_TOOL);
	
	KeySym keysym;
	int quit = 0;
	while (!quit) {
		XNextEvent(dp, &event);

        if(accept_event(&tool, &event))
            continue;

        printf("> %x\n", STATE(&tool)->holding);
		switch (event.type) {
		case MotionNotify:
			if (STATE(&tool)->holding) {
                printf("Still drawin!");
				tool_draw(&tool, event.xbutton.x, event.xbutton.y);
            }
			break;

		case LeaveNotify:
			LOG("User leave for the moment");
			break;
			
		case Expose:
			LOG("User arrive");
			break;

		case ConfigureNotify:
			assert(event.xconfigure.width >= DEFAULT_MIN_WIDTH
			       && event.xconfigure.height >= DEFAULT_MIN_HEIGHT
			       && "Checking the configuration");
			if (event.xconfigure.width != canv_width || event.xconfigure.height != canv_height)
				resize_canvas(event.xconfigure.width, event.xconfigure.height);
			break;

		case ClientMessage:
			if ((Atom) event.xclient.data.l[0] == wm_delete_window)
				quit = 1;
			break;
			
		case KeyPress:
			keysym = XkbKeycodeToKeysym(dp, event.xkey.keycode, 0, 0);
			handle_keypress(keysym, &tool, &quit);
			break;
		}

		/* Refresh the canvas */
		if (event.type == ButtonRelease
		    || event.type == Expose
		    || (event.type == KeyPress && keysym == XK_c))
			refresh_canvas();
	}

	/* Free the tools */
	free_tool(&tool);
}
