#ifndef ENTITY_LIST_H
#define ENTITY_LIST_H

#include "entity.h"

typedef struct entity_list_node_s {
    entity_t *entity;
    struct entity_list_node_s *next, *prev;
} entity_list_node_t;

typedef entity_list_node_t* entity_list_t;

entity_list_t* entity_list_create();
void entity_list_free(entity_list_t *list); // just the list
void entity_list_free_all(entity_list_t *list); // the list and the entities
void entity_list_add(entity_list_t *list, entity_t *entity);
void entity_list_remove(entity_list_t *list, entity_list_node_t *node);
entity_list_node_t* entity_list_find(entity_list_t *list, entity_t *entity);
void entity_list_load_etab(entity_list_t *list, const int gff_idx, const int map_id);

#define entity_list_for_each(el, obj) \
    obj = (el && *el) ? (*el)->entity : NULL; \
    for (entity_list_node_t *__el_rover = (*el); __el_rover;\
        __el_rover = __el_rover->next, obj = __el_rover ? __el_rover->entity : NULL)

#endif
