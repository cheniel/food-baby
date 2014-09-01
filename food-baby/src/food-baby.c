/* ========================================================================== */
/* 
 * food-baby.c
 * 
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Initializes windows and opens main app loop.
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "home.h"
#include "foodSelect.h"
#include "logview.h"
#include "data.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
extern Window *home;
extern Window *foodSelect;
extern Window *logview;
extern int activityToday;
extern int activityRecord;
extern int minutesSinceLastShake;

// ---------------- Private prototypes
int main(void);
static void init(void);
static void deinit(void);
static void tapHandler(AccelAxisType axis, int32_t direction);

/* ========================================================================== */

int main(void) {
    init();
    app_event_loop();
    deinit();
}

static void tapHandler(AccelAxisType axis, int32_t direction) {
    activityToday++;
    minutesSinceLastShake = 0;

    // if the current state is sleeping
        // wake up the sprite

    if (activityToday > activityRecord) { activityRecord = activityToday; }
}

static void init(void) {
    initData();
    home = homeInit();
    foodSelect = foodInit();
    logview = logInit();

    accel_tap_service_subscribe(tapHandler);

    srand(time(NULL)); // for random ops. 
    // causes memory leak... see:
    // http://forums.getpebble.com/discussion/10498/srand-rand-memory-leak

    window_stack_push(home, ANIMATION_SETTING);
}

static void deinit(void) {
    saveData();
    freeResources();
    window_destroy(home);
    window_destroy(foodSelect);
    window_destroy(logview);
    accel_tap_service_unsubscribe();
}


