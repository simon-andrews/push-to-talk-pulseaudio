#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>

Display *display;
Window window;
XEvent event;

char pacmd_command_mute[100];
char pacmd_command_unmute[100];

// Initialize a connection to the X server (if we're not already connected).
void connect_to_x_server()
{
  // First, check that the connection isn't already initialized. If it is, don't
  // do anything.
  if (display != NULL)
    return;
  
  // Since the connection isn't initialized, try to connect.
  display = XOpenDisplay(NULL);

  // If we fail to connect, display will be NULL. Detect this and report an
  // error if needed. TODO: report what actually went wrong.
  assert(display != NULL);
}

void create_window()
{
  // This function expects that we have successfully connected to an X server,
  // so check that assumption.
  assert(display != NULL);

  // Create a new window, which will be a 200px by 200px red square by default.
  window = XCreateSimpleWindow(
    /* display: */ display,
    /* parent window: */ RootWindow(display, 0),
    /* x position: */ 1,
    /* y position: */ 1,
    /* width: */ 200,
    /* height: */ 200,
    /* border width: */ 0,
    /* border: */ BlackPixel(display, 0),
    /* background: */ BlackPixel(display, 0));

  // Associate key press and key release events with the window we just created.
  XSelectInput(display, window, KeyPressMask | KeyReleaseMask);
  
  // Mark the window we created as eligible for displaying.
  XMapWindow(display, window);

  // Flush the output buffer, to make the window display.
  XFlush(display);
}

void set_window_color(char color)
{
  // First, check that we're actually connected to an X server and that the
  // color the user passed in is something we expect.
  assert(display != NULL);
  assert(color == 'b' || color == 'w');

  // Use macros from Xlib to turn the user's color preference into a pixel
  // color that X can work with.
  unsigned long pixel;
  if (color == 'b')
    pixel = BlackPixel(display, 0);
  else
    pixel = WhitePixel(display, 0);

  // Update the background color for the current window using this new
  // X-friendly color.
  XSetWindowBackground(display, window, pixel);

  // Now that the update has been done, draw it so that the user can see the
  // change.
  XClearWindow(display, window);
}

bool is_retrigger(XEvent *current_event)
{
  // Initialize a flag, to be returned at the end of this function.
  bool is_retrigger = false;

  // If there are events in the event queue, it's possible that the next event
  // is the same as the current event, meaning that the current event is a
  // retrigger.
  if (XEventsQueued(display, QueuedAfterReading)) {

    // Get the next queued event from the server.
    XEvent next_event;
    XPeekEvent(display, &next_event);

    // Check if the next event on the queue and the current event are the same.
    // If they are, skip ahead to that event.
    if (next_event.type == KeyPress &&
	next_event.xkey.time == current_event->xkey.time &&
	next_event.xkey.keycode == current_event->xkey.keycode) {
      // The next event on the queue is the same as the current event. Update
      // the current event to this new event and set a flag.
      XNextEvent(display, current_event);
      is_retrigger = true;
    }
  }

  // Now that we've determined whether the initial current event was a retrigger
  // or not, return that fact.
  return is_retrigger;
}

void handle_input()
{
  // Get an event from the X server, and store it in our buffer event, and get
  // the key associated with the event.
  XNextEvent(display, &event);
  long key = (long) XLookupKeysym(&event.xkey, 0);

  switch (event.type) {
    // When a key is pressed, unmute Zoom's PulseAudio stream and set the window
    // color to white.
    case KeyPress:
      if (key == 32) {
	system(pacmd_command_unmute);
	set_window_color('w');
      }
      break;

    // When space is physically released (not a retrigger release), mute Zoom's
    // PulseAudio stream again and reset the window color to black.
    case KeyRelease:
      if (key == 32 && !is_retrigger(&event)) {
	system(pacmd_command_mute);
	set_window_color('b');
      }
      break;
  }
}

int main (int argc, char *argv[])
{
  if (argc != 2) {
    fprintf(stderr, "incorrect # of arguments\n");
    return EXIT_FAILURE;
  }

  int id = atoi(argv[1]);
  snprintf(pacmd_command_mute, 100, "pacmd set-source-output-mute %d 1", id);
  snprintf(pacmd_command_unmute, 100, "pacmd set-source-output-mute %d 0", id);
  
  connect_to_x_server();
  create_window();

  while (true)
  {
    handle_input();
  }
  
  return EXIT_SUCCESS;
}
