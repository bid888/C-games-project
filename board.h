#ifndef _BOARD_H
#define _BOARD_H

#define BOARD_EMPTY_CELL 0

typedef struct board {
	int rows, cols;
	int** grid;
} board;

board* board_create(int rows, int cols);
board* board_copy(board* game_board);
void board_set(board* game_board, int i, int j, int value);
int board_get(board* game_board, int i, int j);
void board_release(board* game_board);
void board_print(board* src);

#endif
