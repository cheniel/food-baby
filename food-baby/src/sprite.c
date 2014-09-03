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
#define SPRITE_X_CENTER PEBBLE_WIDTH / 2 - SPRITE_WIDTH / 2
#define SPRITE_CEILING 50
#define SPRITE_FLOOR 145 - SPRITE_HEIGHT
#define SPRITE_XMIN 0
#define SPRITE_XMAX PEBBLE_WIDTH - SPRITE_WIDTH

#define SLEEP_COUNT 24
#define SLEEP_COUNT_DIV 8
#define SLEEP_Z_INC 1000

#define ANIMATION_DURATION_MS 3000

#define NUM_JUMPS 3
#define JUMP_AIR_TIME 1000

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
static GRect bounds;
static SpriteInfo baby;

static Animation *sleepAnimation;
static int sleepCounter;
static TextLayer *sleepZZZs[SLEEP_COUNT];
static bool continueAnimation;
static GBitmap *sleepSprite;

static PropertyAnimation *sadAnimation;
static GRect moveTo;
static GBitmap *sadRight;
static GBitmap *sadLeft;

static PropertyAnimation *up;
static PropertyAnimation *down;

static GBitmap *happyPreJump;
static GBitmap *happyNormal;
static GBitmap *happyJump;

// ---------------- Private prototypes
static void createSprite();
static void sleepAnimInit();
static void sleepAnimSetup(struct Animation *animation);
static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized);
static void sleepAnimTeardown(struct Animation *animation);
static void sadAnimationStarted(Animation *animation, void *data);
static void sadAnimationStopped(Animation *animation, bool finished, void *data);
static void upAnimationStarted(Animation *animation, void *data);
static void upAnimationStopped(Animation *animation, bool finished, void *data);
static void downAnimationStarted(Animation *animation, void *data);
static void downAnimationStopped(Animation *animation, bool finished, void *data);
static GRect getNextSadLocation();
static void startSadAnimation();

// ----------------- Animation Structures
static const AnimationImplementation sleepAnimImpl = {
    .setup = sleepAnimSetup,
    .update = sleepAnimUpdate,
    .teardown = sleepAnimTeardown,
};

static const AnimationHandlers sadAnimationHandlers = {
    .started = sadAnimationStarted,
    .stopped = sadAnimationStopped,
};

static const AnimationHandlers upAnimationHandlers = {
    .started = upAnimationStarted,
    .stopped = upAnimationStopped,
};

static const AnimationHandlers downAnimationHandlers = {
    .started = downAnimationStarted,
    .stopped = downAnimationStopped,
};

/* ========================================================================== */

void initSprite(Layer* windowLayer) {
    window = windowLayer;
    bounds = layer_get_bounds(windowLayer);

    baby = (SpriteInfo) {
        .x = SPRITE_X_CENTER,
        .y = SPRITE_FLOOR,
        .state = spriteNone,
    };

    createSprite();
    sleepAnimInit();

    startAnimation();
}

static void createSprite() {

    // create images associated with sprite
    spriteImg = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_IDLE);
    sleepSprite = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_ASLEEP);
    sadLeft = gbitmap_create_with_resource(RESOURCE_ID_SAD_LEFT);
    sadRight = gbitmap_create_with_resource(RESOURCE_ID_SAD_RIGHT);
    happyPreJump = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_PRE);
    happyNormal = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_NORM);
    happyJump = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_JUMP);

    spriteLayer = bitmap_layer_create(GRect(baby.x, baby.y, SPRITE_WIDTH, 
        SPRITE_HEIGHT));
    bitmap_layer_set_bitmap(spriteLayer, spriteImg);
    layer_add_child(window, bitmap_layer_get_layer(spriteLayer));
}

void startAnimation() {

    baby.state = getSpriteState();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "determining baby state: ");

    continueAnimation = true;

    switch (baby.state) {
        case spriteAsleep:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteAsleep");
            animation_schedule(sleepAnimation);
            break;
        case spriteSad:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteSad");
            startSadAnimation();
            break;
        case spriteContent:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteContent");
            break;
        case spriteHappy:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteHappy");
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "startAnimation: INVALID STATE");
            break;
    }
}

static void sleepAnimInit() {
    sleepAnimation = animation_create();
    animation_set_duration(sleepAnimation, ANIMATION_DURATION_MS);
    animation_set_implementation(sleepAnimation, &sleepAnimImpl);

    // set up ZZZs
    sleepZZZs[0] = text_layer_create((GRect) {
        .origin = { 85, 90 },
        .size = { 15, 15 }
    });

    sleepZZZs[1] = text_layer_create((GRect) {
        .origin = { 90, 75 },
        .size = { 20, 20 }
    });

    sleepZZZs[2] = text_layer_create((GRect) {
        .origin = { 98, 55 },
        .size = { 15, 25 }
    });

    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) {
        text_layer_set_text(sleepZZZs[z], "z");
        setTextLayerDefaults(sleepZZZs[z]);
    }

    text_layer_set_font(sleepZZZs[0],fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_font(sleepZZZs[1],fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_font(sleepZZZs[2],fonts_get_system_font(FONT_KEY_GOTHIC_24));
}

static void sleepAnimSetup(struct Animation *animation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "setting up sleep animation");

    // move baby to center
    baby.x = SPRITE_X_CENTER;
    baby.y = SPRITE_FLOOR;

    layer_set_frame(bitmap_layer_get_layer(spriteLayer), 
                    GRect(baby.x, baby.y, SPRITE_WIDTH, SPRITE_HEIGHT));

    // change icon to sleeping
    bitmap_layer_set_bitmap(spriteLayer, sleepSprite);

    sleepCounter = 0;
}

