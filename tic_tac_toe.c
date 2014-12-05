#include <assert.h>
#include <limits.h>
#include "SDL_image.h"
#include "tic_tac_toe.h"
#include "gui.h"
#include "board.h"
#include "minimax.h"

/*tic tac toe game related functions*/

game* game_tic_tac_toe = NULL;
SDL_Surface *surface_x, *surface_o;
int difficulties[] = {9};             // single difficulty - level 9 - predict maximum moves...

// Finds the logical i, j cell on the board from the clicked place on the board (from pixels to logical cell) //
static void on_panel_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    int i, j;
    //printf("x: %d, y: %d\n", x, y);

    i = y * 3 / GAME_HEIGHT;
    j = x * 3 / GAME_WIDTH;
    //printf("i: %d, j: %d\n", i, j);

    game_perform_human_move(game_tic_tac_toe, i, j);
}

// ******* start public API *******
// CREATE BOARD 3*3 //
static board* create_initial_state() {
    board* board = board_create(TIC_SIZE, TIC_SIZE);
    return board;
}

// check if there is an empty space on the board
static int is_move_valid(board* game_board, int i, int j, int player) {
    int cell_value = board_get(game_board, i, j);
    if (cell_value == BOARD_EMPTY_CELL)
        return TRUE;

    return FALSE;
}

//find all the future possible moves and return a linked list of those moves
static linked_list* get_possible_child_states(board* game_board, int player) {
    int i = 0, j = 0, cell_value;
    move* next_move;
    board* copy;
    linked_list* possible_moves;

    //check if further moves are even possible, or the game is currently over
    int current_state_score = game_tic_tac_toe->get_state_score(game_board, player);
    if (current_state_score == INT_MAX || current_state_score == - INT_MAX)
        return NULL;

    // will contain all the possible transitions from the current game_board to the next via current player move
    possible_moves = linked_list_create();

    for (i = 0; i < game_board->rows; i++) {
        for (j = 0; j < game_board->cols; j++){
            cell_value = board_get(game_board, i, j);
            if (cell_value != BOARD_EMPTY_CELL)
                continue;

            copy = board_copy(game_board);
            board_set(copy, i, j, player);
            next_move = minimax_move_create(i, j, player, copy);
            linked_list_addLast(possible_moves, next_move);
        }
    }

    return possible_moves;
}

// SCORING FUNCTION - no actual "rank" in tic-tac-toe, only win/lose 
static int get_state_score(board* game_board, int player) {
    int winner, loser, i, j;
    int otherPlayer = player == 1 ? 2 : 1;

    // check rows
    for (i = 0; i < game_board->rows; i++) {
        winner = TRUE;
        loser = TRUE;
        for (j = 0; j < game_board->cols; j++) {
            winner = winner && (board_get(game_board, i, j) == player);
            loser = loser && (board_get(game_board, i, j) == otherPlayer);
            //
        }
        if (winner || loser) {
            return winner ? INT_MAX : - INT_MAX;
        }
    }

    // check cols
    for (j = 0; j < game_board->cols; j++) {
        winner = TRUE;
        loser = TRUE;
        for (i = 0; i < game_board->rows; i++) {
            winner = winner && (board_get(game_board, i, j) == player);
            loser = loser && (board_get(game_board, i, j) == otherPlayer);
        }
        if (winner || loser)
            return winner ? INT_MAX : - INT_MAX;
    }

    // check diag
    winner = TRUE;
    loser = TRUE;
    for (i = 0; i < game_board->rows; i++) {
        winner = winner && (board_get(game_board, i, i) == player);
        loser = loser && (board_get(game_board, i, i) == otherPlayer);
    }
    if (winner || loser)
        return winner ? INT_MAX : - INT_MAX;

    // check other diag
    winner = TRUE;
    loser = TRUE;
    for (i = 0; i < game_board->rows; i++) {
        winner = winner && (board_get(game_board, i, game_board->rows - i - 1) == player);
        loser = loser && (board_get(game_board, i, game_board->rows - i - 1) == otherPlayer);
    }

    if (winner || loser)
        return winner ? INT_MAX : - INT_MAX;

    return 0;
}
//return the possible game difficutlies
static int* get_game_difficulties(int* out_size) {
    if (out_size != NULL)
        *out_size = sizeof(difficulties)/ sizeof(difficulties[0]);

    return difficulties;
}
//Creates the game panel according to the game status, adds the moves of the player - red/yellow moves
static gui_control* create_game_panel(board* game_board, gui_size w, gui_size h) {
    const char *pieces[] = { "images/x.png", "images/o.png" };
    const char *piece;
    int cell_value, i, j;
    gui_control *tmp;

    //TODO
    gui_control* panel = gui_panel_create(0, 0, w, h, "images/tic_tac_toe_bg.png");

    for (i=0; i < game_board->rows; i++) {
        for (j=0; j < game_board->cols; j++) {
            cell_value = board_get(game_board, i, j);
            if (cell_value == BOARD_EMPTY_CELL)
                continue;

            piece = pieces[cell_value - 1];
            tmp = gui_button_create(j * GAME_HEIGHT / 3, i * GAME_WIDTH / 3, piece, NULL);
            gui_child_add(panel, tmp);
        }
    }

    panel->onclick = &on_panel_click;
    return panel;
}

