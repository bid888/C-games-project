#include <assert.h>
#include <limits.h>
#include "SDL_image.h"
#include "reversi.h"
#include "gui.h"
#include "board.h"
#include "minimax.h"

static int is_game_over(board* game_board);

game* game_reversi = NULL;
SDL_Surface *surface_white, *surface_black;
static int difficulties[] = {1, 2, 3, 4};
int valid_moves[] = { 0, 0, 0, 0, 0, 0, 0, 0 };

// the function returns all the valid moves
int* get_moves()
{
    return valid_moves;
}

// click on the panel and perform a move
static void on_panel_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    int i, j;
    printf("x: %d, y: %d\n", x, y);

    i = y * 8 / GAME_HEIGHT;
    j = x * 8 / GAME_WIDTH;
    printf("i: %d, j: %d\n", i, j);

    game_perform_human_move(game_reversi, i, j);
}

// set the initial state of the board and return the game board
static board* create_initial_state() {
    board* board = board_create(8, 8);

    board_set(board, 3, 3, 1);
    board_set(board, 3, 4, 2);
    board_set(board, 4, 3, 2);
    board_set(board, 4, 4, 1);

    return board;
}

// check if there are valid moves for the current player
static int has_next_moves(board* game_board, int player)
{
    int i, j;
    int valid = 0;
    for (i = 0; i < game_board->rows; i++)
    {
        for (j = 0; j < game_board->cols; j++)
        {
            if (board_get(game_board, i, j) == BOARD_EMPTY_CELL)
            {
                if (game_reversi->is_move_valid(game_board, i, j, player) == 0)
                    continue;
				else // at least one valid move
                    return 1;
            }
            else
                continue;
        }
    }
    return valid;
}

