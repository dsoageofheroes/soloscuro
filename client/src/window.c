#include "sol.h"
#include "util.h"
#include "orxNuklear.h"
#include "gff/gfftypes.h"

static int close_window(sol_state_t *state, sol_win_t *win);

static int load_button(sol_state_t *state, gff_file_t *res, char *game, sol_button_t *sb, gff_gui_item_t *item) {
    char buf[1024];
    orxTEXTURE *tex = NULL;
    gff_button_t *gb = &sb->gb;

    sb->nsb = sstNuklear.stContext.style.button;
    gff_read_button(res, item->id, gb);
    gb->frame.initbounds = item->init_bounds;

    // Normal
    snprintf(buf, 1023, "%s/resource/%d.0.r0.icon", game, gb->icon_id);
    if (sol_res_exists(buf) == EXIT_FAILURE) {
        goto no_image;
    }

    sb->nsb.border = 0.0;
    sb->nsb.rounding = 0.0;
    sb->nsb.normal.type = NK_SYMBOL_NONE;
    sb->nsb.hover.type = NK_SYMBOL_NONE;
    sb->nsb.active.type = NK_SYMBOL_NONE;
    sb->nsb.border_color = (struct nk_color){237, 233, 157, 255};
    sb->nsb.padding.x = 0.0;
    sb->nsb.padding.y = 0.0;
    sb->nsb.image_padding.x = 0.0;
    sb->nsb.image_padding.y = 0.0;
    sb->nsb.touch_padding.x = 0.0;
    sb->nsb.touch_padding.y = 0.0;

    sb->len = 0;

    tex = orxTexture_Load(buf, 0);
    if (tex) {
        sb->nsb.active =
        sb->nsb.hover =
        sb->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
        //printf("%d %d\n", gb->frame.initbounds.xmin, gb->frame.initbounds.ymin);
    }

    //hover
    snprintf(buf, 1023, "%s/resource/%d.1.r0.icon", game, gb->icon_id);
    tex = orxTexture_Load(buf, 0);
    if (tex) {
        sb->nsb.hover = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
        //printf("%d %d\n", gb->frame.initbounds.xmin, gb->frame.initbounds.ymin);
    }

    snprintf(buf, 1023, "%s/resource/%d.2.r0.icon", game, gb->icon_id);
    /*
    if (sol_res_exists(buf) == EXIT_FAILURE) {
        snprintf(buf, 1023, "%s/resource/%d.2.r0.icon", game, gb->icon_id);
        //if (sol_res_exists(buf) == EXIT_FAILURE) {
            //snprintf(buf, 1023, "%s/resource/%d.1.r0.icon", game, gb->icon_id);
        //}
    }
    */
    tex = orxTexture_Load(buf, 0);
    if (tex) {
        sb->nsb.active = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
    }

    goto finished;
no_image:

finished:
    return sol_button_fixup(state, res, game, sb, item->id);
}

static int load_ebox(sol_state_t *state, gff_file_t *res, char *game, sol_ebox_t *eb, gff_gui_item_t *item) {
    gff_ebox_t *ge = &eb->ge;

    gff_read_ebox(res, item->id, ge);

    ge->frame.initbounds = item->init_bounds;
    //uint16_t  max_lines, styles, runs, size, user_id;
    printf("%d %d %d %d %d\n", ge->max_lines, ge->styles, ge->runs, ge->size, ge->user_id);
    return EXIT_SUCCESS;
}

