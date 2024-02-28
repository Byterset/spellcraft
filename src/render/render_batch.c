#include "render_batch.h"

#include "../util/sort.h"

void render_batch_init(struct render_batch* batch) {
    batch->element_count = 0;
    batch->transform_count = 0;
    batch->sprite_count = 0;
}

struct render_batch_element* render_batch_add(struct render_batch* batch) {
    if (batch->element_count >= RENDER_BATCH_MAX_SIZE) {
        return NULL;
    }

    struct render_batch_element* result = &batch->elements[batch->element_count];
    ++batch->element_count;

    result->material = NULL;
    result->type = RENDER_BATCH_MESH;
    result->mesh.list = 0;
    result->mesh.transform = NULL;

    return result;
}

void render_batch_add_mesh(struct render_batch* batch, struct mesh* mesh, mat4x4* transform) {
    for (int i = 0; i < mesh->submesh_count; ++i) {
        struct render_batch_element* element = render_batch_add(batch);

        if (!element) {
            return;
        }

        element->mesh.list = mesh->list + i;
        element->material = mesh->materials[i];
        element->mesh.transform = transform;
    }
}

struct render_batch_billboard_element render_batch_get_sprites(struct render_batch* batch, int count) {
    struct render_batch_billboard_element result;

    result.sprites = &batch->sprites[batch->sprite_count];

    if (batch->sprite_count + count > RENDER_BATCH_TRANSFORM_COUNT) {
        result.sprite_count = RENDER_BATCH_TRANSFORM_COUNT - batch->sprite_count;
    } else {
        result.sprite_count = count;
    }

    batch->sprite_count += result.sprite_count;

    return result;
}

mat4x4* render_batch_get_transform(struct render_batch* batch) {
    if (batch->transform_count >= RENDER_BATCH_TRANSFORM_COUNT) {
        return NULL;
    }

    mat4x4* result = &batch->transform[batch->transform_count];
    ++batch->transform_count;
    return result;
}

int render_batch_compare_element(struct render_batch* batch, uint16_t a_index, uint16_t b_index) {
    struct render_batch_element* a = &batch->elements[a_index];
    struct render_batch_element* b = &batch->elements[b_index];

    if (a == b) {
        return 0;
    }

    if (a->material->sortPriority != b->material->sortPriority) {
        return a->material->sortPriority - b->material->sortPriority;
    }

    if (a->material != b->material) {
        return (int)a->material - (int)b->material;
    }

    return a->type - b->type;
}

void render_batch_finish(struct render_batch* batch, mat4x4 view_proj_matrix) {
    uint16_t order[RENDER_BATCH_MAX_SIZE];
    uint16_t order_tmp[RENDER_BATCH_MAX_SIZE];

    for (int i = 0; i < batch->element_count; ++i) {
        order[i] = i;
    }

    sort_indices(order, batch->element_count, batch, (sort_compare)render_batch_compare_element);

    struct material* current_mat = 0;

    glEnable(GL_CULL_FACE);
    glEnable(GL_RDPQ_MATERIAL_N64);
    // glEnable(GL_RDPQ_TEXTURING_N64);
    rdpq_set_mode_standard();

    bool is_sprite_mode = false;

    for (int i = 0; i < batch->element_count; ++i) {
        int index = order[i];
        struct render_batch_element* element = &batch->elements[index];

        if (current_mat != element->material) {
            glCallList(element->material->list);
            current_mat = element->material;
        }

        if (element->type == RENDER_BATCH_MESH) {
            if (is_sprite_mode) {
                rdpq_mode_zoverride(false, 0, 0);
                rdpq_mode_persp(true);
                is_sprite_mode = false;
            }

            if (!element->mesh.list) {
                continue;
            }

            if (element->mesh.transform) {
                glPushMatrix();
                glMultMatrixf((GLfloat*)element->mesh.transform);
            }

            glCallList(element->mesh.list);

            if (element->mesh.transform) {
                glPopMatrix();
            }
        } else if (element->type == RENDER_BATCH_BILLBOARD) {
            is_sprite_mode = true;

            for (int sprite_index = 0; sprite_index < element->billboard.sprite_count; ++sprite_index) {
                struct render_billboard_sprite sprite = element->billboard.sprites[sprite_index];

                struct Vector4 transformed;
                matrixVec3Mul(view_proj_matrix, &sprite.position, &transformed);

                float wInv = 1.0f / transformed.w;

                // float x = transformed.x * wInv;
                // float y = transformed.y * wInv;
                float z = transformed.z * wInv * -0.5f + 0.5f;

                if (z < 0.0f || z > 1.0f) {
                    continue;
                }

                rdpq_mode_zoverride(true, z, 0);
                rdpq_mode_persp(false);

                rdpq_texture_rectangle(TILE0, 0, 0, 128, 128, 0, 0);
            }
        }
    }

    glDisable(GL_RDPQ_MATERIAL_N64);
    glDisable(GL_RDPQ_TEXTURING_N64);
}