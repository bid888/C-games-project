#include <assert.h>
#include <limits.h>
#include "SDL_image.h"
#include "connect4.h"
#include "gui.h"
#include "board.h"
#include "minimax.h"

/*Connect4 game related functions*/

game* game_connect4 = NULL;
SDL_Surface *surface_red, *surface_yellow;
static int difficulties[] = {1,2,3,4,5,6,7 };             // difficutlies in range 1-7 (level 1, 2, etc...)

static int getCellValueForPlayer(int val, int player);
static board* boardGetScoreMatrix(board* game_board, int rows, int cols, int player);
static void score_my_rows(int max_rows, int max_cols, board* game_board, int player, board* result);
static void score_my_cols(int max_rows, int max_cols, board* game_board, int player, board* result);
static void score_my_diag_right(int max_rows, int max_cols, board* game_board,int player,  board* result);
static void score_my_diag_left(int max_rows, int max_cols, board* game_board,int player,  board* result);

// Finds the logical i, j cell on the board from the clicked place on the board (from pixels to logical cell) //
static void on_panel_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
	int i, j;
	//printf("x: %d, y: %d\n", x, y);

	i = y * 6 / 480; //game's height is 480
	j = x * 7 / GAME_WIDTH; //game's width is 640
	//printf("i: %d, j: %d\n", i, j);

	game_perform_human_move(game_connect4, i, j);
}

// CREATE BOARD 6 rows, 7 columns //
static board* create_initial_state() {
	board* board = board_create(6, 7);
	return board;
}

// check if there is an empty space at the top of the board (first logical row) //
static int is_move_valid(board* game_board, int i, int j, int player) {
	int cell_value = board_get(game_board, 0, j);
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
	int current_state_score = game_connect4->get_state_score(game_board, player);
	if (current_state_score == INT_MAX || current_state_score == - INT_MAX)
		return NULL;

	// will contain all the possible transitions from the current game_board to the next via current player move - find an empty row for each column
	possible_moves = linked_list_create();

	for (j = 0; j < game_board->cols; j++) {
		for (i = game_board->rows -1; i >= 0; i--){
			cell_value = board_get(game_board, i, j);
			if (cell_value != BOARD_EMPTY_CELL)
				continue;

			copy = board_copy(game_board);
			board_set(copy, i, j, player);
			next_move = minimax_move_create(i, j, player, copy);
			linked_list_addLast(possible_moves, next_move);
			break;
		}
	}

	return possible_moves;
}

//calculate the score of the board and return the score 
static int get_state_score(board* game_board, int player) {

	int result = 0;
	int i, j;
	int weights[2][3] = { { -1, -2, -5 }, { 1, 2, 5 } };
	board* scoreMat = boardGetScoreMatrix(game_board, Connect4_ROWS, Connect4_COLS, player);
	if (board_get(scoreMat, 0, 3) > 0) {
		board_release(scoreMat);
		return - INT_MAX;
	}
	if (board_get(scoreMat, 1, 3) > 0) {
		board_release(scoreMat);
		return INT_MAX;
	}

	for (i=0; i < 2; i++) {
		for (j = 0; j < 3; j++) {
			result += board_get(scoreMat, i, j) * weights[i][j];
		}
	}

	board_release(scoreMat);
	return result;
}

//get the cell value for each player - retun 1 for the current player and -1 for the second player
static int getCellValueForPlayer(int val, int player) {

	int retVal = 0;

	if (player == 1) {
		if (val == 2) {
			retVal = -1;
		} else if (val == 1) {
			retVal = 1;
		}
	}

	if (player == 2) {
		if (val == 2) {
			retVal = 1;
		} else if (val == 1) {
			retVal = -1;
		}
	}
	return retVal;
}

//create a board to count all the possible permutations 
board* boardGetScoreMatrix(board* game_board, int rows, int cols, int player) {
	board* result = board_create(2, 4);
	score_my_rows(rows, cols, game_board, player, result);
	score_my_cols(rows, cols, game_board, player, result);
	score_my_diag_right(rows, cols, game_board, player, result);
	score_my_diag_left(rows, cols, game_board, player, result);

	return result;
}

