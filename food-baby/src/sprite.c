/* ========================================================================== */
/* 
 * sprite.c
 *
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Functions that relate to sprite initialization and animation.
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>
#include <ctype.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "sprite.h"
#include "data.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

/* constants related to sprite locations and size */
#define SPRITE_WIDTH 45
#define SPRITE_HEIGHT 45
#define SPRITE_X_CENTER PEBBLE_WIDTH / 2 - SPRITE_WIDTH / 2
#define SPRITE_CEILING 50
#define SPRITE_FLOOR 145 - SPRITE_HEIGHT
#define SPRITE_XMIN 0
#define SPRITE_XMAX PEBBLE_WIDTH - SPRITE_WIDTH

/* constants related to the sleep animation */
#define SLEEP_COUNT 24 // number of animation refreshes before each Z appearance
#define SLEEP_COUNT_DIV 8 // number of refreshes before new Z appears
#define SLEEP_Z_INC 1000 // length of time (ms) between each new Z appearing

/* length of sad sprite animation */
#define ANIMATION_DURATION_MS 3000

/* happy jump constants */
#define NUM_JUMPS 1
#define JUMP_AIR_TIME 1000

/* height of jumps when sprite is in content state */
#define CONTENT_JUMP_CEILING 80

/* height of jumps when sprite is in happy state */
#define HAPPY_JUMP_CEILING SPRITE_CEILING

// ---------------- Structures/Types

/* stores info about the sprite to be passed around various functions */
typedef struct SpriteInfo {
    int x;
    int y;
    SpriteState state;
} SpriteInfo;

// ---------------- Private variables

/* information about window */
static Layer* window;
static GRect bounds;

/* sprite graphics and variables */
static BitmapLayer *spriteLayer;
static GBitmap *spriteImg;
static SpriteInfo baby;

/* used to determine in animation teardown if animation should repeat */
static bool continueAnimation; 

/* stores location to animate to */
static GRect moveTo;

/* images, animation, and bookkeeping for sleep animation */
static Animation *sleepAnimation;
static int sleepCounter;
static TextLayer *sleepZZZs[SLEEP_COUNT];
static GBitmap *sleepSprite;

/* animations and sprite images for sad state */
static PropertyAnimation *sadAnimation;
static GBitmap *sadRight;
static GBitmap *sadLeft;

/* animation and bookkeeping for happy state */
static PropertyAnimation *up;
static PropertyAnimation *down;
static int jumpsMade;
static bool happyJumpHappening;

/* animations and sprite images for content state */
static PropertyAnimation *contentUp;
static PropertyAnimation *contentDown;
static GBitmap *contentPreJump;
static GBitmap *contentNormal;
static GBitmap *contentJump;

/* animations and sprite images for happy state */
static PropertyAnimation *happyUp;
static PropertyAnimation *happyDown;
static GBitmap *happyPreJump;
static GBitmap *happyNormal;
static GBitmap *happyJump;

/* minutes since last movement or button press */
extern int minutesSinceLastActivity;

// ---------------- Private prototypes

/* sprite functions */
static void createSprite();
static void updateLocation();
static GRect getNextLocation();

/* sleep state functions */
static void sleepAnimInit();
static void sleepAnimSetup(struct Animation *animation);
static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized);
static void sleepAnimTeardown(struct Animation *animation);

/* sad animation functions */
static void startSadAnimation();
static void sadAnimationStarted(Animation *animation, void *data);
static void sadAnimationStopped(Animation *animation, bool finished, void *data);

/* happy jump functions */
static void upAnimationStopped(Animation *animation, bool finished, void *data);
static void downAnimationStopped(Animation *animation, bool finished, void *data);

/* content animation functions */
static void startContentAnimation();
static void contentUpStarted(Animation *animation, void *data);
static void contentUpStopped(Animation *animation, bool finished, void *data);
static void contentDownStopped(Animation *animation, bool finished, void *data);

/* happy animation functions */
static void startHappyAnimation();
static void happyUpStarted(Animation *animation, void *data);
static void happyUpStopped(Animation *animation, bool finished, void *data);
static void happyDownStopped(Animation *animation, bool finished, void *data);

// ----------------- Animation Structures

