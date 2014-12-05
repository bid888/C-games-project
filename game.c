#include <assert.h>
#include <limits.h>
#include "menu_game_panel.h"
#include "game.h"
#include "linked_list.h"
#include "minimax.h"
#include "string.h"
#include "reversi.h"

/*The game.c is the manages all the game actions that may be used */
game*           current_game            = NULL;
board*          current_game_state      = NULL;
gui_control*    current_game_panel      = NULL;
gui_control*    current_game_window     = NULL;
int             current_game_diffs[]    = { 0, 0, 0 };
int             current_game_players[]  = { 0, 0, 0 };
int             current_player          = 1;
int             game_over               = 0;
board*          winning_board           = NULL;
gui_control*    side_menu               = NULL;
gui_control*    press_for_next_move     = NULL;
linked_list*    games                   = NULL;
gui_control*    current_player_label    = NULL;

//perform the move for the player on the logical board and update the current player
void game_perform_move(game* g, int i, int j, int player) {
    int prev_player = player;

	const char *pieces[] = {"images/player1.png","images/player2.png"};

	//printf("performing move %d %d for player %d\n", i, j, player);

    g->update_board(current_game_state, i, j, player);

    gui_child_remove(current_game_panel);
    gui_release_tree(current_game_panel);

    current_game_panel = g->create_game_panel(current_game_state, GAME_WIDTH, WIN_H);
    gui_child_add(current_game_window, current_game_panel);

	gui_child_remove(current_player_label);
	gui_release_tree(current_player_label);
	current_player_label = NULL;

    if (g->is_winlose_state(current_game_state, current_player)) {
        current_game_panel = g->create_winning_panel(current_game_state, GAME_WIDTH, WIN_H, current_game_panel);
        game_over = 1;
        return;
    }

    current_player = (current_player == 1) ? 2 : 1;

    if (!g->has_next_moves(current_game_state, current_player) && g->has_next_moves(current_game_state, prev_player))
    {
        /**gui_control* tmp = gui_button_create(GAME_HEIGHT / 8, 2 * GAME_WIDTH / 8, "images/nextPlayer.png", NULL);
        gui_child_add(current_game_panel, tmp);
        */

        printf("There are no valid moves, switching player\n");
        current_player = (current_player == 1) ? 2 : 1;
    }

    if (current_game_players[prev_player] == PLAYER_AI &&
        current_game_players[current_player] == PLAYER_AI && g->has_next_moves(current_game_state, current_player)) {
        menu_game_panel_show_space_warning(side_menu);
    }

	current_player_label = gui_button_create(50 , 5, pieces[current_player -1], NULL);
	gui_child_add(side_menu, current_player_label);

}

//the AI performs a move using the Minimax algorithm
void game_process_ai(game* g, board* game_board) {
    move* result;
    int max_steps;

    if (game_over || current_game_players[current_player] == PLAYER_HUMAN)
        return;

    //TODO perform AI turn until its time for human player
    max_steps = current_game_diffs[current_player];

    result = minimax_pruned(g, game_board, current_player, current_player, 0,  max_steps, -INT_MAX, INT_MAX);

    if (result != NULL) {
        assert(result->player == current_player);
        game_perform_move(g, result->row, result->col, current_player);
        minimax_move_release(result);
    }
}

//resets the current game and resets the board, cleans memory
void game_reset() {
    int* difficulties = current_game->get_game_difficulties(NULL);

    game_over = 0;
    current_player = 1;
    current_game_diffs[1] = difficulties[0];
    current_game_diffs[2] = difficulties[0];

    if (current_game_state != NULL) {
        board_release(current_game_state); current_game_state = NULL;
    }

    if (current_game_panel != NULL) {
        gui_child_remove(current_game_panel);
        gui_release_tree(current_game_panel); current_game_panel = NULL;
    }

    if (side_menu)
        menu_game_panel_hide_space_warning(side_menu);

	if (current_player_label) {
		gui_child_remove(current_player_label);
		gui_release_tree(current_player_label);
		current_player_label = NULL;
	}
}

// default implementation for game->has_next_moves
int default_has_next_moves(board* game_board, int player) {
    int i, j;
    for (i = 0; i < game_board->rows; i++)
        for (j = 0; j < game_board->cols; j++)
            if (board_get(game_board, i, j) == BOARD_EMPTY_CELL)
                return TRUE;

    return FALSE;
}

//default implementation for game->update_board
void default_update_board(board* game_board, int i, int j, int player) {
    board_set(game_board, i, j, player);
}

