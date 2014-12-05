#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "board.h"

/* Board.c - All the related functions to build the game board, create a copy of the original board, rlease the board from the memory, get a value from a specific
cell on the board and set a specific cell to a value */

board* board_create(int rows, int cols) {
    int* temp;
	int i, m;

	board* retval = (board*)malloc(sizeof(board));
	if (retval == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}

    retval->rows = rows;
    retval->cols = cols;

    retval->grid = (int**) malloc(rows * sizeof(int*));
	if (retval->grid == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    temp = (int*) malloc(rows * cols * sizeof(int));
	if (temp == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}

    i = 0;
    for (; i < rows; i++) {
        retval->grid[i] = temp + (i * cols);
    }
    m = 0;
    for (; m < rows; m++) {
        int n = 0;
        for (; n < cols; n++) {
            retval->grid[m][n] = BOARD_EMPTY_CELL;
        }
    }

    return retval;
}

board* board_copy(board* src) {
    int i, j;
	board* retval = board_create(src->rows, src->cols);
    for (i=0; i < retval->rows; i++)
        for (j=0; j < retval->cols; j++)
            retval->grid[i][j] = src->grid[i][j];

    return retval;
}

void board_release(board* game_board) {
    free(game_board->grid[0]);
    free(game_board->grid);
    free(game_board);
}

int board_get(board* game_board, int i, int j) {
    assert(game_board);
    assert(game_board->grid);
    assert(game_board->grid[i]);

    return game_board->grid[i][j];
}

void board_set(board* game_board, int i, int j, int value) {
    assert(game_board);
    assert(game_board->grid);
    assert(game_board->grid[i]);
    game_board->grid[i][j] = value;
}