/* sleep animation handler */
static const AnimationImplementation sleepAnimImpl = {
    .setup = sleepAnimSetup,
    .update = sleepAnimUpdate,
    .teardown = sleepAnimTeardown,
};

/* sad animation handler */
static const AnimationHandlers sadAnimationHandlers = {
    .started = sadAnimationStarted,
    .stopped = sadAnimationStopped,
};

/* handler for happy jump animation (when going up) */
static const AnimationHandlers upAnimationHandlers = {
    .started = happyUpStarted,
    .stopped = upAnimationStopped,
};

/* handler for happy jump animation (when going down) */
static const AnimationHandlers downAnimationHandlers = {
    .stopped = downAnimationStopped,
};

/* handler for content jump animation (when going up) */
static const AnimationHandlers contentUpHandlers = {
    .started = contentUpStarted,
    .stopped = contentUpStopped,
};

/* handler for content jump animation (when going down) */
static const AnimationHandlers contentDownHandlers = {
    .stopped = contentDownStopped,
};

/* handler for content jump animation (when going up) */
static const AnimationHandlers happyUpHandlers = {
    .started = happyUpStarted,
    .stopped = happyUpStopped,
};

/* handler for content jump animation (when going down) */
static const AnimationHandlers happyDownHandlers = {
    .stopped = happyDownStopped,
};

/* ========================================================================== */

/* 
 * initializes sprite, initializes bookkeeping and layers, starts animations 
 * 
 * param: layer to initialize sprite
 */
void initSprite(Layer* windowLayer) {
    window = windowLayer;
    bounds = layer_get_bounds(windowLayer);

    baby = (SpriteInfo) {
        .x = SPRITE_X_CENTER,
        .y = SPRITE_FLOOR,
        .state = spriteNone,
    };

    createSprite();
    sleepAnimInit(); // doesn't actually start sleep animation

    happyJumpHappening = false;

    startAnimation();
}

/* 
 * initializes images for animations, puts sprite on window 
 */
static void createSprite() {

    /* create images associated with sprite */
    spriteImg = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_IDLE);
    sleepSprite = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_ASLEEP);
    sadLeft = gbitmap_create_with_resource(RESOURCE_ID_SAD_LEFT);
    sadRight = gbitmap_create_with_resource(RESOURCE_ID_SAD_RIGHT);
    contentPreJump = gbitmap_create_with_resource(RESOURCE_ID_CONTENT_PRE);
    contentNormal = gbitmap_create_with_resource(RESOURCE_ID_CONTENT_NORM);
    contentJump = gbitmap_create_with_resource(RESOURCE_ID_CONTENT_JUMP);
    happyPreJump = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_PRE);
    happyNormal = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_NORM);
    happyJump = gbitmap_create_with_resource(RESOURCE_ID_HAPPY_JUMP);

    /* add sprite to window */
    spriteLayer = bitmap_layer_create(GRect(baby.x, baby.y, SPRITE_WIDTH, 
        SPRITE_HEIGHT));
    bitmap_layer_set_bitmap(spriteLayer, spriteImg);
    layer_add_child(window, bitmap_layer_get_layer(spriteLayer));
}

/*
 * Determine the state of the baby, then animate the state for the sprite
 */
void startAnimation() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "determining baby state: ");

    /* get the new state of the baby */
    baby.state = getSpriteState();

    /* 
     * used to keep track of if the startAnimation should be called again at the
     * animation teardown
     */
    continueAnimation = true;

    /* run animation associated with state */
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
            startContentAnimation();
            break;
        case spriteHappy:
            APP_LOG(APP_LOG_LEVEL_DEBUG, "\tspriteHappy");
            startHappyAnimation();
            break;
        default:
            APP_LOG(APP_LOG_LEVEL_ERROR, "startAnimation: INVALID STATE");
            break;
    }
}

/*
 * initialize the components required to run the sleep animation
 * does not actually run the animation.
 */
