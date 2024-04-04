#include "sol.h"
#include "util.h"
#include "orxNuklear.h"

#include <stdlib.h>
#include <gff/gfftypes.h>
#include <soloscuro/entity.h>
#include <soloscuro/rules.h>
#include <soloscuro/class.h>

static sol_entity_t *pc = NULL;

typedef struct sol_nuk_image_s {
    struct nk_style_button nsb;
    struct nk_rect         loc;
} sol_nuk_image_t;

static sol_win_t *win = NULL, *psi_win = NULL, *sphere_win = NULL;
static sol_nuk_image_t portraits[14];
static sol_nuk_image_t dice_flip[4];
static sol_nuk_image_t dice[6];
static struct nk_style_button classes[8];
static int dice_state = 0;
static int32_t dice_flipping = 0;
static sol_nuk_image_t classmark;

static int can_add_class(int the_class);

static int make_button(sol_nuk_image_t *sni, const char *res, const int x, const int y) {
    orxTEXTURE *tex = orxTexture_Load(res, 0);
    float w, h;

    if (!tex) {
        return EXIT_FAILURE;
    }

    orxTexture_GetSize (tex, &w, &h);

    sni->nsb.border = 0.0;
    sni->nsb.rounding = 0.0;
    sni->nsb.normal.type = NK_SYMBOL_NONE;
    sni->nsb.hover.type = NK_SYMBOL_NONE;
    sni->nsb.active.type = NK_SYMBOL_NONE;
    sni->nsb.border_color = (struct nk_color){237, 233, 157, 255};
    sni->nsb.padding.x = 0.0;
    sni->nsb.padding.y = 0.0;
    sni->nsb.image_padding.x = 0.0;
    sni->nsb.image_padding.y = 0.0;
    sni->nsb.touch_padding.x = 0.0;
    sni->nsb.touch_padding.y = 0.0;
    sni->nsb.hover = sni->nsb.active = sni->nsb.normal = nk_style_item_image(nk_image_ptr(orxTexture_GetBitmap(tex)));
    sni->loc = sol_nk_rect((x - w) / 2, (y - h) / 2, w, h);

    return EXIT_SUCCESS;
}

static int update_display(sol_state_t *state) {
    sol_win_item_t *item = NULL;
    char buf[1024];

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
                    snprintf(buf, 1023, "* STR: %d", pc->stats.str);
                    sol_copy_text(&item->item.button, buf); 
                    break;
                case 2013: // DEX
                    snprintf(buf, 1023, "* DEX: %d", pc->stats.dex);
                    sol_copy_text(&item->item.button, buf); 
                    break;
                case 2014: // CON
                    snprintf(buf, 1023, "* CON: %d", pc->stats.con);
                    sol_copy_text(&item->item.button, buf); 
                    break;
                case 2015: // INT
                    snprintf(buf, 1023, "* INT: %d", pc->stats.intel);
                    sol_copy_text(&item->item.button, buf); 
                    break;
                case 2016: // WIS
                    snprintf(buf, 1023, "* WIS: %d", pc->stats.wis);
                    sol_copy_text(&item->item.button, buf); 
                    break;
                case 2017: // CHA
                    snprintf(buf, 1023, "* CHA: %d", pc->stats.cha);
                    sol_copy_text(&item->item.button, buf); 
                    break;
            }
        }
    }

    return EXIT_SUCCESS;
}

static void reinit_pc() {
    memset(pc->class, 0x0, sizeof(sol_class_t) * 3);
    pc->class[0].class = REAL_CLASS_FIGHTER;
}

