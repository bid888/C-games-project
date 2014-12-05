#include <assert.h>
#include "menu_main.h"
#include "menu_newgame.h"
#include "menu_saveload.h"

/* Create the main menu of the game! */
//set the game on new game click
static void on_new_game_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_control* new_game_panel = new_game_menu_create();
    gui_child_push(window, new_game_panel);
}

//load the game on new load game
static void on_load_game_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_control* load_game_panel = load_game_menu_create();
    gui_child_push(window, load_game_panel);
}

//On quit click - quit!
static void on_quit_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    //printf("quit !\n");
    gui_quit();
}

/*** begin public api ***/

//creates the main window with the title, new game button, load gmae and quit
gui_control* menu_main_create() {
    gui_control *menu_panel = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);

    //Load images
    gui_control *thegamesprogram    = gui_button_create((WIN_W -330)/2, 100,    "images/thegamesprogram.png",   NULL);
	gui_control *newgame            = gui_button_create((WIN_W -230)/2, 210,   "images/newgame.png",           &on_new_game_click );
    gui_control *load               = gui_button_create((WIN_W -230)/2, 300,  "images/loadgame.png",          &on_load_game_click );
    gui_control *quitimg            = gui_button_create((WIN_W -230)/2, 390,   "images/quit.png",              &on_quit_click);

    if (thegamesprogram == NULL || newgame == NULL || load == NULL || quitimg==NULL) {
        printf("ERROR: failed to load image: '%s'\n", gui_last_error());
        exit(1);
    }

    gui_set_name(menu_panel, "main menu");

    //add child controls to the window
    gui_child_add(menu_panel, thegamesprogram);
    gui_child_add(menu_panel, newgame);
    gui_child_add(menu_panel, load);
    gui_child_add(menu_panel, quitimg);

    return menu_panel;
}
