#include "sol.h"
#include "orxNuklear.h"
#include "gff/gfftypes.h"

extern int sol_init_new_char(sol_state_t *state) {
    sol_win_t *win = sol_window_get_top(state);
    sol_win_item_t *item = NULL;
    char buf[1024];

    // Need to add 20086 @ 208, 0
    sol_button_t *sb = &win->prev.item.button;
    sb->gb.frame.initbounds.xmin = 208;
    sb->gb.frame.width = 106;
    sb->gb.frame.height = 86;
    sb->gb.rh.id = 20086;
    snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", "ds1", 20086);
    sb->nsb.active = 
    sb->nsb.hover = 
    sb->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(orxTexture_Load(buf, 0))));

    for (int i = 0; i < win->gwin->itemCount; i++) {
        item = win->items + i;
        if (item->type == GFF_BUTN) {
            switch (item->item.button.gb.rh.id) {
                case 2011: // Alignment
                    sol_copy_text(&item->item.button, "* TRUE NEUTRAL"); 
                    break;
                case 2018: // HP
                    sol_copy_text(&item->item.button, "* 0/0"); 
                    break;
                case 2012: // STR
                    sol_copy_text(&item->item.button, "* STR: 0"); 
                    break;
                case 2013: // DEX
                    sol_copy_text(&item->item.button, "* DEX: 0"); 
                    break;
                case 2014: // CON
                    sol_copy_text(&item->item.button, "* CON: 0"); 
                    break;
                case 2015: // INT
                    sol_copy_text(&item->item.button, "* INT: 0"); 
                    break;
                case 2016: // WIS
                    sol_copy_text(&item->item.button, "* WIS: 0"); 
                    break;
                case 2017: // CHA
                    sol_copy_text(&item->item.button, "* CHA: 0"); 
                    break;
            }
        }
    }

    //DS1_WINDOW_GEN_PSIONIC = 3012,
    //DS1_WINDOW_GEN_SPHERES = 3013,
    sol_window_load(state, DARKSUN_1, DS1_WINDOW_GEN_PSIONIC);
    win = sol_window_get_top(state);
    win->x = 208;
    win->y = 88;

    return EXIT_SUCCESS;
}
