#include "sol.h"
#include "orxNuklear.h"
#include "gff/gfftypes.h"

static int copy_text(sol_button_t *sb, const char *text) {
    size_t len = strlen(text);
    
    // gff buttons only have 64 chars.
    if (len >= 64) {
        len = 63;
    }

    strncpy(sb->gb.text, text, len);
    sb->gb.text[len] = '\0';
    sb->len = len;

    return EXIT_SUCCESS;
}

extern int sol_create_popup(sol_state_t *state, const char *text) {
    sol_window_load(state, DARKSUN_1, DS1_WINDOW_MESSAGE);
    sol_win_t *win = sol_window_get_top(state);
    sol_win_item_t *item = NULL;

    orxFLOAT x, y;
    orxViewport_GetSize(state->view, &x, &y);

    //printf("%f\n", x);
    //printf("%f\n", y);
    //height of popup is 28
    win->y = sol_scale_down(y/4 + 14);
    // width of popup is 192
    win->x = sol_scale_down((sol_scale_down(x) - 192)) / 2;

    for (int i = 0; i < win->gwin->itemCount; i++) {
        item = win->items + i;
        if (item->type == GFF_BUTN) {
            switch (item->item.button.gb.rh.id) {
                case 10309: copy_text(&item->item.button, text); break;
            }
        }
    }

    win->counter = 75;

    return EXIT_SUCCESS;
}

extern int sol_create_option(sol_state_t *state, const char *title, const char *option1, const char *option2, const char *option3) {
    sol_window_load(state, DARKSUN_1, DS1_WINDOW_POPUP);
    sol_win_t *win = sol_window_get_top(state);
    sol_win_item_t *item = NULL;
    orxVECTOR mpos;
    orxMouse_GetPosition(&mpos);

    win->y = sol_scale_down(mpos.fY);
    win->x = sol_scale_down(mpos.fX);

    for (int i = 0; i < win->gwin->itemCount; i++) {
        item = win->items + i;
        if (item->type == GFF_BUTN) {
            switch (item->item.button.gb.rh.id) {
                case 14004: copy_text(&item->item.button, title); break;
                case 14001: copy_text(&item->item.button, option1); break;
                case 14002: copy_text(&item->item.button, option2); break;
                case 14003: copy_text(&item->item.button, option3); break;
            }
        }
    }

    return EXIT_SUCCESS;
}
