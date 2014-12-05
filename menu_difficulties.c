#include "gui.h"
#include "game.h"

/* Manage the difficulties of the game */
// choose level of the difficulty 
static void on_lev_click(gui_control* target, gui_control* window, gui_size x, gui_size y) {
    int difficulty = target->state.istate;
    int player = target->parent->state.istate;
    game_set_difficulty(player, difficulty);
    gui_child_pop(window);
}

//crates the menu of the difficulties where you can choose the difficulty 
gui_control* menu_difficulties_create(game* g, int player) {
    gui_control *menu_panel = gui_panel_create(0, 0, WIN_W, WIN_H, NULL);
    int diff_size = 0;
    int* array_diff = g->get_game_difficulties(&diff_size);
    char* images_paths_array[9] = {"images/lev1.png", "images/lev2.png", "images/lev3.png","images/lev4.png","images/lev5.png","images/lev6.png","images/lev7.png","images/lev8.png","images/lev9.png"};

    //Load images according to the game's difficulties array
    int base = 70;
    int height = 80;
    int i;
    gui_control *level;

    menu_panel->state.istate = player;

    for (i=0; i < diff_size; i++) {
        level = gui_button_create((WIN_W -230)/2, base + height*i, images_paths_array[array_diff[i] - 1], on_lev_click);

        if (level == NULL) {
            printf("ERROR: failed to load image: '%s'\n", gui_last_error());
            exit(1);
        }

        level->state.istate = array_diff[i];
        gui_child_add(menu_panel, level);
    }

    gui_set_name(menu_panel, "select difficulty");

    return menu_panel;
}
