#ifndef __SPELL_MANA_POOL_H__
#define __SPELL_MANA_POOL_H__

struct mana_pool_definition {
    float max_mana;
    float mana_regen_rate;
    float power_scale;
};

struct mana_pool {
    float max_mana;
    float current_mana;
    float charged_mana;
    float previous_mana;
    float mana_regen_rate;
    float last_request_time;
    float power_scale;
};

void mana_pool_init(struct mana_pool* pool, struct mana_pool_definition* definition);
void mana_pool_update(struct mana_pool* pool);

float mana_pool_request(struct mana_pool* pool, float amount);
void mana_pool_charge(struct mana_pool* pool, float amount);

float mana_pool_get_previous_mana(struct mana_pool* pool);

float mana_pool_request_charged_mana(struct mana_pool* pool);

#endif