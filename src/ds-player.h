#ifndef DS_PLAYER_H
#define DS_PLAYER_H

#include "entity.h"

#define MAX_PCS (4)

// New Interface
void ds_player_init();
extern int player_exists(const int slot);
extern entity_t* player_get_entity(const int slot);
extern void player_cleanup();
extern entity_t* player_get_active();
extern int player_get_slot(entity_t *entity);
extern void player_free(const int slot);

#endif
