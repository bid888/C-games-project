#include <stdio.h>
#include <assert.h>
#include "gui.h"
#include "game.h"
#include "menu_main.h"
#include "tic_tac_toe.h"
#include "connect4.h"
#include "reversi.h"

/* GAMESPROG - the main function to start all the program */
int main(int argc, char** argv) {
	//create the main window for the program
	gui_control *main_menu;
    gui_control *window = gui_window_create(WIN_W, WIN_H, "games program");
    if (!window) {
        printf("error creating window, gui_last_error was '%s'\n", gui_last_error());
        return -1;
    }

    main_menu = menu_main_create();

    if (!main_menu) {
        printf("error creating main_menu, gui_last_error was '%s'\n", gui_last_error());
        return -1;
    }

	//initiate all the games and release the memory at the end
    gui_init();
    game_init();

    tic_tac_toe_init();
	connect4_init();
    reversi_init();

    gui_child_push(window, main_menu);
    gui_main(window);

    tic_tac_toe_release();
	connect4_release();
    reversi_release();

    game_release();
    gui_release();


    return 0;
}