extern int sol_window_load(sol_state_t *state, gff_game_type_t gt, int32_t res_id) {
    int status = EXIT_FAILURE;
    char buf[1024];
    char *game = NULL;
    orxTEXTURE *tex = NULL;
    int32_t id = 0;
    uint8_t *u8ptr = NULL;
    gff_file_t *res;
    sol_win_t *win = NULL;

    for (win = state->swin; win < state->swin + WIN_MAX && win->status != WIN_FREE; win++) {
    }

    if (win >= state->swin + WIN_MAX) {
        return EXIT_FAILURE;
    }

    memset(win, 0x0, sizeof(sol_win_t));

    switch (gt) {
        case DARKSUN_1:
            res = state->gman->ds1.resource;
            status = gff_manager_load_window(&state->gman->ds1, res_id, &win->gwin);
            game = "ds1";
            break;
        case DARKSUN_2:
        case DARKSUN_ONLINE:
        case DARKSUN_UNKNOWN:
            goto unsupported_game;
        
    }

    // Take some guesses on the background:
    if (win->gwin->frame.background_bmp) {
        snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, win->gwin->frame.background_bmp);
        tex = orxTexture_Load(buf, 1);
        if (tex) {
            win->background = orxTexture_GetBitmap(tex);
            goto found_background;
        }
    }

    if (win->gwin->frame.border_bmp) {
        snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, win->gwin->frame.border_bmp);
        tex = orxTexture_Load(buf, 1);
        if (tex) {
            win->background = orxTexture_GetBitmap(tex);
            goto found_background;
        }
    }

    snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, win->gwin->rh.id);
    tex = orxTexture_Load(buf, 1);
    if (tex) {
        win->background = orxTexture_GetBitmap(tex);
        goto found_background;
    }

    // Nothing found. Try some hardcoded values
    switch (win->gwin->rh.id) {
        case DS1_WINDOW_MAINMENU: id = 20029; break;
        case DS1_WINDOW_NEW_CHARACTER: id = 13001; break;
        case DS1_WINDOW_GEN_PSIONIC: id = 20087; break;
        case DS1_WINDOW_GEN_SPHERES: id = 20087; break;
        case DS1_WINDOW_GAME_MENU: id = 10000; break;
        case DS1_WINDOW_MESSAGE: id = 10001; break;
        case DS1_WINDOW_VIEW_CHARACTER: id = 11000; break;
        case DS1_WINDOW_INVENTORY: id = 13001; break;
        case DS1_WINDOW_ALERT2: id = 20087; break;
        case DS1_WINDOW_INSPECT2: id = 20086; break;
        case DS1_WINDOW_VIEW_ITEM: id = 20087; break;
        case DS1_WINDOW_SPELL_INFO: id = 20085; break;
        case DS1_WINDOW_PREFERENCES: id = 10000; break;
        case DS1_WINDOW_SPELL_TRAIN: id = 17000; break;
        case DS1_WINDOW_PSI_TRAIN: id = 17001; break;
        case DS1_WINDOW_DUAL: id = 17002; break;
        case DS1_WINDOW_ADD: id = 3009; break;
    }

    snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", game, id);
    tex = orxTexture_Load(buf, 1);
    if (tex) {
        win->background = orxTexture_GetBitmap(tex);
        goto found_background;
    }

    win->background = NULL;

found_background:
    u8ptr = (uint8_t*)win->gwin;
    win->items = malloc(sizeof(sol_win_item_t) * win->gwin->itemCount);
    for (int i = 0; i < win->gwin->itemCount; i++) {
        gff_gui_item_t *item = (gff_gui_item_t*)(u8ptr + sizeof(gff_window_t) + 12 + i *(sizeof(gff_gui_item_t)));
        win->items[i].type = item->type;
        switch (item->type) {
            case GFF_ACCL:
                //print_accl(gff, item->id);
                //printf("ACCL\n");
                break;
            case GFF_APFM:
                //print_frame(gff, item->id);
                printf("APFM: %d\n", item->id);
                break;
            case GFF_BUTN:
                load_button(state, res, game, &win->items[i].item.button, item);
                printf("BUTTON: %d, text: '%s'\n", item->id, win->items[i].item.button.gb.text);
                break;
            case GFF_EBOX:
                //print_ebox(gff, item->id);
                load_ebox(state, res, game, &win->items[i].item.ebox, item);
                printf("EBOX: %d\n", item->id);
                break;
            default:
                printf("UNKNOWN TYPE IN WINDOW: %d\n", item->type);
        }
    }

    win->status = WIN_IN_USE;

    for (int i = 0; i < win->gwin->itemCount; i++) {
        win->items[i].item.button.len = 0;
    }

    switch(res_id) {
        case DS1_WINDOW_NEW_CHARACTER:sol_init_new_char(state); break;
    }

    return status;