extern int sol_init_new_char(sol_state_t *state) {
    win = sol_window_get_top(state);
    char buf[1024];

    sol_entity_create_default_human(&pc);
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

    sol_window_load(state, DARKSUN_1, DS1_WINDOW_GEN_PSIONIC);
    psi_win = sol_window_take_top(state);
    psi_win->backx = 208;
    psi_win->backy = 88;
    sol_window_load(state, DARKSUN_1, DS1_WINDOW_GEN_SPHERES);
    sphere_win = sol_window_take_top(state);
    sphere_win->backx = 208;
    sphere_win->backy = 88;

    for (int i = 0; i < 14; i++) {
        snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", "ds1", 20000 + i);
        make_button(portraits + i, buf, 102, 121);
    }

    for (int i = 0; i < 4; i++) {
        snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", "ds1", 20049 + i);
        make_button(dice_flip + i, buf, 307, 189);
    }

    for (int i = 0; i < 6; i++) {
        snprintf(buf, 1023, "%s/resource/%d.0.r0.bmp", "ds1", 20053 + i);
        make_button(dice + i, buf, 307, 189);
    }

    snprintf(buf, 1023, "%s/resource/%d.1.r0.bmp", "ds1", 20047);
    make_button(&classmark, buf,  0, 0);

    reinit_pc();
    sol_dnd2e_randomize_stats_pc(pc);
    
    for (int i = 0; i < 8; i++) {
        classes[i] = win->items[i+1].item.button.nsb;
    }

    update_display(state);

    return EXIT_SUCCESS;
}

static int draw_race(sol_state_t *state) {
    int idx = 0;
    switch(pc->race) {
        case RACE_HUMAN:
            idx = 0 + pc->gender - 1;
            break;
        case RACE_DWARF:
            idx = 2 + pc->gender - 1;
            break;
        case RACE_ELF:
            idx = 4 + pc->gender - 1;
            break;
        case RACE_HALFELF:
            idx = 6 + pc->gender - 1;
            break;
        case RACE_HALFGIANT:
            idx = 8 + pc->gender - 1;
            break;
        case RACE_HALFLING:
            idx = 10 + pc->gender - 1;
            break;
        case RACE_MUL:
            idx = 12;
            break;
        case RACE_THRIKREEN:
            idx = 13;
            break;
        default:
            printf("UNKNOWN RACE!\n");
            return EXIT_FAILURE;
    }

    nk_layout_space_push(&sstNuklear.stContext, portraits[idx].loc);

    if (nk_input_is_mouse_pressed(&sstNuklear.stContext.input, NK_BUTTON_RIGHT)) {
        if (nk_input_is_mouse_hovering_rect(&sstNuklear.stContext.input, portraits[idx].loc)) {
            if (pc->gender == GENDER_MALE && pc->race == RACE_HUMAN) {
                pc->race = RACE_THRIKREEN;
            } else if (pc->gender == GENDER_FEMALE && pc->race < RACE_MUL) {
                pc->gender = GENDER_MALE;
            } else {
                pc->race--;
                pc->gender = GENDER_FEMALE;
            }
            reinit_pc();
        }
    }

    if (nk_button_text_styled(&sstNuklear.stContext, &portraits[idx].nsb, "", 0)) {
        if (pc->gender == GENDER_MALE && pc->race < RACE_MUL) {
            pc->gender = GENDER_FEMALE;
        } else {
            pc->race++;
            pc->gender = GENDER_MALE;
        }
        if (pc->race >= RACE_MAX) {
            pc->race = RACE_HUMAN;
            pc->gender = GENDER_MALE;
        }
        reinit_pc();
    }

    return EXIT_SUCCESS;
}

static int get_type(char buf[1024]) {
    char *male = "MALE ";
    char *female = "FEMALE ";
    char *sex = "";
    char *race = "";

    sex = (pc->gender == GENDER_MALE) ? male : female;

    switch(pc->race) {
        case RACE_HUMAN: race = "HUMAN"; break;
        case RACE_DWARF: race = "DWARF"; break;
        case RACE_ELF: race = "ELF"; break;
        case RACE_HALFELF: race = "HALFELF"; break;
        case RACE_HALFGIANT: race = "HALFGIANT"; break;
        case RACE_HALFLING: race = "HALFLING"; break;
        case RACE_MUL: race = "MUL"; sex = ""; break;
        case RACE_THRIKREEN: race = "THRIKREEN"; sex = ""; break;
    }

    snprintf(buf, 1023, "%s%s", sex, race);

    return EXIT_SUCCESS;
}

static int draw_info(sol_state_t *state) {
    char buf[1024];

    get_type(buf);
    nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(92, 134, 80, 15));
    nk_label(&sstNuklear.stContext, buf, NK_TEXT_LEFT);

    //nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(92, 148, 80, 15));
    //nk_label(&sstNuklear.stContext, "FIGHTER", NK_TEXT_LEFT);
    return EXIT_SUCCESS;
}

