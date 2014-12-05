#include "game.h"

/* MENU PLAYERS - creates the panel and manages all the possible players selection*/

static void on_player_vs_player_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_players(PLAYER_HUMAN, PLAYER_HUMAN);
    game_start(window);
}

static void on_player_vs_ai_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_players(PLAYER_HUMAN, PLAYER_AI);
    game_start(window);
}

static void on_ai_vs_player_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_players(PLAYER_AI, PLAYER_HUMAN);
    game_start(window);
}

static void on_ai_vs_ai_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    game_set_players(PLAYER_AI, PLAYER_AI);
    game_start(window);
}

static void on_back_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_child_pop(window);
}
//creates the panel to selecet the players
gui_control* menu_players_create() {
    int base = 100;
    int height = 80;
	gui_control *menu_panel = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);

    //Load images
    // Player vs. Player, Player vs. AI, AI vs. Player, and AI vs. AI.
    gui_control *player_vs_player = gui_button_create((WIN_W -230)/2, base,                "images/player_vs_player.png",  &on_player_vs_player_click);
    gui_control *player_vs_ai =     gui_button_create((WIN_W -230)/2, base + height,       "images/player_vs_ai.png",      &on_player_vs_ai_click );
    gui_control *ai_vs_player =     gui_button_create((WIN_W -230)/2, base + height * 2,   "images/ai_vs_player.png",      &on_ai_vs_player_click );
    gui_control *ai_vs_ai =         gui_button_create((WIN_W -230)/2, base + height * 3,   "images/ai_vs_ai.png",          &on_ai_vs_ai_click);
    gui_control *back =             gui_button_create((WIN_W -230)/2, base + height * 4.5,   "images/back.png",              &on_back_click);

	gui_set_name(menu_panel, "players menu");

    if (player_vs_player == NULL || player_vs_ai == NULL || ai_vs_player == NULL || ai_vs_ai == NULL) {
        printf("ERROR: failed to load image: '%s'\n", gui_last_error());
        exit(1);
    }

    //add child controls to the window
    gui_child_add(menu_panel, player_vs_player);
    gui_child_add(menu_panel, player_vs_ai);
    gui_child_add(menu_panel, ai_vs_player);
    gui_child_add(menu_panel, ai_vs_ai);
    gui_child_add(menu_panel, back);

    return menu_panel;
}
