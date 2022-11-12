#ifndef RENDER_BLOCK_H
#define RENDER_BLOCK_H

#include <stdbool.h>
#include <stddef.h>

#include "block/blocks.h"
#include "displaylist.h"
#include "world.h"

size_t render_block_cross(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_fluid(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_full(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_slab(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_bed(struct displaylist* d, struct block_info* this,
						enum side side, struct block_info* it,
						uint8_t* vertex_light, bool count_only);

size_t render_block_farmland(struct displaylist* d, struct block_info* this,
							 enum side side, struct block_info* it,
							 uint8_t* vertex_light, bool count_only);

size_t render_block_crops(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_torch(struct displaylist* d, struct block_info* this,
						  enum side side, struct block_info* it,
						  uint8_t* vertex_light, bool count_only);

size_t render_block_cactus(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_portal(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_rail(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

size_t render_block_ladder(struct displaylist* d, struct block_info* this,
						   enum side side, struct block_info* it,
						   uint8_t* vertex_light, bool count_only);

size_t render_block_cake(struct displaylist* d, struct block_info* this,
						 enum side side, struct block_info* it,
						 uint8_t* vertex_light, bool count_only);

#endif