unsupported_game:
    return EXIT_FAILURE;
}

static float scale = 2.0;

extern struct nk_rect sol_nk_rect(float x, float y, float w, float h) {
    return nk_rect(x * scale, y * scale, w * scale, h * scale);
}

extern float sol_scale_down(float value) { return value / scale; }

static int draw_button(sol_state_t *state, sol_win_t *win, sol_button_t *button) {
    struct nk_rect loc = 
            sol_nk_rect(win->backx + button->gb.frame.initbounds.xmin, win->backy + button->gb.frame.initbounds.ymin, button->gb.frame.width, button->gb.frame.height);

    if (nk_input_is_mouse_pressed(&sstNuklear.stContext.input, NK_BUTTON_RIGHT)) {
        if (nk_input_is_mouse_hovering_rect(&sstNuklear.stContext.input, loc)) {
            sol_right_button_click(state, button);
        }
    }

    nk_layout_space_push(&sstNuklear.stContext, loc);

    nk_style_set_font(&sstNuklear.stContext, &sstNuklear.apstFonts[FONT_TTF_14]->handle);
    /*
    button->nsb.text_active =
    button->nsb.text_hover =
    button->nsb.text_normal = nk_rgb(255, 0, 0);
    */
    if (nk_button_text_styled(&sstNuklear.stContext, &button->nsb, button->gb.text, button->len)) {
        sol_left_button_click(state, button);
    }
    
    return EXIT_SUCCESS;
}

extern int sol_draw_window_components(sol_state_t *state, sol_win_t *win) {
    struct nk_rect back_loc = sol_nk_rect(win->backx, win->backy, win->gwin->frame.width, win->gwin->frame.height);
    // Push the background
    nk_layout_space_push(&sstNuklear.stContext, back_loc);
    nk_image(&sstNuklear.stContext, nk_image_ptr(win->background));

    if (win->prev.item.button.gb.frame.initbounds.xmin) {
        draw_button(state, win, &win->prev.item.button);
    }
    // Draw all the items
    for (int i = 0; i < win->gwin->itemCount; i++) {
        switch (win->items[i].type) {
            case GFF_BUTN:
                draw_button(state, win, &win->items[i].item.button);
                //if (nk_button_label(&sstNuklear.stContext, "button"))
        }
    }
    //sstNuklear.stContext.style.window.fixed_background = nk_style_item_image(nk_image_ptr(bmp));
    /*
    enum {EASY, HARD};
    static orxS32 Op = EASY;
    static orxS32 Property = 20;

    sstNuklear.stContext.style.window.fixed_background;
    //orxBITMAP *pstBitmap = orxTexture_GetBitmap(sstNuklear.astSkins[sstNuklear.s32SkinCount].pstTexture);
    //sstNuklear.astSkins[sstNuklear.s32SkinCount].stImage = nk_image_ptr(pstBitmap);

    nk_layout_space_begin(&sstNuklear.stContext, NK_STATIC, 150, 150);
    nk_layout_space_push(&sstNuklear.stContext, nk_rect(0, 0, 50, 50));
    //nk_layout_row_static(&sstNuklear.stContext, 30, 80, 1);
    if (nk_button_label(&sstNuklear.stContext, "button"))
    {
    orxLOG("Nuklear button pressed.");
    }
    //nk_layout_row_dynamic(&sstNuklear.stContext, 30, 2);
    nk_layout_space_push(&sstNuklear.stContext, nk_rect(0, 0, 50, 50));
    if(nk_option_label(&sstNuklear.stContext, "easy", Op == EASY))
    {
        Op = EASY;
    }
    */
    /*
    if(nk_option_label(&sstNuklear.stContext, "hard", Op == HARD))
    {
        Op = HARD;
    }
    //nk_layout_row_dynamic(&sstNuklear.stContext, 25, 1);
    nk_property_int(&sstNuklear.stContext, "Compression:", 0, &Property, 100, 10, 1);
    nk_edit_string_zero_terminated(&sstNuklear.stContext, NK_EDIT_FIELD, username, sizeof(username) - 1, nk_filter_default);
    */
    return EXIT_SUCCESS;
}

