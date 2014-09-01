/* ========================================================================== */
/* 
 * home.c
 *
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Home window initialization, operations, handlers.
 */
/* ========================================================================== */

// ---------------- Open Issues
// shorten save time by saving whole structure

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>
#include <ctype.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "sprite.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define SPRITE_WIDTH 45
#define SPRITE_HEIGHT 45
#define SPRITE_STARTX PEBBLE_WIDTH / 2 - SPRITE_WIDTH / 2
#define SPRITE_STARTY 140 - SPRITE_HEIGHT
#define SPRITE_XMIN 0
#define SPRITE_XMAX PEBBLE_WIDTH - SPRITE_WIDTH


// ---------------- Structures/Types

// ---------------- Private variables
static BitmapLayer *spriteLayer;
static GBitmap *spriteImg;
static Layer* window;

// ---------------- Private prototypes
static void createSprite();

/* ========================================================================== */

void initSprite(Layer* windowLayer) {
    window = windowLayer;
    createSprite();
    layer_add_child(window, bitmap_layer_get_layer(spriteLayer));
}

static void createSprite() {
    spriteImg = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_IDLE);
    spriteLayer = bitmap_layer_create(GRect(SPRITE_STARTX, SPRITE_STARTY, SPRITE_WIDTH, SPRITE_HEIGHT));
    bitmap_layer_set_bitmap(spriteLayer, spriteImg);
}

void deinitSprite() {
    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(spriteImg);
}



