#ifndef _MENU_GAME_PANEL_H
#define _MENU_GAME_PANEL_H

#include "game.h"
#include "gui.h"

gui_control* menu_game_panel_create(int player_types[], game* current_game);

void menu_game_panel_show_space_warning(gui_control* side_menu);
void menu_game_panel_hide_space_warning(gui_control* side_menu);

#endif