//find the correct index to update in the count matrix
void find_index_result (int summy, board* result) {
	int res;
	if (summy < 0) {
		res = board_get(result, 0, abs(summy) - 1);
		res++;
		board_set(result, 0, abs(summy) - 1, res);
	}
	else if (summy > 0) {
		res = board_get(result, 1, abs(summy) - 1);
		res++;
		board_set(result, 1, abs(summy) - 1, res);
	}

}

//go over all the rows on the board and count all the permutations of 4 discs score for the current player
void score_my_rows(int max_rows, int max_cols, board* game_board, int player, board* result) {

	int summy, i;
	int row, col;

	for (row = 0; row < max_rows; row++) {
		for (col = 0; col <= max_cols - 4; col++) {

			if (col == 0) {
				summy = 0;
				for (i = 0; i < 4; i++) {
					summy += getCellValueForPlayer(board_get(game_board,row, col + i), player);
				}

				find_index_result (summy, result);
			}

			else {
				summy = summy + getCellValueForPlayer(board_get(game_board,row, col + 3), player);
				summy = summy - getCellValueForPlayer(board_get(game_board, row, col - 1), player);

				find_index_result (summy, result);
			}
		}
	}
}

//go over all the columns on the board and count all the permutations of 4 discs score for the current player
void score_my_cols(int max_rows, int max_cols, board* game_board, int player, board* result) {

	int summy, row, col, i;

	for (col = 0; col < max_cols; col++) {
		for (row = 0; row <= max_rows - 4; row++) {

			if (row == 0) {
				summy = 0;
				for (i = 0; i < 4; i++) {
					summy += getCellValueForPlayer(board_get(game_board, row + i, col), player);
				}

				find_index_result (summy, result);
			}

			else {
				summy = summy+ getCellValueForPlayer(board_get(game_board, row + 3, col), player);
				summy = summy- getCellValueForPlayer(board_get(game_board,row - 1, col), player);

				find_index_result (summy, result);
			}
		}
	}

}

//go over all the diagonal from left to right on the board and count all the permutations of 4 discs score for the current player
void count_diag_right(int max_rows, int max_cols, board* game_board, int player, board* result, int row, int col) {

	int i, k, l;
	int first_summy = 0;

	for (i = 0; i < 4; i++) {
		first_summy += getCellValueForPlayer(board_get(game_board,row + i,col + i), player);
	}

	find_index_result (first_summy, result);

	for (k = row + 1, l = col + 1; k <= max_rows - 4 && l <= max_cols - 4; k++, l++) {
		first_summy += getCellValueForPlayer(board_get(game_board, k + 3, l + 3), player);
		first_summy -= getCellValueForPlayer(board_get(game_board,k - 1, l - 1), player);

		find_index_result (first_summy, result);
	}
}

//go over all the diagonal roght to left on the board and count all the permutations of 4 discs score for the current player
void count_diag_left(int max_rows, int max_cols, board* game_board, int player, board* result, int row, int col) {

	int i, k, l;
	int first_summy = 0;

	for (i = 0; i < 4; i++) {
		first_summy += getCellValueForPlayer(board_get(game_board, row + i, col - i), player);
	}

	find_index_result (first_summy, result);

	for (k = row + 1, l = col - 1; k <= max_rows - 4 && l >= 3; k++, l--) {
		first_summy += getCellValueForPlayer(board_get(game_board, k + 3, l - 3), player);
		first_summy -= getCellValueForPlayer(board_get(game_board, k - 1, l + 1), player);

		find_index_result (first_summy, result);
	}
}

//find the score for all the diagonal from left to right - once by going over the first row (top half of the board) and once by going over the first column (bottom half of the board)
void score_my_diag_right(int max_rows, int max_cols, board* game_board, int player, board* result) {

	int i, j;

	for (i = 0; i <= max_cols - 4; i++) {
		count_diag_right(max_rows, max_cols, game_board, player, result, 0, i);
	}

	for (j = 1; j <= max_rows - 4; j++) {
		count_diag_right(max_rows, max_cols, game_board, player, result, j, 0);
	}

}
//find the score for all the diagonal from left to right - once by going over the first row (top half of the board) and once by going over the first column (bottom half of the board)
void score_my_diag_left(int max_rows, int max_cols, board* game_board, int player, board* result) {

	int i, j;

	for (i = max_cols - 1; i >= 3; i--) {
		count_diag_left(max_rows, max_cols, game_board, player, result, 0, i);
	}

	for (j = 1; j <= max_rows - 4; j++) {
		count_diag_left(max_rows, max_cols, game_board, player, result, j, max_cols - 1);
	}

}

