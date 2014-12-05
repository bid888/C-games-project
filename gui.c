#include <stdlib.h>
#include <assert.h>
#include "SDL_image.h"
#include "gui.h"

static linked_list* keyboard_handlers = NULL;
static const char* gui_error;
static int quit = 0;

//get last gui error sting
const char* gui_last_error() {
    return gui_error;
}

//set last gui error string
void gui_set_error(char* format, ...) {
    static char buf[1000];
    va_list argp;
    va_start(argp, format);
    vsprintf(buf, format, argp);
    va_end(argp);
    gui_error = buf;
}

//allocate and initialize a new gui_control struct
gui_control* gui_new_control(gui_size w, gui_size h, gui_size x, gui_size y) {
    gui_control* ctl = (gui_control*)malloc(sizeof(gui_control));
	if (ctl == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    ctl->x = x; ctl->y = y;
    ctl->w = w; ctl->h = h;
    ctl->surface = NULL;
    ctl->children = NULL;
    ctl->parent = NULL;
    ctl->onclick = NULL;
    ctl->name = NULL;
    ctl->nav_stack = NULL;

    ctl->state.istate = UNDEFINED;
    ctl->state.fstate = UNDEFINED;
    ctl->state.pstate = NULL;

    return ctl;
}

//release a tree of controls recursively
void gui_release_tree(gui_control* ctl) {
    if (ctl->surface)
        SDL_FreeSurface(ctl->surface);

    if (ctl->nav_stack)
        stack_release(ctl->nav_stack);

    if (ctl->children) {
        linked_list_node *p = ctl->children->head;
        while (p != NULL) {
            gui_release_tree((gui_control*)p->item);
            p = p->next;
        }
        linked_list_release(ctl->children);
    }

    free(ctl);
}

//render a tree of controls recursively onto target surface
void gui_render_tree(gui_control* node, gui_size offset_x, gui_size offset_y, SDL_Surface* target) {
    //render if this is not the window itself, and there's a surface to render
    if (node->parent && node->surface) {
        //Make a temporary rectangle to hold the offsets
        SDL_Rect offset;

        //Give the offsets to the rectangle
        offset.x = offset_x + node->x;
        offset.y = offset_y + node->y;

        //Blit the surface
        if (SDL_BlitSurface(node->surface, NULL, target, &offset) != 0) {
            printf("ERROR: failed to blit image: %s\n", SDL_GetError());
            exit(-1);
        }
    }
    if (node->children) {
        gui_size node_offset_x = offset_x + node->x;
        gui_size node_offset_y = offset_y + node->y;

        linked_list_node *p = node->children->head;
        while (p != NULL) {
            gui_render_tree((gui_control*)p->item, node_offset_x, node_offset_y, target);
            p = p->next;
        }
    }
}

// checks a control tree recursively to find the click location.
int gui_click_tree(gui_control* node, gui_control* window, gui_size x, gui_size y) {

    //if the click was inside the control
    if (node->onclick &&
        (node->x <= x && x <= node->x + node->w) &&
        (node->y <= y && y <= node->y + node->h))
    {
        //call the click handler. x and y are relative to control root.
        node->onclick(node, window, x - node->x, y - node->y);
        return 1;
    }

    if (node->children) {
        linked_list_node *p = node->children->head;
        while (p != NULL) {
            if (gui_click_tree((gui_control*)p->item, window, x - node->x, y - node->y))
                return 1;

            p = p->next;
        }
    }

    return 0;
}

//calls keyboard handlers registered for event with the event info
void dispatch_keyboard_event(SDL_KeyboardEvent* key) {
    linked_list_node* p = keyboard_handlers->head;
    opaque_fpointer_t* fpointer_wrapper = NULL;
    gui_keyboard_handler handler = NULL;

    while (p != NULL) {
        //see fpointer_wrapper for why we do this...
        fpointer_wrapper = p->item;
        handler = (gui_keyboard_handler)fpointer_wrapper->fp;
        handler(key);
        p = p->next;
    }
}

/*****************************
      start public api
*****************************/

//create the main application window
gui_control* gui_window_create(gui_size w, gui_size h, const char* caption) {
    SDL_Surface *surface;
	gui_control* ctl;

    /* make new window centered */
    putenv("SDL_VIDEO_WINDOW_POS");
    putenv("SDL_VIDEO_CENTERED=1");

    getenv("SDL_VIDEO_WINDOW_POS");
    getenv("SDL_VIDEO_CENTERED");

    // Initialize SDL and make sure it quits
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        gui_set_error("ERROR: failed to set video mode: %s\n", SDL_GetError());
        return NULL;
    }
    atexit(SDL_Quit);

    //Load images
    SDL_WM_SetCaption(caption, caption);

    surface = SDL_SetVideoMode(w, h, 0, SDL_HWSURFACE | SDL_DOUBLEBUF);
    if (surface == NULL) {
        gui_set_error("ERROR: failed to set video mode: %s\n", SDL_GetError());
        return NULL;
    }

    ctl = gui_new_control(w, h, UNDEFINED, UNDEFINED);
    ctl->surface = surface;
    ctl->children = linked_list_create();
    ctl->nav_stack = stack_create();
    return ctl;
}

//create a new button control in position x,y, with background image specified by image_path. onclick is an optional function pointer for
//a handler to call on click
gui_control* gui_button_create(gui_size x, gui_size y, const char* image_path, gui_click_handler onclick) {
    SDL_Surface *surface = IMG_Load(image_path);
    gui_control* ctl;

	if (surface == NULL) {
        gui_set_error("ERROR: failed loading image: %s\n", image_path);
        return NULL;
    }

    ctl = gui_button_create_from_surface(x, y, surface, onclick);
    return ctl;
}

