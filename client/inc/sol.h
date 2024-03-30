#ifndef SOLOSCURO_H
#define SOLOSCURO_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "gff/gff.h"
#include "gff/manager.h"
#include "orx.h"
#include "orxNuklear.h"

#define WIN_MAX (16)

typedef struct sol_button_s {
    gff_button_t gb;
    struct nk_style_button nsb;
    size_t len;
} sol_button_t;

typedef struct sol_win_item_s {
    uint32_t type;
    union {
        sol_button_t button;
    } item;
} sol_win_item_t;

enum {
    WIN_FREE,
    WIN_IN_USE,
    WIN_TO_DELETE,
};

typedef struct sol_win_s {
    uint8_t         status;
    uint32_t        x, y, counter;
    gff_window_t   *gwin;
    orxBITMAP      *background;
    sol_win_item_t  prev;// Hack, hopefully just need once.
    sol_win_item_t *items;
} sol_win_t;

typedef struct sol_state_s {
    gff_manager_t *gman;
    sol_win_t      swin[WIN_MAX];
    orxVIEWPORT   *view;
} sol_state_t;

extern int sol_register_gff_type(sol_state_t *state);
extern int sol_window_load(sol_state_t *state, gff_game_type_t gt, int32_t res_id);
extern int sol_window_draw(sol_state_t *state);
extern int sol_res_exists(const char *name);
extern int sol_button_fixup(sol_state_t *state, gff_file_t *res, char *game, sol_button_t *sb, int b);
extern int sol_right_button_click(sol_state_t *state, sol_button_t *button);
extern int sol_left_button_click(sol_state_t *state, sol_button_t *button);
extern int sol_create_option(sol_state_t *state, const char *title, const char *option1, const char *option2, const char *option3);
extern int sol_create_popup(sol_state_t *state, const char *text);
extern int sol_close_top_window(sol_state_t *state);
extern int sol_copy_text(sol_button_t *sb, const char *text);
extern int sol_init_new_char(sol_state_t *state);

extern sol_win_t* sol_window_get_top(sol_state_t *state);
extern sol_win_t* sol_window_get_below_top(sol_state_t *state);
extern float      sol_scale_down(float value);

enum sol_windows_type_e {
    DS1_WINDOW_MAINMENU = 3000,
    DS1_WINDOW1 = 3001,
    DS1_WINDOW2 = 3004,
    DS1_WINDOW_NARRATE = 3007,
    DS1_WINDOW4 = 3008,
    DS1_WINDOW_SAVE = 3009,
    DS1_WINDOW_NEW_CHARACTER = 3011,
    DS1_WINDOW_GEN_PSIONIC = 3012,
    DS1_WINDOW_GEN_SPHERES = 3013,
    DS1_WINDOW_INTERACT = 3020,
    DS1_WINDOW_ADD = 3024,
    DS1_WINDOW11 = 3500,
    DS1_WINDOW_GAME_MENU = 10500,
    DS1_WINDOW_MESSAGE = 10501,
    DS1_WINDOW_VIEW_CHARACTER = 11500,
    DS1_WINDOW_INVENTORY = 13500,
    DS1_WINDOW16 = 13501, // Body for container selection?
    DS1_WINDOW_POPUP = 14000,
    DS1_WINDOW_INSPECT = 14001,
    DS1_WINDOW_ALERT2 = 14002,
    DS1_WINDOW_INSPECT2 = 15500,
    DS1_WINDOW_VIEW_ITEM = 15502,
    DS1_WINDOW_SPELL_INFO = 15503,
    DS1_WINDOW_PREFERENCES = 16500,
    DS1_WINDOW_SPELL_TRAIN = 17500,
    DS1_WINDOW_PSI_TRAIN = 17501,
    DS1_WINDOW_DUAL = 17502,
};

enum {
    FONT_TTF_14 = 0,
};

#endif