//return the possible game difficutlies
static int* get_game_difficulties(int* out_size) {
	if (out_size != NULL)
		*out_size = sizeof(difficulties)/ sizeof(difficulties[0]);

	return difficulties;
}

//Creates the game panel according to the game status, adds the moves of the player - red/yellow moves
static gui_control* create_game_panel(board* game_board, gui_size w, gui_size h) {
	const char *pieces[] = { "images/red.png", "images/yellow.png" };
	const char *piece;
	int cell_value, i, j;
	gui_control *tmp;

	//TODO
	gui_control* panel = gui_panel_create(20, 50, w, 480, "images/Connect4Board.png"); // panel starts 20 to the right and 50 to the bottom

	for (i=0; i < game_board->rows; i++) {
		for (j=0; j < game_board->cols; j++) {
			cell_value = board_get(game_board, i, j);
			if (cell_value == BOARD_EMPTY_CELL)
				continue;

			piece = pieces[cell_value - 1];
			tmp = gui_button_create(j * GAME_WIDTH / 7 , i * 480 / 6, piece, NULL);
			gui_child_add(panel, tmp);
		}
	}

	panel->onclick = &on_panel_click;
	return panel;
}

//updates the board and adds disc to the first free row of the column 
static void update_board(board* game_board, int i, int j, int player) {
	// check for columns only for Connect4 //
	int row=game_board->rows -1;
	for (; row >=0 ; row--) {
		if (board_get(game_board, row, j) == BOARD_EMPTY_CELL) {
			board_set(game_board, row, j, player);
			break;
		}
	}
}

//adds a mark to the winning option on the board according to the 4 possible winning stats - row, col, diag right, diag left
void check_draw_win(gui_control* current_panel, int row, int col, int summy, int option) {

	const char *pieces[] = {"images/connect_win.png"};
	const char *piece = pieces[0];
	gui_control* to_add;
	int j;

	if (summy == 4 || summy == -4) {
		if (option == 1){
			for (j=0; j<4; j++){
				to_add = gui_button_create((col+j) * GAME_WIDTH / 7 , row * 480 / 6, piece, NULL);
				gui_child_add(current_panel, to_add);
			}
		}

		if (option == 2){
			for (j=0; j<4; j++){
				to_add = gui_button_create(col * GAME_WIDTH / 7 , (row+j) * 480 / 6, piece, NULL);
				gui_child_add(current_panel, to_add);
			}
		}

		if (option == 3){
			for (j=0; j<4; j++){
				to_add = gui_button_create((col + j) * GAME_WIDTH / 7 , (row+j) * 480 / 6, piece, NULL);
				gui_child_add(current_panel, to_add);
			}
		}

		if (option == 4){
			for (j=0; j<4; j++){
				to_add = gui_button_create((col - j) * GAME_WIDTH / 7 , (row + j) * 480 / 6, piece, NULL);
				printf("%d %d\n", col -j, row+j);
				gui_child_add(current_panel, to_add);
			}
		}
	}
}

