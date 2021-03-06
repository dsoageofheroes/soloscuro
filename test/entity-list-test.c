#include "unity.h"
#include "../src/gff.h"
#include "../src/entity.h"
#include "../src/gff-char.h"
#include "../src/rules.h"
#include "../src/dsl.h"
#include "../src/entity-list.h"
#include <string.h>
#include <stdlib.h>

// A hack, but inclue base for default procedures
#include "base.c"

void setUp() {
    gff_init();
    gff_load_directory("ds1");
    dsl_init();
}

void tearDown() {
    dsl_cleanup();
    gff_cleanup();
}

void test_basic(void) {
    entity_list_t *list = entity_list_create();
    entity_t* slig = entity_create_from_objex(-269);
    entity_t* screamer_beatle = entity_create_from_objex(-150);
    int hp_sum = 0;

    entity_list_add(list, slig);
    entity_list_add(list, screamer_beatle);

    entity_t *dude;
    entity_list_for_each(list, dude) {
        hp_sum += dude->stats.hp;
    }

    TEST_ASSERT_EQUAL_INT(33, hp_sum);

    entity_list_free(list);
    entity_free(slig);
    entity_free(screamer_beatle);
}

void test_load_etab(void) {
    char gff_name[32];
    dude_t *dude;
    size_t xsum = 0;
    size_t ysum = 0;
    entity_list_t *list = entity_list_create();

    snprintf(gff_name, 32, "rgn%x.gff", 42);
    int gff_index = gff_find_index(gff_name);
    entity_list_load_etab(list, gff_index, 42);

    entity_list_for_each(list, dude) {
        xsum += dude->mapx;
        ysum += dude->mapy;
    }

    TEST_ASSERT_EQUAL_INT(3222, xsum);
    TEST_ASSERT_EQUAL_INT(2501, ysum);

    entity_list_free(list);
}

int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_basic);
    RUN_TEST(test_load_etab);
    return UNITY_END();
}