//create a control from a surface
gui_control* gui_button_create_from_surface(gui_size x, gui_size y, SDL_Surface* surface, gui_click_handler onclick) {
    gui_control* ctl = gui_new_control(surface->w, surface->h, x, y);
    ctl->surface = surface;
    ctl->onclick = onclick;
    return ctl;
}

//create a panel gui control - meant to contain child elements.
gui_control* gui_panel_create(gui_size x, gui_size y, gui_size w, gui_size h, const char* image_path) {
    SDL_Surface *surface = NULL;
    gui_control* ctl;
	if (image_path) {
        surface = IMG_Load(image_path);
        if (surface == NULL) {
            gui_set_error("ERROR: failed loading image: %s\n", image_path);
            return NULL;
        }
    }

    ctl = gui_new_control(w, h, x, y);
    ctl->surface = surface;
    ctl->children = linked_list_create();
    return ctl;
}

//add a child control to a parent
void gui_child_add(gui_control* parent, gui_control* child) {
    assert(parent->children != NULL);
    assert(child->parent == NULL);
    linked_list_addLast(parent->children, child);
    child->parent = parent;
}

//remove a child control from its parent
void gui_child_remove(gui_control* child) {
    assert(child != NULL);
    if (!child->parent) return;
    assert(child->parent->children != NULL);
    linked_list_remove_item(child->parent->children, (void*)child);
    child->parent = NULL;
}

//quit the main UI loop started by gui_main
void gui_quit() {
    quit = 1;
}

//register a new keyboard handler to be called on keyboard events
void gui_keyboard_register(gui_keyboard_handler kbhandler) {
    //see fpointer_wrapper definition for why we do this
    opaque_fpointer_t* fpointer_wrapper = malloc(sizeof(opaque_fpointer_t));
    if (fpointer_wrapper == NULL) {
        printf("ERROR : Not enough memory");
        exit(1);
    }
    fpointer_wrapper->fp = (void(*)(void))kbhandler;
    linked_list_addLast(keyboard_handlers, fpointer_wrapper);
}

//start the main ui loop, polling for input events and rendering the UI tree on screen
void gui_main(gui_control* window) {
    SDL_Event e;
    int x,y;
    quit = 0;

    while (!quit) {
        SDL_Delay(100);

        // Clear window to BLACK
		if (SDL_FillRect(window->surface, 0, SDL_MapRGB(window->surface->format, 0, 0, 0)) != 0) {
            printf("ERROR: failed to draw rect: %s\n", SDL_GetError());
            break;
        }

        gui_render_tree(window, 0, 0, window->surface);

        // We finished drawing
        if (SDL_Flip(window->surface) != 0) {
            printf("ERROR: failed to flip buffer: %s\n", SDL_GetError());
            break;
        }

        //Poll for keyboard & mouse events
        while (SDL_PollEvent(&e) != 0) {

            // If a quit event has been sent
            if (e.type == SDL_QUIT) {
                quit = 1;
            }

             /* If a button on the mouse is released. */
            if (e.type == SDL_MOUSEBUTTONUP) {
                /* If the left button was released */

                if (e.button.button == SDL_BUTTON_RIGHT){
                    printf("Please use the left mouse button");
                }

                if (e.button.button == SDL_BUTTON_LEFT){
                    x = e.button.x;
                    y = e.button.y;

                    gui_click_tree(window, window, x, y);
                }
            }

            if (e.type == SDL_KEYDOWN) {
                dispatch_keyboard_event(&e.key);
            }
        }
    }
}

//set a name to a gui control - mainly meant for debugging
void gui_set_name(gui_control* ctl, const char* name) {
    assert(ctl != NULL);
    ctl->name = name;
}

//the navigation between windows is treated as a stack - push the child onto the stack and display it
void gui_child_push(gui_control* window, gui_control* child) {
    gui_control* last_child;
	assert(window->nav_stack);
    last_child = (gui_control*)stack_peep(window->nav_stack);

    if (last_child != NULL)
        gui_child_remove(last_child);

    gui_child_add(window, child);
    stack_push(window->nav_stack, child);
}

//the navigation between windows is treated as a stack - pop the last child from the stack, display the item above it
gui_control* gui_child_pop(gui_control* window) {
    gui_control* child = (gui_control*)stack_pop(window->nav_stack);
    gui_control* prev_child = (gui_control*)stack_peep(window->nav_stack);

    gui_child_remove(child);
    if (prev_child)
        gui_child_add(window, prev_child);

    return child;
}

//the navigation between windows is treated as a stack - pop all children except last - reach the main menu...
void gui_child_popall(gui_control* window) {
    gui_control* child = NULL;
    gui_control* prev_child = (gui_control*)stack_peep(window->nav_stack);

    while (prev_child) {
        child = (gui_control*)stack_pop(window->nav_stack);
        prev_child = (gui_control*)stack_peep(window->nav_stack);

        if (child->parent)
            gui_child_remove(child);

        //if the child we popped was the last
        if (prev_child == NULL) {
            //re-add it back
            gui_child_push(window, child);
            break;
        }

        gui_release_tree(child);
    }
}

//initialize the gui module - allocate any data structures needed
void gui_init() {
    keyboard_handlers = linked_list_create();
}

//destroy and release and data structures
void gui_release() {
    linked_list_release(keyboard_handlers);
    keyboard_handlers = NULL;
}