//finds the winning option on the board to create the winning panel and mark the win
static gui_control* create_winning_panel(board* game_board, gui_size w, gui_size h, gui_control* current_panel) {
	int summy, row, col, i, k, l;

	//rows - option 1
	for (row = 0; row < Connect4_ROWS; row++) {
		for (col = 0; col <= Connect4_COLS - 4; col++) {
			if (col == 0) {
				summy = 0;
				for (i = 0; i < 4; i++) {
					summy += getCellValueForPlayer(board_get(game_board,row, col + i), 1);
				}
				check_draw_win(current_panel, row, col, summy, 1);
			}
			else {
				summy = summy + getCellValueForPlayer(board_get(game_board,row, col + 3), 1);
				summy = summy - getCellValueForPlayer(board_get(game_board, row, col - 1), 1);

				check_draw_win(current_panel, row, col, summy, 1);
			}
		}
	}

	//cols - option 2
	for (col = 0; col < Connect4_COLS; col++) {
		for (row = 0; row <= Connect4_ROWS - 4; row++) {
			if (row == 0) {
				summy = 0;
				for (i = 0; i < 4; i++) {
					summy += getCellValueForPlayer(board_get(game_board, row + i, col), 1);
				}
				check_draw_win(current_panel, row, col, summy, 2);
			}

			else {
				summy = summy+ getCellValueForPlayer(board_get(game_board, row + 3, col), 1);
				summy = summy- getCellValueForPlayer(board_get(game_board,row - 1, col), 1);

				check_draw_win(current_panel, row, col, summy, 2);
			}
		}
	}

	//diag right case 1 (option 3) - moves by going on the first row to the right
	row = 0;
	for (col = 0; col <= Connect4_COLS - 4; col++) {
		summy=0;
		for (i = 0; i < 4; i++) {
			summy += getCellValueForPlayer(board_get(game_board, i, col + i), 1);
		}
		check_draw_win(current_panel, 0, col, summy, 3);

		for (k = row + 1, l = col + 1; k <= Connect4_ROWS - 4 && l <= Connect4_COLS - 4; k++, l++) {
			summy += getCellValueForPlayer(board_get(game_board, k + 3, l + 3), 1);
			summy -= getCellValueForPlayer(board_get(game_board,k - 1, l - 1), 1);

			check_draw_win(current_panel, k, l, summy, 3);
		}
	}

	//diag right case 2 (still option 3) - moves by going on the first column down
	col=0;
	for (row = 1; row <= Connect4_ROWS - 4; row++) {
		summy=0;
		for (i = 0; i < 4; i++) {
			summy += getCellValueForPlayer(board_get(game_board, row + i, i), 1);
		}
		check_draw_win(current_panel, row, 0, summy, 3);

		for (k = row + 1, l = col + 1; k <= Connect4_ROWS - 4 && l <= Connect4_COLS - 4; k++, l++) {
			summy += getCellValueForPlayer(board_get(game_board, k + 3, l + 3), 1);
			summy -= getCellValueForPlayer(board_get(game_board, k - 1, l - 1), 1);

			check_draw_win(current_panel, k, l, summy, 3);
		}
	}

	//diag left case 1 (option 4) - going left on the first row
	row=0;
	for (col = Connect4_COLS - 1; col >= 3; col--) {
		summy=0;
		for (i = 0; i < 4; i++) {
			summy += getCellValueForPlayer(board_get(game_board, i, col - i), 1);
		}
		check_draw_win(current_panel, 0, col, summy, 4);

		for (k = row + 1, l = col - 1; k <= Connect4_ROWS - 4 && l >= 3; k++, l--) {
			summy += getCellValueForPlayer(board_get(game_board, k + 3, l - 3), 1);
			summy -= getCellValueForPlayer(board_get(game_board, k - 1, l + 1), 1);

			check_draw_win(current_panel, k, l, summy, 4);
		}
	}
	//diag left case 2 (still otion 4) - going left on the right col
	col = Connect4_COLS - 1;
	for (row = 1; row <= Connect4_ROWS - 4; row++) {
		summy = 0;
		for (i = 0; i < 4; i++) {
			summy += getCellValueForPlayer(board_get(game_board, row + i, col - i), 1);
		}
		check_draw_win(current_panel, row, Connect4_COLS - 1, summy, 4);

		for (k = row + 1, l = col - 1; k <= Connect4_ROWS - 4 && l >= 3; k++, l--) {
			summy += getCellValueForPlayer(board_get(game_board, k + 3, l - 3), 1);
			summy -= getCellValueForPlayer(board_get(game_board, k - 1, l + 1), 1);

			check_draw_win(current_panel, k, l, summy, 4);
		}

	}
	return current_panel;
}


//initiate the game //
void connect4_init() {
	game* connect4 = game_create();
	connect4->name = "Connect4";
	connect4->create_initial_state = &create_initial_state;
	connect4->get_possible_child_states = &get_possible_child_states;
	connect4->get_state_score = &get_state_score;
    connect4->get_game_difficulties = &get_game_difficulties;
    connect4->create_game_panel = &create_game_panel;
    connect4->is_move_valid = &is_move_valid;
    connect4->update_board = &update_board;
    connect4->create_winning_panel = &create_winning_panel;

	game_connect4 = connect4;
	game_register(connect4);
}

//release game //
void connect4_release() {
	free(game_connect4);
	game_connect4 = NULL;
}
