#ifndef BLOCKS_H
#define BLOCKS_H
#include "face_occlusion.h"
#include <stdbool.h>
#include <stdint.h>

#include "../displaylist.h"
#include "../world.h"

#define TEXTURE_INDEX(x, y) (((y)*14) + (x))
#define TEXTURE_X(idx) ((idx) % 14)
#define TEXTURE_Y(idx) ((idx) / 14)

enum block_material {
	MATERIAL_WOOD,
	MATERIAL_STONE,
	MATERIAL_WOOL,
	MATERIAL_ORGANIC,
	MATERIAL_SAND,
	MATERIAL_GLASS,
};

enum side {
	SIDE_TOP = 0,
	SIDE_BOTTOM = 1,
	SIDE_LEFT = 2,
	SIDE_RIGHT = 3,
	SIDE_FRONT = 4,
	SIDE_BACK = 5,
	SIDE_MAX
};

enum block_render_type {
	RENDERTYPE_FULL,
	RENDERTYPE_FULL_OVERLAY,
	RENDERTYPE_HALF,
	RENDERTYPE_STAIRS,
	RENDERTYPE_CROSS,
	RENDERTYPE_CAKE,
	RENDERTYPE_FLAT,
	RENDERTYPE_LADDER,
	RENDERTYPE_CACTUS,
	RENDERTYPE_DOOR,
	RENDERTYPE_TRAPDOOR,
	RENDERTYPE_FLUID,
	RENDERTYPE_LAYER
};

enum block_type {
	BLOCK_AIR = 0,
	BLOCK_STONE = 1,
	BLOCK_GRASS = 2,
	BLOCK_DIRT = 3,
	BLOCK_COBBLESTONE = 4,
	BLOCK_WATER_FLOW = 8,
	BLOCK_WATER_STILL = 9,
	BLOCK_LAVA_FLOW = 10,
	BLOCK_LAVA_STILL = 11,
	BLOCK_SAND = 12,
	BLOCK_GRAVEL = 13,
	BLOCK_LOG = 17,
	BLOCK_LEAVES = 18,
	BLOCK_TALL_GRASS = 31,
	BLOCK_MOSSY_COBBLE = 48,
	BLOCK_OBSIDIAN = 49,
	BLOCK_SNOW = 78,
	BLOCK_ICE = 79,
	BLOCK_PORTAL = 90,
};

#include "aabb.h"

struct block {
	char name[32];
	enum block_render_type (*getRenderType)(struct block_info*);
	enum block_material (*getMaterial)(struct block_info*);
	uint8_t (*getTextureIndex)(struct block_info*, enum side);
	struct face_occlusion* (*getSideMask)(struct block_info*, enum side,
										  struct block_info*);
	bool (*getBoundingBox)(struct block_info*, bool, struct AABB*);
	uint32_t (*getBaseColor)(struct block_info*, enum side);
	size_t (*renderBlock)(struct displaylist*, struct block_info*, enum side,
						  struct block_info*, uint8_t*, bool);
	bool transparent;
	int luminance;
	bool double_sided;
	bool can_see_through;
	bool ignore_lighting;
	union {
		bool cross_random_displacement;
		bool rail_curved_possible;
	} render_block_data;
};

extern struct block block_bedrock;
extern struct block block_slab;
extern struct block block_dirt;
extern struct block block_log;
extern struct block block_stone;
extern struct block block_leaves;
extern struct block block_grass;
extern struct block block_water;
extern struct block block_lava;
extern struct block block_sand;
extern struct block block_sandstone;
extern struct block block_gravel;
extern struct block block_ice;
extern struct block block_snow;
extern struct block block_tallgrass;
extern struct block block_deadbush;
extern struct block block_flower;
extern struct block block_rose;
extern struct block block_furnaceoff;
extern struct block block_furnaceon;
extern struct block block_workbench;
extern struct block block_glass;
extern struct block block_clay;
extern struct block block_coalore;
extern struct block block_ironore;
extern struct block block_goldore;
extern struct block block_diamondore;
extern struct block block_redstoneore;
extern struct block block_lapisore;
extern struct block block_stairs;
extern struct block block_obsidian;
extern struct block block_spawner;
extern struct block block_cobblestone;
extern struct block block_mossstone;
extern struct block block_chest;
extern struct block block_locked_chest;
extern struct block block_cactus;
extern struct block block_pumpkin;
extern struct block block_pumpkin_lit;
extern struct block block_brown_mushroom;
extern struct block block_red_mushroom;
extern struct block block_reed;
extern struct block block_glowstone;
extern struct block block_torch;
extern struct block block_rail;
extern struct block block_powered_rail;
extern struct block block_detector_rail;
extern struct block block_redstone_torch;
extern struct block block_redstone_torch_lit;
extern struct block block_ladder;
extern struct block block_farmland;
extern struct block block_crops;
extern struct block block_planks;
extern struct block block_portal;
extern struct block block_iron;
extern struct block block_gold;
extern struct block block_diamond;
extern struct block block_lapis;
extern struct block block_cake;
extern struct block block_fire;
extern struct block block_double_slab;
extern struct block block_bed;
extern struct block block_sapling;
extern struct block block_bricks;

extern struct block* blocks[256];

#include "../render_block.h"

void blocks_init(void);
enum side blocks_side_opposite(enum side s);
void blocks_side_offset(enum side s, int* x, int* y, int* z);
const char* block_side_name(enum side s);

#endif