//finds the winning option on the board to create the winning panel and mark the win
static gui_control* create_winning_panel(board* game_board, gui_size w, gui_size h, gui_control* current_panel) {
    const char *pieces[] = { "images/x_win.png", "images/o_win.png" };
    const char *piece;
    int cell_value, i, j;
    gui_control *tmp;

    int count_player;
    int count_other_player;


    //search rows
    for (i = 0; i < game_board->rows; i++) {
        count_player = 0;
        count_other_player=0;
        for (j = 0; j < game_board->cols; j++) {
            if (board_get(game_board, i, j) == 1)
                count_player++;
            else if (board_get(game_board, i, j) == 2)
                count_other_player++;
        }
        if (count_player == TIC_SIZE || count_other_player == TIC_SIZE){
            cell_value = board_get(game_board, i, j-1);
            piece = pieces[cell_value - 1];
            for (j=0; j<game_board->cols; j++) {
                tmp = gui_button_create(j * GAME_HEIGHT / 3, i * GAME_WIDTH / 3, piece, NULL);
                gui_child_add(current_panel, tmp);
            }
            break;
        }
    }


    //search cols
    for (j = 0; j < game_board->cols; j++) {
        count_player = 0;
        count_other_player=0;
        for (i = 0; i < game_board->rows; i++) {
            if (board_get(game_board, i, j) == 1)
                count_player++;
            else if (board_get(game_board, i, j) == 2)
                count_other_player++;
        }

        if (count_player == TIC_SIZE || count_other_player == TIC_SIZE){
            cell_value = board_get(game_board, i-1, j);
            piece = pieces[cell_value - 1];
            for (i=0; i<game_board->cols; i++) {
                tmp = gui_button_create(j * GAME_HEIGHT / 3, i * GAME_WIDTH / 3, piece, NULL);
                gui_child_add(current_panel, tmp);
            }
            break;
        }
    }

    //diag
    count_player = 0;
    count_other_player=0;
    for (i = 0; i < game_board->rows; i++) {
        if (board_get(game_board, i, i) == 1)
            count_player++;
        else if (board_get(game_board, i, i) == 2)
            count_other_player++;
    }

    if (count_player == TIC_SIZE || count_other_player == TIC_SIZE){
        cell_value = board_get(game_board, i-1, i-1);
        piece = pieces[cell_value - 1];
        for (j=0; j<game_board->cols; j++) {
            tmp = gui_button_create(j * GAME_HEIGHT / 3, j * GAME_WIDTH / 3, piece, NULL);
            gui_child_add(current_panel, tmp);

        }
    }

    //other diag
    count_player = 0;
    count_other_player=0;
    for (i = 0; i < game_board->rows; i++) {
        if (board_get(game_board, i, game_board->rows - i - 1) == 1)
            count_player++;
        else if (board_get(game_board, i, game_board->rows - i - 1) == 2)
            count_other_player++;
    }
    if (count_player == TIC_SIZE || count_other_player == TIC_SIZE){
        cell_value = board_get(game_board, i-1, i-3);
        piece = pieces[cell_value - 1];
        for (j=game_board->cols -1; j>=0; j--) {
            tmp = gui_button_create(j * GAME_HEIGHT / 3, (game_board->cols -1 - j) * GAME_WIDTH / 3, piece, NULL);
            gui_child_add(current_panel, tmp);

        }
    }

    return current_panel;
}

//initiate the game //
void tic_tac_toe_init() {
    game* tic_tac_toe = game_create();
    tic_tac_toe->name = "TicTacToe";
    tic_tac_toe->create_initial_state = &create_initial_state;
    tic_tac_toe->get_possible_child_states = &get_possible_child_states;
    tic_tac_toe->get_state_score = &get_state_score;
    tic_tac_toe->get_game_difficulties = &get_game_difficulties;
    tic_tac_toe->create_game_panel = &create_game_panel;
    tic_tac_toe->is_move_valid = &is_move_valid;
    tic_tac_toe->create_winning_panel = &create_winning_panel;

    game_tic_tac_toe = tic_tac_toe;
    game_register(game_tic_tac_toe);
}

//release game //
void tic_tac_toe_release() {
    free(game_tic_tac_toe);
    game_tic_tac_toe = NULL;
}