static void sleepAnimInit() {
    sleepAnimation = animation_create();/* length of sad sprite duration */
    animation_set_duration(sleepAnimation, ANIMATION_DURATION_MS);
    animation_set_implementation(sleepAnimation, &sleepAnimImpl);

    /* set up ZZZs text layers. */
    sleepZZZs[0] = text_layer_create((GRect) {
        .origin = { 85, 100 },
        .size = { 15, 15 }
    });

    sleepZZZs[1] = text_layer_create((GRect) {
        .origin = { 90, 85 },
        .size = { 20, 20 }
    });

    sleepZZZs[2] = text_layer_create((GRect) {
        .origin = { 98, 65 },
        .size = { 15, 25 }
    });

    /* set properties of text layers */
    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) {
        text_layer_set_text(sleepZZZs[z], "z");
        setTextLayerDefaults(sleepZZZs[z]);
    }

    text_layer_set_font(sleepZZZs[0],fonts_get_system_font(FONT_KEY_GOTHIC_14));
    text_layer_set_font(sleepZZZs[1],fonts_get_system_font(FONT_KEY_GOTHIC_18));
    text_layer_set_font(sleepZZZs[2],fonts_get_system_font(FONT_KEY_GOTHIC_24));
}

/*
 * setup for sleep animation.
 * move the sprite to the center, reset sleep counter
 */
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

/*
 * update for sleep animation
 * add next Z text layer, if all are on the window, remove all of the text 
 * layers.
 */
static void sleepAnimUpdate(struct Animation *animation, 
    const uint32_t time_normalized) {

    /* if all zs are displayed */
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

/*
 * teardown for sleep animation
 * remove all z text layers from window
 * if animations should continue, call startAnimation
 */
static void sleepAnimTeardown(struct Animation *animation) {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "tearing down animation");

    // remove all displayed Zs
    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) {
        layer_remove_from_parent(text_layer_get_layer(sleepZZZs[z]));
    }   

    if (continueAnimation) { startAnimation(); } 
}

/*
 * sets up and schedules sad animation
 * based on randomly generated location.
 */
static void startSadAnimation() {
    if (sadAnimation) { property_animation_destroy(sadAnimation); }

    moveTo = getNextLocation(); // get randomly generated location
    sadAnimation = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &moveTo);
    animation_set_handlers((Animation*) sadAnimation, sadAnimationHandlers, NULL);
    animation_set_duration((Animation*) sadAnimation, 2000);
    animation_schedule((Animation*) sadAnimation);
}

/*
 * returns a random GRect which is used as the frame to animate to.
 * keeps y axis the same, random x location on the screen. 
 */
static GRect getNextLocation() {
    int x = randomInRange(0, bounds.size.w - SPRITE_WIDTH);
    return GRect(x, baby.y, SPRITE_WIDTH, SPRITE_HEIGHT);
}

/* 
 * change icon based on direction of sprite for sad animation
 */
static void sadAnimationStarted(Animation *animation, void *data) {
    if (moveTo.origin.x > baby.x) {
        bitmap_layer_set_bitmap(spriteLayer, sadRight); // move right
    } else if (moveTo.origin.x < baby.x) {
        bitmap_layer_set_bitmap(spriteLayer, sadLeft); // move left
    } 
}

/*
 * stop for sad animation
 * updates the stored location of the baby
 * starts the animation if appropriate
 */
static void sadAnimationStopped(Animation *animation, bool finished, void *data) {
    updateLocation(); // update location of baby

    if (continueAnimation) { startAnimation(); } 
}

/*
 * sets up content animation and then schedules it.
 * gets a random location to move to
 */
static void startContentAnimation() {
    if (contentUp) { property_animation_destroy(contentUp); }
    if (contentDown) { property_animation_destroy(contentDown); }

    /* get next random location to go to */
    moveTo = getNextLocation();

    /* 
     * set up the jump animation to move horizontally halfway to moveTo, and
     * vertically to the content jump height
     */
    GRect peak = GRect((moveTo.origin.x - baby.x) / 2 + baby.x, 
      CONTENT_JUMP_CEILING, SPRITE_WIDTH, SPRITE_HEIGHT);
    contentUp = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &peak);
    animation_set_duration((Animation*) contentUp, JUMP_AIR_TIME / 2);
    animation_set_curve((Animation*) contentUp, AnimationCurveEaseIn);
    animation_set_handlers((Animation*) contentUp, contentUpHandlers, NULL);

    /* go to moveTo from peak */
    GRect base = GRect(moveTo.origin.x, SPRITE_FLOOR, SPRITE_WIDTH, 
        SPRITE_HEIGHT);
    contentDown = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), &peak, &base);
    animation_set_duration((Animation*) contentDown, JUMP_AIR_TIME / 2);
    animation_set_curve((Animation*) contentDown, AnimationCurveEaseOut);
    animation_set_handlers((Animation*) contentDown, contentDownHandlers, NULL);

    animation_schedule((Animation*) contentUp);
}

