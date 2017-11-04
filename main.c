#include <SDL2/SDL.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <signal.h>

#define plog(...) do {fprintf(stderr, __VA_ARGS__); fputc('\n', stderr);} while(0)
#define perr(...) do {plog(__VA_ARGS__); exit(1);} while(0)

// Simulate mouse click
void click (Display *display, int button) {
  // Create and setting up the event
  XEvent event;
  memset (&event, 0, sizeof (event));
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (display);
  while (event.xbutton.subwindow)
    {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer (display, event.xbutton.window,
		     &event.xbutton.root, &event.xbutton.subwindow,
		     &event.xbutton.x_root, &event.xbutton.y_root,
		     &event.xbutton.x, &event.xbutton.y,
		     &event.xbutton.state);
    }
  // Press
  event.type = ButtonPress;
  if (XSendEvent (display, PointerWindow, True, ButtonPressMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
}

void coords (Display *display, int *x, int *y) {
  XEvent event;
  XQueryPointer (display, DefaultRootWindow (display),
                 &event.xbutton.root, &event.xbutton.window,
                 &event.xbutton.x_root, &event.xbutton.y_root,
                 &event.xbutton.x, &event.xbutton.y,
                 &event.xbutton.state);
  *x = event.xbutton.x;
  *y = event.xbutton.y;
}

void unclick (Display *display, int button) {
  // Create and setting up the event
  XEvent event;
  memset (&event, 0, sizeof (event));
  event.xbutton.button = button;
  event.xbutton.same_screen = True;
  event.xbutton.subwindow = DefaultRootWindow (display);
  while (event.xbutton.subwindow)
    {
      event.xbutton.window = event.xbutton.subwindow;
      XQueryPointer (display, event.xbutton.window,
		     &event.xbutton.root, &event.xbutton.subwindow,
		     &event.xbutton.x_root, &event.xbutton.y_root,
		     &event.xbutton.x, &event.xbutton.y,
		     &event.xbutton.state);
    }
  // Release
  event.type = ButtonRelease;
  if (XSendEvent (display, PointerWindow, True, ButtonReleaseMask, &event) == 0)
    fprintf (stderr, "Error to send the event!\n");
  XFlush (display);
}

// Move mouse pointer (absolute)
void move_to (Display *display, int x, int y) {
  int cur_x, cur_y;
  coords (display, &cur_x, &cur_y);
  XWarpPointer (display, None, None, 0,0,0,0, -cur_x, -cur_y);
  XWarpPointer (display, None, None, 0,0,0,0, x, y);
}

int main(int argc, char** argv) {
	SDL_Joystick *joy;

	// Initialize the joystick subsystem
	SDL_InitSubSystem(SDL_INIT_JOYSTICK);

	// Check for joystick
	int joysticks = SDL_NumJoysticks();
	if (joysticks > 0) {
		for (int j = 0; j < joysticks; j++) {
			joy = SDL_JoystickOpen(j);

			if (joy) {
				plog("joystick %d:", j);
				plog("\tName: %s", SDL_JoystickNameForIndex(0));
				plog("\tNumber of Axes: %d", SDL_JoystickNumAxes(joy));
			} else {
				plog("Couldn't open Joystick %i", j);
			}

			// Close if opened
			if (SDL_JoystickGetAttached(joy)) {
				SDL_JoystickClose(joy);
			}
		}
		int n = -1;
		
		if (argc < 2) {
			printf("Which one: ");
			scanf("%d", &n);
		} else
			sscanf(argv[1], "%d", &n);
		plog("chosed %i", n);
		joy = SDL_JoystickOpen(n);

/*
		int xaxis, yaxis;
		printf("input x axis, y axis: ");
		scanf("%d, %d", &xaxis, &yaxis);
*/
		if (!SDL_JoystickGetAttached(joy)) {
			perr("cant open joystick nº %d", n);
		}
		
		Display *dpy = XOpenDisplay (NULL);
        Screen* pscr = NULL;
        pscr = DefaultScreenOfDisplay(dpy);
        if (!pscr) {
                XCloseDisplay(dpy);
                perr("X11 fail to get screen of display");
        }

		uint8_t stop = 0;
		void sigint(int s) { stop = 1; }
		signal(SIGINT, sigint);
			
		#define scale 1

		int c_up = 0;

		while (!stop) {
			SDL_JoystickUpdate();

			#define C_UP_MIN 5000

			int cstickvert = SDL_JoystickGetAxis(joy,2);
			if (!c_up && cstickvert < -C_UP_MIN) {
				//c-up
				c_up = 1;
				move_to(dpy, 1630, 815);
//				SDL_Delay(50);
				click(dpy,1);
				SDL_Delay(20);
				unclick(dpy,1);

			} else if (c_up && cstickvert > -C_UP_MIN) {
				c_up = 0;
			}

			if (SDL_JoystickGetButton(joy, 7)) {
				//II item
				move_to(dpy, 1890, 990);
				click(dpy,1);
				SDL_Delay(20);
				unclick(dpy,1);
			}
			if (SDL_JoystickGetButton(joy, 9)) {
				//"items"
				move_to(dpy, 1810, 1008);
				click(dpy,1);
				SDL_Delay(20);
				unclick(dpy,1);
			}
			if (SDL_JoystickGetButton(joy, 12)) {
				//orcarina
				move_to(dpy, 1620, 990);
				click(dpy,1);
				SDL_Delay(20);
				unclick(dpy,1);
			}
//			SDL_Delay(1000.0/60.0);
		}
				
		plog("quitting");
		
		unclick(dpy, 1);
        XCloseDisplay(dpy);
		SDL_JoystickClose(joy);

	} else perr("no joysticks");

	return 0;
}