static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "updating sleep animation");

    if (sleepCounter >= SLEEP_COUNT) {
        sleepCounter = 0;

        // remove all displayed Zs
        for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) {
            layer_remove_from_parent(text_layer_get_layer(sleepZZZs[z]));
        }   

    } else {

        // make Z at sleepCounter visible 
        layer_add_child(window, 
            text_layer_get_layer(sleepZZZs[sleepCounter / SLEEP_COUNT_DIV]));

        sleepCounter++;
    }

    psleep(SLEEP_Z_INC / SLEEP_COUNT_DIV);
}

static void sleepAnimTeardown(struct Animation *animation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "tearing down animation");

    // remove all displayed Zs
    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) {
        layer_remove_from_parent(text_layer_get_layer(sleepZZZs[z]));
    }   

    if (continueAnimation) { startAnimation(); } 
}

static GRect getNextSadLocation() {
    int x = randomInRange(0, bounds.size.w - SPRITE_WIDTH);
    return GRect(x, baby.y, SPRITE_WIDTH, SPRITE_HEIGHT);
}

// change icon based on direction of sprite
static void sadAnimationStarted(Animation *animation, void *data) {
    if (moveTo.origin.x > baby.x) {
        bitmap_layer_set_bitmap(spriteLayer, sadRight); // move right
    } else if (moveTo.origin.x < baby.x) {
        bitmap_layer_set_bitmap(spriteLayer, sadLeft); // move left
    } 
}

static void sadAnimationStopped(Animation *animation, bool finished, void *data) {
    // update location of baby
    baby.x = moveTo.origin.x;
    baby.y = moveTo.origin.y;

    property_animation_destroy(sadAnimation);

    if (continueAnimation) { startAnimation(); } 
}

static void startSadAnimation() {
    moveTo = getNextSadLocation();
    sadAnimation = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &moveTo);
    animation_set_duration((Animation*) sadAnimation, 2000);
    animation_set_handlers((Animation*) sadAnimation, sadAnimationHandlers, NULL);
    animation_schedule((Animation*) sadAnimation);
}

void stopAnimation() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "stopping animation");
    continueAnimation = false;
    animation_unschedule_all();
}

void happyJumps() {
    if (!up) { property_animation_destroy(up); }
    if (!down) { property_animation_destroy(down); }

    stopAnimation(); // stop previous animations    

    GRect base = GRect(baby.x, SPRITE_FLOOR, SPRITE_WIDTH, SPRITE_HEIGHT);
    GRect peak = GRect(baby.x, SPRITE_CEILING, SPRITE_WIDTH, SPRITE_HEIGHT);

    up = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &peak);

    down = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &base);

    animation_set_duration((Animation*) up, JUMP_AIR_TIME / 2);
    animation_set_duration((Animation*) down, JUMP_AIR_TIME / 2);

    animation_set_curve((Animation*) up, AnimationCurveEaseIn);
    animation_set_curve((Animation*) down, AnimationCurveEaseOut);

    animation_set_handlers((Animation*) up, upAnimationHandlers, NULL);
    animation_set_handlers((Animation*) down, downAnimationHandlers, NULL);

    for (int i = 0; i < NUM_JUMPS; i++) {
        animation_schedule((Animation*) up);
    }

    // this doesn't work here. right idea though.
    // startAnimation(); // restart previous animations
}

static void upAnimationStarted(Animation *animation, void *data) {
    // set up pre jump

}

static void upAnimationStopped(Animation *animation, bool finished, void *data) {

}

static void downAnimationStarted(Animation *animation, void *data) {

}

static void downAnimationStopped(Animation *animation, bool finished, void *data) {

}

void deinitSprite() {
    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(sadLeft);
    gbitmap_destroy(sadRight);
    gbitmap_destroy(spriteImg);
    gbitmap_destroy(sleepSprite);
    gbitmap_destroy(happyPreJump);
    gbitmap_destroy(happyNormal);
    gbitmap_destroy(happyJump);

    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) { 
        text_layer_destroy(sleepZZZs[z]); 
    }

    stopAnimation();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "freeing animations");

    if (!sleepAnimation) { animation_destroy(sleepAnimation); }
    if (!sadAnimation) { property_animation_destroy(sadAnimation); }
    if (!up) { property_animation_destroy(up); }
    if (!down) { property_animation_destroy(down); }
}



