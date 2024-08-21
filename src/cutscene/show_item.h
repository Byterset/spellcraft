#ifndef __CUTSCENE_SHOW_RUNE_H__
#define __CUTSCENE_SHOW_RUNE_H__

#include <stdint.h>
#include <stdbool.h>
#include "cutscene.h"
#include "../render/material.h"

struct show_item {
    struct material* spell_symbol_material;
    uint16_t showing_item;
    uint16_t should_show;
    float show_item_timer;
};

void show_item_init(struct show_item* show_item);
void show_item_start(struct show_item* show_item, union cutscene_step_data* data);
bool show_item_update(struct show_item* show_item, union cutscene_step_data* data);
void show_item_render(struct show_item* show_item);

#endif