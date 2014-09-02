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

#define SLEEP_COUNT 3

#define ANIMATION_DURATION_MS 7000

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
static Animation *sleepAnim;

static int sleepCounter;
static TextLayer *sleepZZZs[SLEEP_COUNT];

// ---------------- Private prototypes
static void createSprite();
static void startAnimation();
static void sleepAnimSetup(struct Animation *animation);
static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized);
static void sleepAnimTeardown(struct Animation *animation);

// ----------------- Animation Structures
static const AnimationImplementation sleepAnimImpl = {
    .setup = sleepAnimSetup,
    .update = sleepAnimUpdate,
    .teardown = sleepAnimTeardown,
};

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

    APP_LOG(APP_LOG_LEVEL_DEBUG, "determining baby state: ");

    sleepAnim = animation_create();

    switch (baby.state) {
        case spriteAsleep:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteAsleep");
            animation_set_duration(sleepAnim, ANIMATION_DURATION_MS);
            animation_set_implementation(sleepAnim, &sleepAnimImpl);
            break;
        case spriteSad:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteSad");
            animation_set_duration(sleepAnim, ANIMATION_DURATION_MS);
            animation_set_implementation(sleepAnim, &sleepAnimImpl);
            break;
        case spriteContent:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteContent");
            animation_set_duration(sleepAnim, ANIMATION_DURATION_MS);
            animation_set_implementation(sleepAnim, &sleepAnimImpl);
            break;
        case spriteHappy:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteHappy");
            animation_set_duration(sleepAnim, ANIMATION_DURATION_MS);
            animation_set_implementation(sleepAnim, &sleepAnimImpl);
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "startAnimation: INVALID STATE");
            break;
    }

    animation_schedule(sleepAnim);
}

static void sleepAnimSetup(struct Animation *animation) {

    // set up ZZZs
    sleepZZZs[0] = text_layer_create((GRect) {
        .origin = { 90, 80 },
        .size = { 15, 15 }
    });

    sleepZZZs[1] = text_layer_create((GRect) {
        .origin = { 100, 70 },
        .size = { 15, 15 }
    });

    sleepZZZs[2] = text_layer_create((GRect) {
        .origin = { 110, 60 },
        .size = { 15, 15 }
    });

    for (int z = 0; z < SLEEP_COUNT; z++) {
        text_layer_set_text(sleepZZZs[z], "z");
        setTextLayerDefaults(sleepZZZs[z]);

    }

    // move baby to center
    baby.x = SPRITE_STARTX;
    baby.y = SPRITE_STARTY;

    // change icon to sleeping

    sleepCounter = 0;
}

static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized) {
    
    if (sleepCounter >= SLEEP_COUNT) {
        sleepCounter = 0;

        // remove all displayed Zs
        for (int z = 0; z < SLEEP_COUNT; z++) {
            layer_remove_from_parent(text_layer_get_layer(sleepZZZs[z]));
        }   

    } else {

        // make Z at sleepCounter visible 
        layer_add_child(window, text_layer_get_layer(sleepZZZs[sleepCounter]));

        sleepCounter++;
    }

    psleep(1000);
}

static void sleepAnimTeardown(struct Animation *animation) {
    for (int z = 0; z < SLEEP_COUNT; z++) { text_layer_destroy(sleepZZZs[z]); }

    startAnimation();
}

void deinitSprite() {
    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(spriteImg);
}



