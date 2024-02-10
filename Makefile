V=1
SOURCE_DIR=src
BUILD_DIR=build
include $(N64_INST)/include/n64.mk

all: spellcraft.z64
.PHONY: all

###
# images
###

PNG_RGBA16 := $(shell find assets/ -type f -name '*.RGBA16.png' | sort)

MKSPRITE_FLAGS ?=

SPRITES := $(PNG_RGBA16:assets/%.RGBA16.png=filesystem/%.sprite)

filesystem/%.sprite: assets/%.RGBA16.png
	@mkdir -p $(dir $@)
	@echo "    [SPRITE] $@"
	@$(N64_MKSPRITE) -f RGBA16 --compress -o "$(dir $@)" "$<"

###
# meshes
###

EXPORT_SOURCE := $(shell find tools/mesh_export/ -type f -name '*.py' | sort)
BLENDER_4 := /home/james/Blender/blender-4.0.2-linux-x64/blender

MESH_SOURCES := $(shell find assets/meshes -type f -name '*.blend' | sort)

MESHES := $(MESH_SOURCES:assets/meshes/%.blend=filesystem/meshes/%.mesh)

filesystem/meshes/%.mesh: assets/meshes/%.blend $(EXPORT_SOURCE)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(@:filesystem/meshes/%.mesh=build/assets/meshes/%.mesh))
	$(BLENDER_4) $< --background --python tools/mesh_export/mesh.py -- $(@:filesystem/meshes/%.mesh=build/assets/meshes/%.mesh)
	mkasset -o $(dir $@) -w 256 $(@:filesystem/meshes/%.mesh=build/assets/meshes/%.mesh)

###
# materials
###

MATERIAL_SOURCES := $(shell find assets/ -type f -name '*.mat.json' | sort)

MATERIALS := $(MATERIAL_SOURCES:assets/%.mat.json=filesystem/%.mat)

filesystem/%.mat: assets/%.mat.json $(EXPORT_SOURCE)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(@:filesystem/%.mat=build/assets/%.mat))
	python tools/mesh_export/material.py --default assets/materials/default.mat.json $< $(@:filesystem/%.mat=build/assets/%.mat)
	mkasset -o $(dir $@) -w 4 $(@:filesystem/%.mat=build/assets/%.mat)

###
# worlds
###

WORLD_SOURCES := $(shell find assets/worlds -type f -name '*.blend' | sort)

WORLDS := $(MESH_SOURCES:assets/worlds/%.blend=filesystem/worlds/%.world)

filesystem/worlds/%.world: assets/worlds/%.blend $(EXPORT_SOURCE)
	@mkdir -p $(dir $@)
	@mkdir -p $(dir $(@:filesystem/worlds/%.world=build/assets/worlds/%.world))
	$(BLENDER_4) $< --background --python tools/mesh_export/world.py -- $(@:filesystem/worlds/%.world=build/assets/worlds/%.world)
	mkasset -o $(dir $@) -w 256 $(@:filesystem/worlds/%.world=build/assets/worlds/%.world)

###
# source code
###

SOURCES := $(shell find src/ -type f -name '*.c' | sort)
SOURCE_OBJS := $(SOURCES:src/%.c=$(BUILD_DIR)/%.o)
OBJS := $(BUILD_DIR)/main.o $(SOURCE_OBJS)

$(BUILD_DIR)/spellcraft.dfs: $(SPRITES) $(MESHES) $(MATERIALS) $(WORLDS)
$(BUILD_DIR)/spellcraft.elf: $(OBJS)

spellcraft.z64: N64_ROM_TITLE="SpellCraft"
spellcraft.z64: $(BUILD_DIR)/spellcraft.dfs

clean:
	rm -rf $(BUILD_DIR)/* filesystem/ *.z64
.PHONY: clean

clean-filesystem:
	rm -rf filesystem/ 
.PHONY: clean-resource

-include $(wildcard $(BUILD_DIR)/*.d)