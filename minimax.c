#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <limits.h>
#include "minimax.h"

/* for debugging purposes
void print_possible_next_moves(linked_list* list) {
    linked_list_node* p = list->head;
    move* current_move = NULL;

    while (p != NULL) {
        current_move = (move*)p->item;
        printf("player %d score %d row %d col %d\n", current_move->player, current_move->score, current_move->row, current_move->col);
      //board_print(current_move->new_game_state);

        p = p->next;
    }
}*/

/*MINIMAX PRUNED! */
move* minimax_pruned(game* g, board* game_board, int maximizing_player, int current_level_player, int level, int max_steps, int alpha, int beta) {
    int next_player = current_level_player == 1 ? 2 : 1;
	linked_list_node* p = NULL;
    move* current_move = NULL;
    move* best_next_move = NULL;
    move* best_move = NULL;
    linked_list* possible_moves = NULL;

    if (level == max_steps)
        return NULL;

	possible_moves = g->get_possible_child_states(game_board, current_level_player);

    if (possible_moves == NULL)
        return NULL;

	p = possible_moves->head;

    while (p != NULL) {
        current_move = (move*)p->item;
        best_next_move = minimax_pruned(g, current_move->new_game_state, maximizing_player, next_player, level + 1, max_steps, alpha, beta);

        if (best_next_move == NULL) {
            current_move->score = g->get_state_score(current_move->new_game_state, maximizing_player);

            //prefer short paths
            if (current_move->score == INT_MAX)
                current_move->score -= level;
            if (current_move->score == - INT_MAX)
                current_move->score += level;
        }
        else {
            current_move->score = best_next_move->score;
            minimax_move_release(best_next_move);
            best_next_move = NULL;
        }

        if (best_move == NULL)
            best_move = minimax_move_copy(current_move);

        if (current_level_player == maximizing_player && current_move->score > best_move->score) {
            if (current_move->score > alpha)
                alpha = current_move->score;

            if (best_move != current_move) {
                minimax_move_release(best_move);
                best_move = minimax_move_copy(current_move);
            }
        }
        else if (current_level_player != maximizing_player && current_move->score < best_move->score) {
            if (current_move->score < beta)
                beta = current_move->score;

            if (best_move != current_move) {
                minimax_move_release(best_move);
                best_move = minimax_move_copy(current_move);
            }
        }

        if (beta <= alpha)
            break;

        p = p->next;
    }

    minimax_tree_release(possible_moves);

    return best_move;
}

//relases the instance of minimax move
void minimax_move_release(move* m) {
    if (m->new_game_state != NULL)
        board_release(m->new_game_state);

    m->new_game_state = NULL;

    free(m);
}
//relases all the minimax tree from memory
void minimax_tree_release(linked_list* list) {
    linked_list_node* move_node = NULL;
	move* current_move = NULL;
	if (list == NULL)
        return;

    move_node = list->head;

    while (move_node != NULL) {
        current_move = (move*)move_node->item;

        minimax_move_release(current_move);
        move_node->item = current_move = NULL;

        move_node = move_node->next;
    }

    linked_list_release(list);
}
//copy the move to manage the memory better
move* minimax_move_copy(move* src) {
    move* retval = NULL;
	if (src == NULL)
        return src;

    retval = (move*)malloc(sizeof(move));
	if (retval == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    retval->row = src->row;
    retval->col = src->col;
    retval->player = src->player;
    retval->new_game_state = board_copy(src->new_game_state);
    retval->score = src->score;

    return retval;
}
//creates a move in the minimax and initiates it
move* minimax_move_create(int row, int col, int player, board* new_game_state) {
    move* retval = (move*)malloc(sizeof(move));
	if (retval == NULL) {
		printf("ERROR : Not enough memory");
		exit(1);
	}
    retval->row = row;
    retval->col = col;
    retval->player = player;
    retval->new_game_state = new_game_state;
    retval->score = UNDEFINED;

    return retval;
}

