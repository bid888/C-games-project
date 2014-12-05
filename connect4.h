#ifndef _CONNECT4_H
#define _CONNECT4_H

#define Connect4_ROWS 6
#define Connect4_COLS 7

#include "game.h"

extern game* game_connect4;

void connect4_init();
void connect4_release();

#endif