static int draw_classmark(sol_state_t *state) {
    for (int i = 0; i < 3; i++) {
        int class = pc->class[i].class;
        switch (class) {
            case REAL_CLASS_AIR_CLERIC:
            case REAL_CLASS_EARTH_CLERIC:
            case REAL_CLASS_FIRE_CLERIC:
            case REAL_CLASS_WATER_CLERIC:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 10, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_AIR_DRUID:
            case REAL_CLASS_EARTH_DRUID:
            case REAL_CLASS_FIRE_DRUID:
            case REAL_CLASS_WATER_DRUID:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 18, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_FIGHTER:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 27, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_GLADIATOR:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 35, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_PRESERVER:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 44, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_PSIONICIST:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 52, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_AIR_RANGER:
            case REAL_CLASS_EARTH_RANGER:
            case REAL_CLASS_FIRE_RANGER:
            case REAL_CLASS_WATER_RANGER:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 61, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_THIEF:
                nk_layout_space_push(&sstNuklear.stContext, sol_nk_rect(218, 69, 6, 5));
                nk_button_text_styled(&sstNuklear.stContext, &classmark.nsb, "", 0);
                break;
            case REAL_CLASS_DEFILER:
            case REAL_CLASS_TEMPLAR:
                break;
        }
    }
    return EXIT_SUCCESS;
}

static int has_class(int class) {
    int check[4];
    check[0] = check[1] = check[2] = check[3] = class;

    switch (class) {
        case REAL_CLASS_AIR_CLERIC:
        case REAL_CLASS_EARTH_CLERIC:
        case REAL_CLASS_FIRE_CLERIC:
        case REAL_CLASS_WATER_CLERIC:
            check[0] = REAL_CLASS_AIR_CLERIC;
            check[1] = REAL_CLASS_EARTH_CLERIC;
            check[2] = REAL_CLASS_FIRE_CLERIC;
            check[3] = REAL_CLASS_WATER_CLERIC;
            break;
        case REAL_CLASS_AIR_DRUID:
        case REAL_CLASS_EARTH_DRUID:
        case REAL_CLASS_FIRE_DRUID:
        case REAL_CLASS_WATER_DRUID:
            check[0] = REAL_CLASS_AIR_DRUID;
            check[1] = REAL_CLASS_EARTH_DRUID;
            check[2] = REAL_CLASS_FIRE_DRUID;
            check[3] = REAL_CLASS_WATER_DRUID;
            break;
        case REAL_CLASS_AIR_RANGER:
        case REAL_CLASS_EARTH_RANGER:
        case REAL_CLASS_FIRE_RANGER:
        case REAL_CLASS_WATER_RANGER:
            check[0] = REAL_CLASS_AIR_RANGER;
            check[1] = REAL_CLASS_EARTH_RANGER;
            check[2] = REAL_CLASS_FIRE_RANGER;
            check[3] = REAL_CLASS_WATER_RANGER;
            break;
    }
    for (int i = 0; i < 3; i++) {
        int tc = pc->class[i].class;
        if (tc == check[0] || tc == check[1] || tc == check[2] || tc == check[3]) {
            return i + 1; //returns index + 1.
        }
    }

    return 0;
}

    //if (can_add_class(REAL_CLASS_AIR_CLERIC) == EXIT_SUCCESS) {
static void check_class_button(int class, int idx) {
    struct nk_style_button *but = &win->items[idx + 1].item.button.nsb;

    if (has_class(class) || can_add_class(class) == EXIT_SUCCESS) {
        but->active = classes[idx].active;
        but->hover = classes[idx].hover;
        but->normal = classes[idx].normal;
    } else {
        but->active = but->hover = but->normal = classes[idx].active;
    }
}

