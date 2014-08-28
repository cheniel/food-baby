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
}

static void init(void) {
    home = homeInit();
    foodSelect = foodInit();
    logview = logInit();

    initData();

    accel_tap_service_subscribe(tapHandler);

    srand(time(NULL)); // for random ops
    window_stack_push(home, ANIMATION_SETTING);
}

static void deinit(void) {
    window_destroy(home);
    window_destroy(foodSelect);
    window_destroy(logview);
    accel_tap_service_unsubscribe();
}