//default implementation for game->is_winlose_state
int default_is_winlose_state(board* game_board, int player) {
    int score = current_game->get_state_score(game_board, player);

    if (score == INT_MAX)
        return 1;
    else if (score == -INT_MAX)
        return -1;

    return 0;
}

//handle keyboard events
void game_handle_keyboardevent(SDL_KeyboardEvent* key) {
    if (key->keysym.sym == SDLK_SPACE) {
        menu_game_panel_hide_space_warning(side_menu);
        game_process_ai(current_game, current_game_state);
    }
}

/**** start public API ****/

//add the game to a list of existing games
void game_register(game* g) {
    linked_list_addLast(games, g);
}

//find the game from the game list by name
linked_list* games_get() {
    return games;
}
//allocates and initialzes a new game obeject
game* game_create() {
    game empty = {0};
    game* retval = (game*)malloc(sizeof(game));
	if (retval == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    *retval = empty;

    //set default implementation
    retval->update_board = &default_update_board;
    retval->has_next_moves = &default_has_next_moves;
    retval->is_winlose_state = &default_is_winlose_state;

    return retval;
}

//initializes all the UI elements for the new game
void game_panel_start(gui_control* game_window) {

	const char *pieces[] = {"images/player1.png","images/player2.png"};

    //if the current state was not set from load via game_set_game_state()
    if (current_game_state == NULL) {
        current_game_state  = current_game->create_initial_state();
        current_game_panel  = current_game->create_game_panel(current_game_state, GAME_WIDTH, WIN_H);

        if (current_game_players[current_player] == PLAYER_AI)
            menu_game_panel_show_space_warning(side_menu);
    }
    else {
        current_game_panel  = current_game->create_game_panel(current_game_state, GAME_WIDTH, WIN_H);
        if (current_game->is_winlose_state(current_game_state, current_player)) {
            current_game_panel = current_game->create_winning_panel(current_game_state, GAME_WIDTH, WIN_H, current_game_panel);
            game_over = 1;
        }
        else if (current_game_players[current_player] == PLAYER_AI)
            menu_game_panel_show_space_warning(side_menu);
    }

    gui_child_add(current_game_window, current_game_panel);

	current_player_label = gui_button_create(50 , 5, pieces[current_player -1], NULL);
	gui_child_add(side_menu, current_player_label);
}

//creates game window, display it and process any pending AI moves
void game_start(gui_control* window) {
    current_game_window = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);

    side_menu = menu_game_panel_create(current_game_players, current_game);
    gui_child_add(current_game_window, side_menu);

    game_panel_start(current_game_window);

    gui_child_push(window, current_game_window);
}

//set next game we choose
void game_set_next_game(game* g) {
    current_game = g;
    game_reset();
}

//set the board for the current game state
void game_set_game_state(board* game_board, int player) {
    current_game_state = game_board;
	current_player = player;
}
//set the players for the curren game state
void game_set_players(int player1_type, int player2_type) {
    current_game_players[1] = player1_type;
    current_game_players[2] = player2_type;
}
//set the difficulty for the game state
void game_set_difficulty(int player, int difficulty) {
    current_game_diffs[player] = difficulty;
}
//get the current game
game* game_get_current() {
    return current_game;
}
//get the current board for the game state
board* game_get_current_board() {
    return current_game_state;
}
//get the current player for the game
int game_get_current_player() {
    return current_player;
}

//try to perform a move, or ignore if its not one of the possible moves for the current game state.
void game_perform_human_move(game* g, int i, int j) {
    if (game_over || current_game_players[current_player] != PLAYER_HUMAN)
        return;

    if (!g->is_move_valid(current_game_state, i, j, current_player)) {
        printf("ignoring invalid move %d %d for player %d\n", i, j, current_player);
        return;
    }

    game_perform_move(g, i, j, current_player);
    game_process_ai(g, current_game_state);
}

void game_init() {
    games = linked_list_create();
    gui_keyboard_register(game_handle_keyboardevent);
}

//free the memory of the current game
void game_release() {
    if (press_for_next_move != NULL) {
        gui_release_tree(press_for_next_move); press_for_next_move = NULL;
    }
    if (games != NULL) {
        linked_list_release(games);
        games = NULL;
    }
}

//**** sidepanel actions ****//
void game_quit(gui_control* window) {
    game_quit_to_main_menu(window);
    gui_quit();
}

//quit and return to main menu
void game_quit_to_main_menu(gui_control* window) {
    game_reset();

    gui_child_popall(window);
    current_game_window = NULL;
    side_menu = NULL;
}

//restart the game
void game_restart() {
    game_reset();
    game_set_next_game(current_game);
    game_panel_start(current_game_window);
}

