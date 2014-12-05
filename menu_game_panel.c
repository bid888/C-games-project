#include "menu_game_panel.h"
#include "menu_difficulties.h"
#include "menu_saveload.h"
#include "game.h"
#include "tic_tac_toe.h"

#define BUTTON_BASE 70
#define BUTTON_ROW_HEIGHT 80

/* NEW GAME PANEL - all the related functions of the game panel */

//on restatrt click use the restart func
static void on_restart_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_restart();
    //printf("restart\n");
}

//on save click saves the game
static void on_save_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_control* save_dlg = save_game_menu_create();
    gui_child_push(window, save_dlg);
}
//return to main menu
static void on_main_menu_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_quit_to_main_menu(window);
}
//quit the game
static void on_quit_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_quit(window);
}
//choose a different difficulty for player 1
static void on_difficulty1_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game* current_game = (game*)target->state.pstate;
    gui_control* diff_panel = menu_difficulties_create(current_game, 1);
    gui_child_push(window, diff_panel);
}
//choose difficulty for player 2
static void on_difficulty2_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game* current_game = (game*)target->state.pstate;
    gui_control* diff_panel = menu_difficulties_create(current_game, 2);
    gui_child_push(window, diff_panel);
}

//show the 'click on spce for nect move' message
void menu_game_panel_show_space_warning(gui_control* side_menu) {
    gui_control* press_for_next_move = gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT * 5, "images/space_for_next_move.png",   NULL);
    gui_child_add(side_menu, press_for_next_move);
    side_menu->state.pstate = press_for_next_move;
}
//hide space message when the player moves, only when the AI is the current player
void menu_game_panel_hide_space_warning(gui_control* side_menu) {
    gui_control* press_for_next_move = (gui_control*)side_menu->state.pstate;
    if (press_for_next_move != NULL) {
        gui_child_remove(press_for_next_move);
        gui_release_tree(press_for_next_move);
        side_menu->state.pstate = NULL;
    }
}

//creates the game panel with all the buttons
gui_control* menu_game_panel_create(int player_types[], game* current_game) {
    gui_control* menu_panel = gui_panel_create(GAME_WIDTH, 0, WIN_W - GAME_WIDTH, WIN_H, NULL);

    gui_control* restart =      gui_button_create(45, BUTTON_BASE,                "images/restart.png",       &on_restart_click);
    gui_control* save =         gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT,       "images/save.png",          &on_save_click);
    gui_control* main_menu =    gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT * 2,   "images/main_menu.png",     &on_main_menu_click);

    gui_control* diff1 =        gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT * 3,   "images/diff1.png",          &on_difficulty1_click);
    gui_control* diff2 =        gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT * 4,   "images/diff2.png",          &on_difficulty2_click);

    gui_control* quit =         gui_button_create(45, BUTTON_BASE + BUTTON_ROW_HEIGHT * 6,   "images/quit.png",          &on_quit_click);

    diff1->state.pstate = current_game;
    diff2->state.pstate = current_game;

    gui_set_name(menu_panel, "game sidepanel");

    if (restart == NULL || save == NULL || main_menu == NULL || diff1 == NULL || diff2 == NULL || quit == NULL) {
        printf("ERROR: failed to load image: '%s'\n", gui_last_error());
        return NULL;
    }

    //add child controls to the window
    gui_child_add(menu_panel, restart);
    gui_child_add(menu_panel, save);
    gui_child_add(menu_panel, main_menu);

    if (player_types[1] == PLAYER_AI)
        gui_child_add(menu_panel, diff1);

    if (player_types[2] == PLAYER_AI)
        gui_child_add(menu_panel, diff2);

    gui_child_add(menu_panel, quit);

    return menu_panel;
}