static int draw_window(sol_state_t *state, sol_win_t *win, const char *name) {
    //printf("window '%s': %d, %d\n", name, win->gwin->frame.width, win->gwin->frame.height);
    struct nk_rect win_loc = sol_nk_rect(win->x, win->y, win->gwin->frame.width, win->gwin->frame.height);

    if(nk_begin(&sstNuklear.stContext, name, win_loc, NK_WINDOW_MOVABLE | NK_WINDOW_NO_SCROLLBAR ))
    {
        sstNuklear.stContext.style.window.fixed_background = nk_style_item_color(nk_rgba(0,0,0,0));
        nk_layout_space_begin(&sstNuklear.stContext, NK_STATIC, scale * win->gwin->frame.width, scale * win->gwin->frame.height);
        sol_draw_window_components(state, win);
    }
    switch (win->gwin->rh.id) {
        case DS1_WINDOW_NEW_CHARACTER: sol_new_char_draw(state); break;
    }
    nk_end(&sstNuklear.stContext);

    // If the window has a count down, decrement and then close if needed.
    if (win->counter > 0) {
        if (--win->counter == 0) { close_window(state, win); }
    }

    return EXIT_SUCCESS;
}

extern int sol_window_draw(sol_state_t *state) {
    char buf[1024];
    int i = 0;

    //for (sol_win_t *win = state->swin; win < state->swin + WIN_MAX && win->status != WIN_FREE; win++) {
    for (sol_win_t *win = state->swin; win < state->swin + WIN_MAX; win++) {
        sprintf(buf, "%d", i++);
        if (win->status == WIN_IN_USE) {
            draw_window(state, win, buf);
        }
    }

    // cleanup
    for (sol_win_t *win = state->swin; win < state->swin + WIN_MAX; win++) {
        if (win->status == WIN_TO_DELETE) {
            close_window(state, win);
        }
    }

    return EXIT_SUCCESS;
}

extern sol_win_t* sol_window_take_top(sol_state_t *state) {
    sol_win_t *win = sol_window_get_top(state), *ret = NULL;

    if (!win) {
        return NULL;
    }

    ret = malloc(sizeof(sol_win_t));
    memcpy(ret, win, sizeof(sol_win_t));
    win->status = WIN_FREE;

    return (ret);
}

extern sol_win_t* sol_window_give_top(sol_state_t *state, sol_win_t *src) {
    sol_win_t *win = sol_window_get_top(state);

    if (!win) {
        return NULL;
    }

    memcpy(win, src, sizeof(sol_win_t));

    free(src);

    return (win);
}

extern sol_win_t* sol_window_get_top(sol_state_t *state) {
    sol_win_t *win = NULL;
    for (win = state->swin + WIN_MAX - 1; win >= state->swin && win->status == WIN_FREE; win--) {
    }

    if (win < state->swin) {
        return NULL;
    }

    return (win);
}

extern int sol_close_top_window(sol_state_t *state) {
    sol_win_t *win = sol_window_get_top(state);

    if (win) {
        win->status = WIN_TO_DELETE;
    }

    return EXIT_SUCCESS;
}

extern sol_win_t* sol_window_get_below_top(sol_state_t *state) {
    sol_win_t *win = sol_window_get_top(state);

    while (win != state->swin) {
        win--;
        if (win->status == WIN_IN_USE) {
            return win;
        }
    }

    return NULL;
}

static int close_window(sol_state_t *state, sol_win_t *win) {
    if (win->items) {
        free(win->items);
        win->items = NULL;
    }

    if (win->gwin) {
        free(win->gwin);
        win->gwin = NULL;
    }

    win->status = WIN_FREE;
    return EXIT_SUCCESS;
}
