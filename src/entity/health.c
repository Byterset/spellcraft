#include "health.h"

#include "../util/hash_map.h"
#include "../time/time.h"
#include <stddef.h>

static struct hash_map health_entity_mapping;

void health_reset() {
    hash_map_destroy(&health_entity_mapping);
    hash_map_init(&health_entity_mapping, 32);
}

void health_update(void *data) {
    struct health* health = (struct health*)data;

    if (health->frozen_timer) {
        health->frozen_timer -= fixed_time_step;

        if (health->frozen_timer < 0.0f) {
            health->frozen_timer = 0.0f;
        }
    }

    if (health->burning_timer) {
        health->burning_timer -= fixed_time_step;

        if (health->burning_timer < 0.0f) {
            health->burning_timer = 0.0f;
        }
    }
}

void health_init(struct health* health, entity_id id, float max_health) {
    health->entity_id = id;
    health->max_health = max_health;
    health->current_health = max_health;
    health->frozen_timer = 0.0f;
    health->burning_timer = 0.0f;

    hash_map_set(&health_entity_mapping, id, health);

    update_add(health, health_update, 0, UPDATE_LAYER_WORLD);

    health->callback = NULL;
    health->callback_data = NULL;
}

void health_destroy(struct health* health) {
    hash_map_delete(&health_entity_mapping, health->entity_id);
}

void health_damage(struct health* health, float amount, entity_id source, enum damage_type type) {
    if (health->max_health) {
        health->current_health -= amount;
    }

    if (type & DAMAGE_TYPE_FIRE) {
        health->burning_timer = 7;
        health->frozen_timer = 0;
    }

    if (type & DAMAGE_TYPE_ICE) {
        health->frozen_timer = 7;
        health->burning_timer = 0;
    }

    if (health->callback) {
        health->callback(health->callback_data, amount, source, type);
    }
}

struct health* health_get(entity_id id) {
    return hash_map_get(&health_entity_mapping, id);
}

bool health_is_burning(struct health* health) {
    return health->burning_timer > 0.0f;
}

bool health_is_frozen(struct health* health) {
    return health->frozen_timer > 0.0f;
}