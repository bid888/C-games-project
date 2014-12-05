#include "menu_newgame.h"
#include "game.h"
#include "tic_tac_toe.h"
#include "connect4.h"
#include "menu_players.h"
#include "reversi.h"

/*creates the game choose menu */

//show the players menu - AI vs AI, Player vs PLayer...
static void show_players_menu(gui_control* window) {
    gui_control* players_menu = menu_players_create();
    gui_child_push(window, players_menu);
}
//on back click return to previous menu
static void on_back_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_child_pop(window);
}
//shows player menu for each game selection - tictactoe, connect4, reversi
static void on_tic_tac_toe_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_next_game(game_tic_tac_toe);
    show_players_menu(window);
}

static void on_connect4_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_next_game(game_connect4);
    show_players_menu(window);
}

static void on_reversi_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_next_game(game_reversi);
    show_players_menu(window);
}
//create the 'choose game' panel 
gui_control* new_game_menu_create() {
    gui_control *menu_panel = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);

    //Load images
    int base = 100;
    int height = 80;
    gui_control *tic = gui_button_create((WIN_W -230)/2, base, "images/tic_tac_toe.png", &on_tic_tac_toe_click);
    gui_control *connect4 = gui_button_create((WIN_W -230)/2, base + height,  "images/connect4.png", &on_connect4_click);
    gui_control *reversi = gui_button_create((WIN_W -230)/2, base + height * 2, "images/reversi.png", &on_reversi_click);

    gui_control *back = gui_button_create((WIN_W -230)/2, base + height * 3.5, "images/back.png", &on_back_click);

    gui_set_name(menu_panel, "new game menu");

    if (tic == NULL || connect4 == NULL || reversi == NULL) {
        printf("ERROR: failed to load image: '%s'\n", gui_last_error());
        exit(1);
    }

    //add child controls to the window
    gui_child_add(menu_panel, tic);
    gui_child_add(menu_panel, connect4);
    gui_child_add(menu_panel, reversi);
    gui_child_add(menu_panel, back);

    return menu_panel;
}