/*
 * start handler for content animation
 * flash pre-jump, idle, then jump sprites
 */
static void contentUpStarted(Animation *animation, void *data) {
    // set up pre jump
    bitmap_layer_set_bitmap(spriteLayer, contentPreJump); 
    psleep(100);
    bitmap_layer_set_bitmap(spriteLayer, contentNormal); 
    psleep(100);    
    bitmap_layer_set_bitmap(spriteLayer, contentJump); 
}

/* 
 * stop handler for content animation. simply schedules down animation. 
 */
static void contentUpStopped(Animation *animation, bool finished, void *data) {
    animation_schedule((Animation*) contentDown);
}

/*
 * stop handler for content animation. updates location and starts next 
 * animation if necessary. Reset sprite to normal image.
 */
static void contentDownStopped(Animation *animation, bool finished, void *data) {
    bitmap_layer_set_bitmap(spriteLayer, contentNormal); 
    updateLocation();
    if (continueAnimation) { startAnimation(); } 
}

/*
 * start handler for happy animation.
 * sets up animation and then schedules the animation.
 */
static void startHappyAnimation() {
    if (happyUp) { property_animation_destroy(happyUp); }
    if (happyDown) { property_animation_destroy(happyDown); }

    /* get next random location to go to */
    moveTo = getNextLocation();

    /* 
     * set up the jump animation to move horizontally halfway to moveTo, and
     * vertically to the content jump height
     */
    GRect peak = GRect((moveTo.origin.x - baby.x) / 2 + baby.x, 
      HAPPY_JUMP_CEILING, SPRITE_WIDTH, SPRITE_HEIGHT);
    happyUp = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), NULL, &peak);
    animation_set_duration((Animation*) happyUp, JUMP_AIR_TIME / 2);
    animation_set_curve((Animation*) happyUp, AnimationCurveEaseIn);
    animation_set_handlers((Animation*) happyUp, happyUpHandlers, NULL);

    /* go to moveTo from peak */
    GRect base = GRect(moveTo.origin.x, SPRITE_FLOOR, SPRITE_WIDTH, 
        SPRITE_HEIGHT);
    happyDown = property_animation_create_layer_frame(
        bitmap_layer_get_layer(spriteLayer), &peak, &base);
    animation_set_duration((Animation*) happyDown, JUMP_AIR_TIME / 2);
    animation_set_curve((Animation*) happyDown, AnimationCurveEaseOut);
    animation_set_handlers((Animation*) happyDown, happyDownHandlers, NULL);

    animation_schedule((Animation*) happyUp);
}

/*
 * start handler for happy animation
 * flash pre-jump, idle, then jump sprites
 */
static void happyUpStarted(Animation *animation, void *data) {
    // set up pre jump
    bitmap_layer_set_bitmap(spriteLayer, happyPreJump); 
    psleep(100);
    bitmap_layer_set_bitmap(spriteLayer, happyNormal); 
    psleep(100);    
    bitmap_layer_set_bitmap(spriteLayer, happyJump); 
}

/* 
 * stop handler for happy animation. simply schedules down animation. 
 */
static void happyUpStopped(Animation *animation, bool finished, void *data) {
    animation_schedule((Animation*) happyDown);
}

/*
 * stop handler for happy animation. updates location and starts next 
 * animation if necessary. Reset sprite to normal image.
 */
static void happyDownStopped(Animation *animation, bool finished, void *data) {
    bitmap_layer_set_bitmap(spriteLayer, happyNormal); 
    
    updateLocation();

    if (continueAnimation) { startAnimation(); } 
}

/*
 * stops animations and sets continue animation to false so animation teardowns
 * don't call startAnimation
 */
void stopAnimation() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "stopping animation");
    continueAnimation = false;
    animation_unschedule_all();
}

/*
 * has the sprite jump NUM_JUMPS. Stops all current animations and then restarts
 * it at the end.
 * 
 * called when a food is selected
 */
