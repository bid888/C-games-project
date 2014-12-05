#ifndef _MINIMAX_H
#define _MINIMAX_H

#include "linked_list.h"
#include "board.h"
#include "game.h"

typedef struct move {
    int row;
    int col;
    int player;
    board* new_game_state;
    int score;
} move;

move*   minimax_move_create(int row, int col, int player, board* new_game_state);
move*   minimax_move_copy(move* src);
void 	minimax_move_release(move* m);
void    minimax_tree_release(linked_list* list);
move*   minimax_pruned(game* g, board* game_board, int maximizing_player, int current_level_player, int level, int max_steps, int alpha, int beta);

#endif