// perform all human valid moves (i,j) for the current player
void human_move(int* moves, board* game_board, int i, int j, int player)
{
    int m, n;
    int otherPlayer = player == 1 ? 2 : 1;
    board_set(game_board, i, j, player);

    if (moves[0] == 1) // perform a row move
	{
        m = i;
        for (n = j - 1; n >= 0; n--)
        {
            if (board_get(game_board, m, n) == otherPlayer)
                board_set(game_board, m, n, player);
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }

    if (moves[1] == 1) // perform a row move
    {
        m = i;
        for (n = j + 1; n < game_board->cols; n++)
        {
            if (board_get(game_board, m, n) == otherPlayer)
                board_set(game_board, m, n, player);
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }
    if (moves[2] == 1) // perform a column move
    {
        m = j;
        for (n = i - 1; n >= 0; n--)
        {
            if (board_get(game_board, n, m) == otherPlayer)
                board_set(game_board, n, m, player);
            else if (board_get(game_board, n, m) == player)
                break;
        }
    }
    if (moves[3] == 1) // perform a column move
    {
        m = j;
        for (n = i + 1; n < game_board->rows; n++)
        {
            if (board_get(game_board, n, m) == otherPlayer)
                board_set(game_board, n, m, player);
            else if (board_get(game_board, n, m) == player)
                break;
        }
    }

    if (moves[4] == 1) // perform a diagonal move
    {
        m = i - 1; n = j - 1;
        while (m >= 0 && n >= 0)
        {
            if (board_get(game_board, m, n) == otherPlayer)
            {
                board_set(game_board, m, n, player);
                m -= 1;
                n -= 1;
            }
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }

    if (moves[5] == 1) // perform a diagonal move
    {
        m = i + 1; n = j + 1;
        while (m < game_board->rows && n < game_board->cols)
        {
            if (board_get(game_board, m, n) == otherPlayer)
            {
                board_set(game_board, m, n, player);
                m += 1;
                n += 1;
            }
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }

    if (moves[6] == 1) // perform a diagonal move
    {
        m = i - 1; n = j + 1;
        while (m >= 0 && n < game_board->cols)
        {
            if (board_get(game_board, m, n) == otherPlayer)
            {
                board_set(game_board, m, n, player);
                m -= 1;
                n += 1;
            }
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }

    if (moves[7] == 1) // perform a diagonal move
    {
        m = i + 1; n = j - 1;
        while (m < game_board->rows && n >= 0)
        {
            if (board_get(game_board, m, n) == otherPlayer)
            {
                board_set(game_board, m, n, player);
                m += 1;
                n -= 1;
            }
            else if (board_get(game_board, m, n) == player)
                break;
        }
    }
}

// the function check if the move (i,j) is valid for the current player
static int is_move_valid(board* game_board, int i, int j, int player) {
    int otherPlayer = player == 1 ? 2 : 1;
    int m, n, k, valid = 0;
    int cell_value = board_get(game_board, i, j);

	// there are 8 possible moves on the game board
    for (k = 0; k < 8; k++)
    {
        valid_moves[k] = 0;
    }

    // first check if the cell is empty
    if (cell_value != BOARD_EMPTY_CELL)
        return valid;


    // check row
    if ((j - 1 >= 0) && board_get(game_board, i, j - 1) == otherPlayer)
    {

        m = i;
        if (j - 2 >= 0) {
            for (n = j - 2; n >= 0; n--)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                    continue;
                else if (board_get(game_board, m, n) == player)
                {
                    valid_moves[0] = 1; // the row move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }


    if ((j + 1 < game_board->cols) && (board_get(game_board, i, j + 1) == otherPlayer))
    {
        m = i;
        if (j + 2 < game_board->cols){
            for (n = j + 2; n < game_board->cols; n++)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                    continue;
                else if (board_get(game_board, m, n) == player)
                {
                    valid_moves[1] = 1; // the second row move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }

    // check col
    if ((i-1 >= 0) && (board_get(game_board, i - 1, j) == otherPlayer))
    {
        m = j;
        if (i - 2 >= 0){
            for (n = i - 2; n >= 0; n--)
            {
                if (board_get(game_board, n, m) == otherPlayer)
                    continue;
                else if (board_get(game_board, n, m) == player)
                {
                    valid_moves[2] = 1; // the column move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }

    if ((i+1 < game_board->rows) && (board_get(game_board, i + 1, j) == otherPlayer))
    {
        m = j;
        if (i + 2 <= game_board->rows){
            for (n = i + 2; n < game_board->rows; n++)
            {
                if (board_get(game_board, n, m) == otherPlayer)
                    continue;
                else if (board_get(game_board, n, m) == player)
                {
                    valid_moves[3] = 1; // the second column mover is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }

    // check diagonal
    if ((i - 1 >= 0) && (j - 1 >= 0) && (board_get(game_board, i - 1, j - 1) == otherPlayer))
    {
        if (i - 2 >= 0 && j - 2 >= 0){
            m = i - 2; n = j - 2;
            while (m >= 0 && n >= 0)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                {
                    m = m-1;
                    n = n-1;
                }
                else if (board_get(game_board, m, n) == player)
				{
                    valid_moves[4] = 1; // the diagonal move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }


    if ((i + 1 < game_board->rows) && (j + 1 < game_board->cols) && (board_get(game_board, i + 1, j + 1) == otherPlayer))
    {
        if (i + 2 <= game_board->rows && j + 2 <= game_board->cols){
            m = i + 2; n = j + 2;
            while (m < game_board->rows && n < game_board->cols)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                {
                    m += 1;
                    n += 1;
                }
                else if (board_get(game_board, m, n) == player)
                {
                    valid_moves[5] = 1; // the diagonal move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }

    // check other diagonal
    if ((i - 1 >= 0) && (j + 1 < game_board->cols) && (board_get(game_board, i - 1, j + 1) == otherPlayer))
    {
        if (i - 2 >= 0 && j + 2 < game_board->cols){
            m = i - 2; n = j + 2;
            while (m >= 0 && n < game_board->cols)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                {
                    m = m-1;
                    n += 1;
                }
                else if (board_get(game_board, m, n) == player)
                {
                    valid_moves[6] = 1; // the diagonal move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }

    if ((i + 1 < game_board->rows) && (j - 1 >= 0) && (board_get(game_board, i + 1, j - 1) == otherPlayer))
    {
        if (i + 2 <= game_board->rows && j - 2 >= 0){
            m = i + 2; n = j - 2;
            while (m < game_board->rows && n >= 0)
            {
                if (board_get(game_board, m, n) == otherPlayer)
                {
                    m += 1;
                    n = n-1;
                }
                else if (board_get(game_board, m, n) == player)
				{
                    valid_moves[7] = 1; // the diagonal move is valid
                    valid = 1;
                    break;
                }
                else
                    break;
            }
        }
    }
    return valid;
}

// the function returns possible moves for the AI
static linked_list* get_possible_child_states(board* game_board, int player) {
    int i = 0, j = 0;
    move* next_move;
    board* copy;
    linked_list* possible_moves;

    //check if further moves are even possible, or the game is currently over
    int current_state_score = game_reversi->get_state_score(game_board, player);
    //printf("The current state score is: %d\n", current_state_score);

	if (current_state_score == INT_MAX || current_state_score == -INT_MAX)
        return NULL;

    // will contain all the possible transitions from the current game_board to the next via current player move
    possible_moves = linked_list_create();


    for (i = 0; i < game_board->rows; i++) {
        for (j = 0; j < game_board->cols; j++){
            if (is_move_valid(game_board, i, j, player) == 0) // if the move isn't valid, continue scanning the game board
                continue;

			// the move is valid
            copy = board_copy(game_board);
            board_set(copy, i, j, player);
            next_move = minimax_move_create(i, j, player, copy); // create next move by minimax algorithm
            linked_list_addLast(possible_moves, next_move);
        }
    }

    return possible_moves;
}

// the function is_game_over checks if the game is over
int is_game_over(board* game_board)
{
    int i, j, over;
    over = TRUE; // as default the game is over
    for (i = 0; i < game_board->rows; i++)
    {
        for (j = 0; j < game_board->cols; j++)
        {
            if (board_get(game_board, i, j) != 0) // the slot (i,j) on the board isn't empty, then check the others
                continue;
			else if (has_next_moves(game_board, 1) != 0 || has_next_moves(game_board, 2) != 0) // at least one of the players has some moves to do
            {
                over = FALSE; // game is not over
                break;
            }
        }
    }
    return over;
}


// the function gives a score to the game board
static int get_state_score(board* game_board, int player) {
	int i, j;
	int sumPlayer1 = 0, sumPlayer2 = 0, sumRegion = 0;
	int regions_vector[5] = { 1, -1, 5, -5, 10 };

	// check if the board is full or if there are not valid moves at all
	int over = is_game_over(game_board);

	if (over)
		return INT_MAX;

	// check region 1
	for (i = 2; i < game_board->rows - 2; i++)
	{
		for (j = 2; j < game_board->cols - 2; j++)
		{
			if (board_get(game_board, i, j) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, i, j) == 2) // player 2
				sumPlayer2 += 1;
		}
	}

	sumRegion += (regions_vector[0] * (sumPlayer1 - sumPlayer2));

	// check region 2
	i = 1, sumPlayer1 = 0, sumPlayer2 = 0;
	while (i < game_board->rows - 1)
	{
		for (j = 2; j < game_board->cols - 2; j++)
		{
			if (board_get(game_board, i, j) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, i, j) == 2) // player 2
				sumPlayer2 += 1;
			if (board_get(game_board, j, i) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, j, i) == 2) // player 2
				sumPlayer2 += 1;
		}
		i += 5;
	}

	sumRegion += (regions_vector[1] * (sumPlayer1 - sumPlayer2));

	// check region 3
	i = 0, sumPlayer1 = 0, sumPlayer2 = 0;
	while (i < game_board->rows)
	{
		for (j = 2; j < game_board->cols - 2; j++)
		{
			if (board_get(game_board, i, j) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, i, j) == 2) // player 2
				sumPlayer2 += 1;
			if (board_get(game_board, j, i) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, j, i) == 2) // player 2
				sumPlayer2 += 1;
		}
		i += 7;
	}

	sumRegion += (regions_vector[2] * (sumPlayer1 - sumPlayer2));

	// check region 4
	i = 0, sumPlayer1 = 0, sumPlayer2 = 0;
	while (i < 2)
	{
		for (j = 0; j < game_board->cols; j++)
		{
			if ((i == 0 && j == 0) || (i == 0 && j == game_board->cols - 1) || ((j > 1 && j < game_board->cols - 2) || (i > 1 && i < game_board->rows - 2)))
				continue;
			if (board_get(game_board, i, j) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, i, j) == 2) // player 2
				sumPlayer2 += 1;
			if (board_get(game_board, game_board->rows - i - 1, game_board->cols - j - 1) == 1) // player 1
				sumPlayer1 += 1;
			else if (board_get(game_board, game_board->rows - i - 1, game_board->cols - j - 1) == 2) // player 2
				sumPlayer2 += 1;
		}
		i += 1;
	}

	sumRegion += (regions_vector[3] * (sumPlayer1 - sumPlayer2));

	// check region 5
	i = 0, j = 0; sumPlayer1 = 0, sumPlayer2 = 0;
	if (board_get(game_board, i, j) == 1) // player 1
		sumPlayer1 += 1;
	else if (board_get(game_board, i, j) == 2) // player 2
		sumPlayer2 += 1;

	if (board_get(game_board, game_board->rows - i - 1, game_board->cols - j - 1) == 1) // player 1
		sumPlayer1 += 1;
	else if (board_get(game_board, game_board->rows - i - 1, game_board->cols - j - 1) == 2) // player 2
		sumPlayer2 += 1;

	if (board_get(game_board, i, game_board->cols - j - 1) == 1) // player 1
		sumPlayer1 += 1;
	else if (board_get(game_board, i, game_board->cols - j - 1) == 2) // player 2
		sumPlayer2 += 1;

	if (board_get(game_board, game_board->rows - i - 1, j) == 1) // player 1
		sumPlayer1 += 1;
	else if (board_get(game_board, game_board->rows - i - 1, j) == 2) // player 2
		sumPlayer2 += 1;

	sumRegion += (regions_vector[4] * (sumPlayer1 - sumPlayer2));

	return sumRegion;
}

// creates the panel of the game area and adds it to the gui
static gui_control* create_game_panel(board* game_board, gui_size w, gui_size h) {
    const char *pieces[] = { "images/white.png", "images/black.png" };
    const char *piece;
    int cell_value, i, j;
    gui_control *tmp;

    //TODO
    gui_control* panel = gui_panel_create(0, 0, w, h, "images/reversiBoard.png");

    for (i = 0; i < game_board->rows; i++) {
        for (j = 0; j < game_board->cols; j++) {
            cell_value = board_get(game_board, i, j);
            if (cell_value == BOARD_EMPTY_CELL)
                continue;

            piece = pieces[cell_value - 1];
            tmp = gui_button_create(j * GAME_HEIGHT / 8, i * GAME_WIDTH / 8, piece, NULL);
            gui_child_add(panel, tmp);
        }
    }

    panel->onclick = &on_panel_click;
    return panel;
}

// the function returns the game possible difficulties
static int* get_game_difficulties(int* size) {
	if (size != NULL)
		*size = sizeof(difficulties)/sizeof(difficulties[0]);

    return difficulties;
}

// the function updates the game board - perform the human move
static void update_board(board* game_board, int i, int j, int player) {
	int valid = is_move_valid(game_board, i, j, player);
	if (valid != 0)
	 	human_move(get_moves(), game_board, i, j, player);
	else
		return;

}

// the function creates the winning panel with the victory announcement
static gui_control* create_winning_panel(board* game_board, gui_size w, gui_size h, gui_control* current_panel) {
	const char *pieces[] = { "images/white_win.png", "images/black_win.png", "images/tie.png" };
	const char *piece;
	int i, j;
	gui_control *tmp, *game_over;

	int count_black_player = 0;
	int count_white_player = 0;

	if (is_game_over(game_board))
	{
		// counting the number of game pieces on the board
		for (i = 0; i < game_board->rows; i++)
		{
			for (j = 0; j < game_board->cols; j++)
			{
				if (board_get(game_board, i, j) == 1) // piece of player 1
					count_white_player += 1;
				else if (board_get(game_board, i, j) == 2) // piece of player 2
					count_black_player += 1;
			}
		}

		//printf("Player1 has %d disks, while player2 has %d disks\n", count_white_player, count_black_player);
		if (count_white_player > count_black_player)
			piece = pieces[0]; // white player wins
		else if (count_white_player < count_black_player)
			piece = pieces[1]; // black player wins
		else
			piece = pieces[2]; // it's a tie
		game_over = gui_button_create(2 * GAME_HEIGHT / 8, 2 * GAME_WIDTH / 8, "images/game_over.png", NULL);
		tmp = gui_button_create(2 * GAME_HEIGHT / 8 , (2 * GAME_WIDTH / 8) + 80, piece, NULL);
		gui_child_add(current_panel, game_over);
		gui_child_add(current_panel, tmp);


	}

	return current_panel;
}

int is_winlose_state(board* game_board, int player) {
    int next_player = (player == 1) ? 2 : 1;
    int score = get_state_score(game_board, player);

    if (score == INT_MAX)
        return 1;
    else if (score == -INT_MAX)
        return -1;

    if (!has_next_moves(game_board, next_player) && !has_next_moves(game_board, player))
        return TRUE;

    return FALSE;
}

void reversi_init() {
    game* reversi = game_create();
    reversi->name = "Reversi";
    reversi->create_initial_state = &create_initial_state;
    reversi->get_possible_child_states = &get_possible_child_states;
    reversi->get_state_score = &get_state_score;
    reversi->get_game_difficulties = &get_game_difficulties;
    reversi->create_game_panel = &create_game_panel;
    reversi->is_move_valid = &is_move_valid;
    reversi->update_board = &update_board;
    reversi->create_winning_panel = &create_winning_panel;
	reversi->has_next_moves = &has_next_moves;
    reversi->is_winlose_state = &is_winlose_state;
    game_reversi = reversi;
    game_register(reversi);
}

void reversi_release() {
    free(game_reversi);
    game_reversi = NULL;
}
