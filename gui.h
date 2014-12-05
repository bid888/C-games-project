#ifndef _GUI_H
#define _GUI_H

#define NO_STDIO_REDIRECT /* otherwise STL swallows output */

#include <SDL.h>
#include <SDL_video.h>
#include "linked_list.h"
#include "stack.h"

#define UNDEFINED -1

typedef unsigned int gui_size;

typedef union
{
   int istate;
   float fstate;
   void* pstate;
} ustate;

//window, panel, label/image, and button
typedef struct gui_control {
	gui_size x;
	gui_size y;
	gui_size w;
	gui_size h;

	linked_list *children;
	struct gui_control *parent;

	SDL_Surface *surface;
	void (*onclick)(struct gui_control*, struct gui_control*, gui_size, gui_size);

	const char* name;
	stack* nav_stack;	//only relevant for window
	ustate state;		//generic state
} gui_control;


// x and y are relative to control root.
typedef void (*gui_click_handler)(gui_control* target, gui_control* window, gui_size x, gui_size y);
typedef void (*gui_keyboard_handler)(SDL_KeyboardEvent *key);

gui_control* gui_window_create(gui_size w, gui_size h, const char* caption);
gui_control* gui_button_create(gui_size x, gui_size y, const char* image_path, gui_click_handler onclick);
gui_control* gui_button_create_from_surface(gui_size x, gui_size y, SDL_Surface* surface, gui_click_handler onclick);
gui_control* gui_panel_create(gui_size x, gui_size y, gui_size w, gui_size h, const char* image_path);
void gui_child_add(gui_control* parent, gui_control* child);
void gui_child_remove(gui_control* child);

// remove child from its parent
void gui_release_tree(gui_control* ctl);

// for navigation purposes
void gui_child_push(gui_control* window, gui_control* child);
gui_control* gui_child_pop(gui_control* window);
void gui_child_popall(gui_control* window);

// register for keyboard events
void gui_keyboard_register(gui_keyboard_handler kbhandler);

// main rendering
void gui_main(gui_control* window);
void gui_quit();
const char* gui_last_error();

// set control name for easier debugging
void gui_set_name(gui_control* ctl, const char* name);

void gui_init();
void gui_release();

#endif
