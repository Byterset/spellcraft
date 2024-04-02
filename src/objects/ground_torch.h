#ifndef __OBJECTS_GROUND_TORCH_H__
#define __OBJECTS_GROUND_TORCH_H__

#include "../scene/world_definition.h"
#include "../math/transform_single_axis.h"
#include "../render/renderable.h"
#include "../collision/dynamic_object.h"

struct ground_torch {
    struct Vector3 position;
    struct mesh* base_mesh;
    struct mesh* flame_mesh;
    
    struct dynamic_object dynamic_object;
};

void ground_torch_init(struct ground_torch* ground_torch, struct ground_torch_definition* definition);
void ground_torch_destroy(struct ground_torch* ground_torch);

#endif