/*
 * These are the functions needed to be implmeneted for porting to different systems.
 * This include both OS specific call and callbacks.
 */
#ifndef PORT_H
#define PORT_H

#include "ds-object.h"
#include "region.h"
#include "combat.h"

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_SIZET "%I64u"
#  else
#    define PRI_SIZET "%I32u"
#  endif
#else
#  define PRI_SIZET "%zu"
#endif

#ifdef _WIN32
#  ifdef _WIN64
#    define PRI_LI "%I64d"
#  else
#    define PRI_LI "%I32d"
#  endif
#else
#  define PRI_LI "%lld"
#endif

// Narrate functions
extern int8_t port_narrate_open(int16_t action, const char *text, int16_t index);
extern void port_narrate_clear();
extern void port_narrate_close();

extern int port_ask_yes_no();
extern void port_enter_combat();
extern void port_exit_combat();

extern void port_add_entity(entity_t *entity, gff_palette_t *pal);
extern void port_remove_entity(entity_t *entity);
extern void port_animate_entity(entity_t *obj);
extern void port_swap_enitity(int obj_id, entity_t *dude);
extern void port_update_entity(entity_t *entity, const uint16_t xdiff, const uint16_t ydiff);

extern void port_change_region(region_t *reg);
extern void port_combat_action(combat_action_t *ca);
extern void port_load_item(item_t *item);

#endif
