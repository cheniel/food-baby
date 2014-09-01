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
#include "data.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define SPRITE_WIDTH 45
#define SPRITE_HEIGHT 45
#define SPRITE_STARTX PEBBLE_WIDTH / 2 - SPRITE_WIDTH / 2
#define SPRITE_STARTY 140 - SPRITE_HEIGHT
#define SPRITE_XMIN 0
#define SPRITE_XMAX PEBBLE_WIDTH - SPRITE_WIDTH

// ---------------- Structures/Types
typedef struct SpriteInfo {
    int x;
    int y;
    SpriteState state;
} SpriteInfo;

// ---------------- Private variables
static BitmapLayer *spriteLayer;
static GBitmap *spriteImg;
static Layer* window;
static SpriteInfo baby;

// ---------------- Private prototypes
static void createSprite();
static void startAnimation();

/* ========================================================================== */

void initSprite(Layer* windowLayer) {
    window = windowLayer;

    baby = (SpriteInfo) {
        .x = SPRITE_STARTX,
        .y = SPRITE_STARTY,
        .state = spriteAsleep,
    };

    createSprite();
    layer_add_child(window, bitmap_layer_get_layer(spriteLayer));

    startAnimation();
}

static void createSprite() {
    spriteImg = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_IDLE);
    spriteLayer = bitmap_layer_create(GRect(baby.x, baby.y, SPRITE_WIDTH, SPRITE_HEIGHT));
    bitmap_layer_set_bitmap(spriteLayer, spriteImg);
}

static void startAnimation() {
    baby.state = getSpriteState();

    switch (baby.state) {
        case spriteAsleep:
            break;
        case spriteSad:
            break;
        case spriteContent:
            break;
        case spriteHappy:
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "startAnimation: INVALID STATE");
            break;
    }

    
}

void deinitSprite() {
    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(spriteImg);
}



