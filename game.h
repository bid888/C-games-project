#ifndef _GAME_H
#define _GAME_H

#include "linked_list.h"
#include "gui.h"
#include "board.h"

#define WIN_W 960
#define WIN_H 640

#define GAME_WIDTH 640
#define GAME_HEIGHT (WIN_H)
#define MENU_WIDTH (WIN_W - GAME_WIDTH)

#define IMG_W 330
#define IMG_H 60

#define PLAYER_HUMAN 1
#define PLAYER_AI 2

#define TRUE 1
#define FALSE 0

typedef struct game {
    const char* name;
    board* (*create_initial_state)();
    linked_list* (*get_possible_child_states)(board*, int);
    int (*get_state_score)(board*, int player);
    int* (*get_game_difficulties)(int* out_size);
    gui_control* (*create_game_panel)(board*, gui_size, gui_size);

	gui_control* (*create_winning_panel)(board*, gui_size, gui_size, gui_control* panel);
    int (*is_move_valid)(board*, int, int, int);
    void (*update_board)(board*, int, int, int);
	int (*has_next_moves)(board*, int);
    int (*is_winlose_state)(board*, int);
} game;

void game_register(game* g);
linked_list* games_get();
game* game_create();
void game_start(gui_control* window);
void game_set_next_game(game* g);
void game_set_game_state(board* game_board, int player);
void game_set_players(int player1_type, int player2_type);
void game_set_difficulty(int player, int difficulty);

game* game_get_current();
board* game_get_current_board();
int game_get_current_player();

//try to perform a move, or ignore if its not one of the possible moves for the current game state.
void game_perform_human_move(game* g, int i, int j);

//sidepanel actions
void game_restart();
void game_quit(gui_control* window);
void game_quit_to_main_menu(gui_control* window);
void game_select_diff1(gui_control* window);
void game_select_diff2(gui_control* window);

//initialize the game management module
void game_init();
void game_release();

#endif
