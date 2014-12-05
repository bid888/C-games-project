#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include "menu_newgame.h"
#include "menu_players.h"
#include "game.h"

#define SLOTS 5
#define LOAD_MODE 1
#define SAVE_MODE 2
#define LINE_MAX_LEN 100

/* Manages the SAVE LOAD functions */

//Saves the current game
static void save_game(gui_control* window, int slot) {
    FILE* f;
    char fileName[100];
	int player_map[] = { 0, 1, -1 };

    int i, j;

    game* g = game_get_current();
    board* game_board = game_get_current_board();
    int player = game_get_current_player();

    sprintf(fileName, "saveload%d.data", slot);
    f = fopen(fileName, "w");

    fprintf(f, "%s\n", g->name);
    fprintf(f, "%d\n", player);

    for (i = 0; i < game_board->rows; i++) {
        fprintf(f, "\n");
        for (j = 0; j < game_board->cols; j++) {
            fprintf(f, "%d ", player_map[board_get(game_board, i, j)]);
        }
    }

    fclose(f);
}
//loads the saved game from a slot
static void load_game(gui_control* window, int slot) {
    char fileName[100];
    char gameName[50];
	int player_map[] = { 2, 0, 1 };

	gui_control* players_menu;

    int player;
    board* game_board;
    linked_list* games;
    linked_list_node* game_node;
    game* g;

    int i, j, val;

    FILE* f;

    sprintf(fileName, "saveload%d.data", slot);
    f = fopen(fileName, "r");
    if (f == NULL) {
        printf("no game found for saved game %d\n", slot);
        return;
    }
    fscanf(f, "%s", gameName);
    fscanf(f, "%d", &player);

    games = games_get();
    game_node = games->head;

    while (game_node != NULL) {
        g = (game*)game_node->item;
        if (strcmp(g->name, gameName) == 0) break;
        game_node = game_node->next;
    }

    if (game_node == NULL) {
        printf("no game found for saved game %d\n", slot);
        return;
    }

    game_board = g->create_initial_state();
    for (i = 0; i < game_board->rows; i++) {
        for (j = 0; j < game_board->cols; j++) {
            fscanf(f, "%d", &val);
			val = player_map[val + 1]; // -1 => 2; 0 => 0; 1=>1
            board_set(game_board, i, j ,val);
        }
    }

    fclose(f);

    game_set_next_game(g);
	game_set_game_state(game_board, player);
    players_menu = menu_players_create();
    gui_child_push(window, players_menu);
}
//on slot click load the game or save it 
static void on_slot_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    int slot = target->state.istate;
    int mode = target->parent->state.istate;
    gui_child_pop(window);

    if (mode == LOAD_MODE)
        load_game(window, slot);
    else if (mode == SAVE_MODE)
        save_game(window, slot);
}

//return to previous panel
static void on_back_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    gui_child_pop(window);
}

//cross platform method to test existance - not perfect but good enough for win/linux/mac
static int file_exists(const char* filename) {
	FILE* file;
	if ((file = fopen(filename, "r")) == NULL) {
		return 0;
	}
	fclose(file);
	return 1;
}

//creates the slot menu for saving and loading
static gui_control* menu_create(int state) {
    gui_control *menu_panel = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);
    gui_control *slot = NULL;
	gui_control *back = NULL;

    //Load images
    int base = 100;
    int height = 80;
    char fileName[100];
    int i;

    for (i = 0; i < SLOTS; i++) {
        sprintf(fileName, "./saveload%d.data", i);
        //file not found
		if (state == LOAD_MODE && !file_exists(fileName)) {
            continue;
        }

        slot = gui_button_create((WIN_W -230)/2, base + i * height, "images/slot.png", &on_slot_click);
        slot->state.istate = i;
        if (slot == NULL) {
            printf("ERROR: failed to load image: '%s'\n", gui_last_error());
            exit(1);
        }
        gui_child_add(menu_panel, slot);
    }

    back = gui_button_create((WIN_W -230)/2, base + i * height, "images/back.png", &on_back_click);
    gui_set_name(menu_panel, "save/load menu");

    gui_child_add(menu_panel, back);
    menu_panel->state.istate = state;

    return menu_panel;
}

//create the load menu
gui_control* load_game_menu_create() {
    gui_control* loadgame_menu = menu_create(LOAD_MODE);
    return loadgame_menu;
}
//create the save menu
gui_control* save_game_menu_create() {
    gui_control* savegame_menu = menu_create(SAVE_MODE);
    return savegame_menu;
}
