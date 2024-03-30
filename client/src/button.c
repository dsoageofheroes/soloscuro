#include "sol.h"
#include "orxNuklear.h"
#include "gff/gfftypes.h"

static int option_selected(sol_state_t *state, int option) {
    sol_win_t* win = sol_window_get_below_top(state);

    if (win == NULL) {
        printf("NO PTION!\n");
        return EXIT_FAILURE;
    }

    switch (win->gwin->rh.id) {
        case DS1_WINDOW_VIEW_CHARACTER:
            switch (option) {
                case 0:
                    break;
                case 1:
                    return sol_create_popup(state, "NOT IMPLEMENTED");
                    break;
                case 2:
                    return sol_close_top_window(state);
                    break;
            }
            break;
    }

    return EXIT_SUCCESS;
}

extern int sol_right_button_click(sol_state_t *state, sol_button_t *button) {
    printf("Right Button Click: %d\n", button->gb.rh.id);
    switch (button->gb.rh.id) {
        case 11300: // Player 0
        case 11301: // player 1
        case 11302: // player 2
        case 11303: // player 3
            sol_create_option(state,
                    "INACTIVE CHARACTER",
                    "NEW", "ADD", "CANCEL");
            break;
    }
    return EXIT_SUCCESS;
}

extern int sol_left_button_click(sol_state_t *state, sol_button_t *button) {
    printf("Left Button Click: %d\n", button->gb.rh.id);
    switch (button->gb.rh.id) {
        case 10308: // game return
        //case 10309: // message window
             return sol_close_top_window(state);
        case 11300: // Player 0
        case 11301: // player 1
        case 11302: // player 2
        case 11303: // player 3
            break;
        case 11309: // leader 0
        case 11310: // leader 1
        case 11311: // leader 2
        case 11312: // leader 3
        case 11313: // ai 0
        case 11314: // ai 1
        case 11315: // ai 2
        case 11316: // ai 3
        case 14001:
        case 14002:
        case 14003:
            return option_selected(state, button->gb.rh.id - 14001);
    }
    return EXIT_SUCCESS;
}

