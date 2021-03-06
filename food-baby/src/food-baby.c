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
#include "sprite.h"
#include "strap/strap.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
extern Window *home;
extern Window *foodSelect;
extern Window *logview;
extern int activityToday;
extern int activityRecord;
extern int minutesSinceLastActivity;

// ---------------- Private prototypes
int main(void);
static void init(void);
static void deinit(void);
static void tapHandler(AccelAxisType axis, int32_t direction);

/* ========================================================================== */

/*
 * main for food-baby project.
 * initializes app, loops app, deinitializes app
 */
int main(void) {
    init();
    app_event_loop();
    deinit();
}

/*
 * handler called when pebble is shaken. increments activity for the day, sets
 * new record if appropriate. Wakes up baby.
 */
static void tapHandler(AccelAxisType axis, int32_t direction) {
    activityToday++;
    wakeUp();

    if (activityToday > activityRecord) { activityRecord = activityToday; }
}

/*
 * initialize windows, tap service and home window.
 */
static void init(void) {
    initData();
    home = homeInit();
    foodSelect = foodInit();
    logview = logInit();

    accel_tap_service_subscribe(tapHandler);

    srand(time(NULL)); // for random ops. causes negligible memory leak... see:
    // http://forums.getpebble.com/discussion/10498/srand-rand-memory-leak

    // for strap
    int in_size = app_message_inbox_size_maximum();
    int out_size = app_message_outbox_size_maximum();
    app_message_open(in_size, out_size);
    strap_init();

    window_stack_push(home, ANIMATION_SETTING);
}

/*
 * free up resources
 */
static void deinit(void) {
    strap_deinit();
    saveData();
    freeResources();
    window_destroy(home);
    window_destroy(foodSelect);
    window_destroy(logview);
    accel_tap_service_unsubscribe();
}