void happyJumps() {
    if (!((up && animation_is_scheduled((Animation*) up)) 
        || (down && animation_is_scheduled((Animation*) down)))) {

        stopAnimation(); // stop previous animations    

        if (up) { property_animation_destroy(up); }
        if (down) { property_animation_destroy(down); }

        /* 
         * create peak by animating to a frame that is the same except y is the
         * ceiling. used for up animation.
         */
        GRect peak = GRect(baby.x, SPRITE_CEILING, SPRITE_WIDTH, SPRITE_HEIGHT);
        up = property_animation_create_layer_frame(
            bitmap_layer_get_layer(spriteLayer), NULL, &peak);
        animation_set_duration((Animation*) up, JUMP_AIR_TIME / 2);
        animation_set_curve((Animation*) up, AnimationCurveEaseIn);
        animation_set_handlers((Animation*) up, upAnimationHandlers, NULL);

        /*
         * base is the baby's current location. used for down animation
         */
        GRect base = GRect(baby.x, SPRITE_FLOOR, SPRITE_WIDTH, SPRITE_HEIGHT);
        down = property_animation_create_layer_frame(
            bitmap_layer_get_layer(spriteLayer), &peak, &base);
        animation_set_duration((Animation*) down, JUMP_AIR_TIME / 2);
        animation_set_curve((Animation*) down, AnimationCurveEaseOut);
        animation_set_handlers((Animation*) down, downAnimationHandlers, NULL);

        jumpsMade = 0; // keeps track of number of jumps

        // used in another function to determine if this animation is happening
        happyJumpHappening = true; 

        animation_schedule((Animation*) up);
    }
}

/* 
 * stop animation for up animation for happy jumps. schedules down animation 
 */
static void upAnimationStopped(Animation *animation, bool finished, void *data) {
    animation_schedule((Animation*) down);
}

/*
 * stop handler for down animation. repeats animation if not enough jumps have 
 * occured, stops and starts animations based on sprite state if the correct 
 * number.
 */
static void downAnimationStopped(Animation *animation, bool finished, void *data) {
    bitmap_layer_set_bitmap(spriteLayer, happyNormal); 

    jumpsMade++;

    if (jumpsMade < NUM_JUMPS) {
        psleep(100);
        bitmap_layer_set_bitmap(spriteLayer, happyPreJump); 
        animation_schedule((Animation*) up);
    } else {
        happyJumpHappening = false;
        startAnimation(); // restart previous animations
    }
}

/* 
 * cleans up everything releated to sprite and sprite animation 
 */
void deinitSprite() {
    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(sadLeft);
    gbitmap_destroy(sadRight);
    gbitmap_destroy(spriteImg);
    gbitmap_destroy(sleepSprite);
    gbitmap_destroy(contentPreJump);
    gbitmap_destroy(contentNormal);
    gbitmap_destroy(contentJump);
    gbitmap_destroy(happyPreJump);
    gbitmap_destroy(happyNormal);
    gbitmap_destroy(happyJump);

    for (int z = 0; z < SLEEP_COUNT / SLEEP_COUNT_DIV; z++) { 
        text_layer_destroy(sleepZZZs[z]); 
    }

    stopAnimation();

    APP_LOG(APP_LOG_LEVEL_DEBUG, "freeing animations");
    if (sleepAnimation) { animation_destroy(sleepAnimation); }
    if (sadAnimation) { property_animation_destroy(sadAnimation); }
    if (up) { property_animation_destroy(up); }
    if (down) { property_animation_destroy(down); }
    if (contentUp) { property_animation_destroy(contentUp); }
    if (contentDown) { property_animation_destroy(contentDown); }
    if (happyUp) { property_animation_destroy(happyUp); }
    if (happyDown) { property_animation_destroy(happyDown); }
}

/*
 * updates stateinfo location based on layer's current location
 */
static void updateLocation() {
    GRect location = layer_get_frame(bitmap_layer_get_layer(spriteLayer));
    baby.x = location.origin.x;
    baby.y = location.origin.y;    
}

/*
 * returns true if the happy jump animation is not occuring (tends to cause 
 * crash if something is done while happy jump is happening)
 */
bool animationIsReady() {
    return !happyJumpHappening;
}

/*
 * function used to wake up sprite from sleep
 */
void wakeUp() {
    minutesSinceLastActivity = 0;

    if (baby.state == spriteAsleep) {
        stopAnimation();
        startAnimation();
    }
}

