#include "sol.h"
#include "orxNuklear.h"
#include "gff/gfftypes.h"

extern int sol_button_fixup(sol_state_t *state, gff_file_t *res, char *game, sol_button_t *sb, int b) {
    char buf[1024];
    orxTEXTURE *tex = NULL;

    sol_win_t *swin = sol_window_get_top(state);
    if (swin == NULL) {
        swin = state->swin;
    } else {
        swin++;
    }

    switch(b) {
        case 11300: // Player 0
        case 11301: // player 1
        case 11302: // player 2
        case 11303: // player 3
        case 11309: // leader 0
        case 11310: // leader 1
        case 11311: // leader 2
        case 11312: // leader 3
        case 11313: // ai 0
        case 11314: // ai 1
        case 11315: // ai 2
        case 11316: // ai 3
        case 11318:
            sb->gb.frame.initbounds.ymin -= 3;
            break;
        case 10300:
        case 11304:
        case 11305:
        case 11306:
        case 11319:
        case 11320:
        case 11308:
        case 10308: // Game return
            if (swin->gwin->rh.id == DS1_WINDOW_VIEW_CHARACTER) {
                //sb->gb.frame.initbounds.xmin -= 2;
                sb->gb.frame.initbounds.ymin -= 10;
            }
            break;
        case 10309: // Message Box
            sb->gb.frame.initbounds.ymin -= 6;
            // fall through
        case 14004: // Other popup box
            sb->nsb.normal = nk_style_item_color((struct nk_color){237, 233, 157, 0});
            sb->nsb.hover = nk_style_item_color((struct nk_color){237, 233, 157, 0});
            sb->nsb.active = nk_style_item_color((struct nk_color){237, 233, 157, 0});
            //sb->text = "";
            //sb->len = 0;
            break;
        case 2001:// Race on New Character
            snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, 13005);
            tex = orxTexture_Load(buf, 0);
            sb->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.hover = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.active = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            break;
        case 2027:// Preview of New Character
            snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, 20083);
            tex = orxTexture_Load(buf, 0);
            sb->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.hover = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.active = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            break;
        case 2010:// Dice Background
            snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, 20085);
            tex = orxTexture_Load(buf, 0);
            sb->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.hover = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            sb->nsb.active = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
            break;
        case 2012: // STR
        case 2013: // DEX
        case 2014: // CON
        case 2015: // INT
        case 2016: // WIS
        case 2017: // CHA
        case 2018:// HP
        case 2011:// Alignment
            sb->nsb.hover = sb->nsb.active =
            sb->nsb.normal = nk_style_item_color((struct nk_color){237, 233, 157, 0});
            break;
    }

    return EXIT_SUCCESS;
}