extern int sol_new_char_draw(sol_state_t *state) {
    draw_race(state);

    if (dice_flipping == 0) {
        nk_layout_space_push(&sstNuklear.stContext, dice[dice_state].loc);

        if (nk_button_text_styled(&sstNuklear.stContext, &dice[dice_state].nsb, "", 0)) {
            dice_flipping = 120;
            dice_state = rand() % 6;
        }
    } else {
        nk_layout_space_push(&sstNuklear.stContext, dice_flip[dice_flipping / 30].loc);

        if (nk_button_text_styled(&sstNuklear.stContext, &dice_flip[dice_flipping / 30].nsb, "", 0)) {
            dice_flipping = 120;
            dice_state = rand() % 6;
        }
        dice_flipping--;

        if ((dice_flipping % 30) == 0) {
            sol_dnd2e_randomize_stats_pc(pc);
            update_display(state);
        }
    }

    draw_info(state);
    draw_classmark(state);

    check_class_button(REAL_CLASS_AIR_CLERIC, 0);
    check_class_button(REAL_CLASS_AIR_DRUID, 1);
    check_class_button(REAL_CLASS_FIGHTER, 2);
    check_class_button(REAL_CLASS_GLADIATOR, 3);
    check_class_button(REAL_CLASS_PRESERVER, 4);
    check_class_button(REAL_CLASS_PSIONICIST, 5);
    check_class_button(REAL_CLASS_AIR_RANGER, 6);
    check_class_button(REAL_CLASS_THIEF, 7);

    sol_draw_window_components(state, psi_win);

    return EXIT_SUCCESS;
}

static int can_add_class(int the_class) {
    sol_class_t class[3];
    class[0] = pc->class[0];
    class[1] = pc->class[1];
    class[2] = pc->class[2];

    if (class[0].class && class[1].class && class[2].class ) { return EXIT_FAILURE; }

    if (class[0].class == REAL_CLASS_NONE) {
        memset(class + 0, 0x0, sizeof(sol_class_t));
        class[0].class = the_class;
    } else if (class[1].class == REAL_CLASS_NONE) {
        memset(class + 1, 0x0, sizeof(sol_class_t));
        class[1].class = the_class;
    } else if (class[2].class == REAL_CLASS_NONE) {
        memset(class + 2, 0x0, sizeof(sol_class_t));
        class[2].class = the_class;
    }

    return sol_dnd2e_is_class_allowed(pc->race, class) == EXIT_FAILURE;
}

static int add_class(int the_class) {
    sol_class_t class[3];
    class[0] = pc->class[0];
    class[1] = pc->class[1];
    class[2] = pc->class[2];

    if (class[0].class && class[1].class && class[2].class ) { return EXIT_FAILURE; }

    if (class[0].class == REAL_CLASS_NONE) {
        memset(class + 0, 0x0, sizeof(sol_class_t));
        class[0].class = the_class;
    } else if (class[1].class == REAL_CLASS_NONE) {
        memset(class + 1, 0x0, sizeof(sol_class_t));
        class[1].class = the_class;
    } else if (class[2].class == REAL_CLASS_NONE) {
        memset(class + 2, 0x0, sizeof(sol_class_t));
        class[2].class = the_class;
    }

    if (sol_dnd2e_is_class_allowed(pc->race, class) == EXIT_FAILURE) {
        return EXIT_FAILURE;
    }

    memcpy(pc->class, class, sizeof(sol_class_t) * 3);

    return EXIT_SUCCESS;
}

static int add_rm_class(int the_class) {
    int class_idx = has_class(the_class);

    if (class_idx == 0) {
        // Does not have the class, add it.
        return add_class(the_class);
    }

    // Has the class at idx class_idx;
    for (int i = class_idx - 1; i < 2; i++) {
        memcpy(pc->class+i, pc->class+(i+1), sizeof(sol_class_t));
    }

    memset(pc->class+2, 0x0, sizeof(sol_class_t));

    return EXIT_SUCCESS;
}


extern int sol_new_char_click(sol_state_t *state, sol_button_t *button) {
    switch (button->gb.rh.id) {
        case 2002:
            add_rm_class(REAL_CLASS_AIR_CLERIC);
            break;
        case 2003:
            add_rm_class(REAL_CLASS_AIR_DRUID);
            break;
        case 2004:
            add_rm_class(REAL_CLASS_FIGHTER);
            break;
        case 2005:
            add_rm_class(REAL_CLASS_GLADIATOR);
            break;
        case 2006:
            add_rm_class(REAL_CLASS_PRESERVER);
            break;
        case 2007:
            add_rm_class(REAL_CLASS_PSIONICIST);
            break;
        case 2008:
            add_rm_class(REAL_CLASS_AIR_RANGER);
            break;
        case 2009:
            add_rm_class(REAL_CLASS_THIEF);
            break;
    }
    return EXIT_SUCCESS;
}
