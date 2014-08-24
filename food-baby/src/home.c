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

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"

// ---------------- Constant definitions
// static const int = 5

// ---------------- Macro definitions
#define MAX_TIME_CHAR 10
#define MAX_DATE_CHAR 30
#define TIME_FORMAT "%I:%M"
#define DATE_FORMAT "%a, %b %e"

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer *timeText;
static TextLayer *dateText;
static TextLayer *recText;
static char *timeString;
static char *dateString;

static BitmapLayer *sidebarLayer;
static GBitmap *sidebarImg;

static BitmapLayer *spriteLayer;
static GBitmap *spriteImg;

static BitmapLayer *foodIconLayer;
static GBitmap *foodIcon;

static BitmapLayer *waterIconLayer;
static GBitmap *waterIcon;

static BitmapLayer *logIconLayer;
static GBitmap *logIcon;

// ---------------- Private prototypes
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void click_config_provider(void *context);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);

static void load(Window *window);
static void unload(Window *window);
static void updateTime();
static void updateDate(struct tm *tick_time);

/* ========================================================================== */

Window *homeInit() {
    Window *home = window_create();
    window_set_click_config_provider(home, click_config_provider);
    window_set_window_handlers(home, (WindowHandlers) {
     .load = load,
      .unload = unload,
    }); 

    window_set_fullscreen(home, true);
    window_set_background_color(home, BACKGROUND_COLOR);

    return home;
}

static void load(Window *window) {
    Layer *window_layer = window_get_root_layer(window);
    GRect bounds = layer_get_bounds(window_layer);

    /* initialize services */
    tick_timer_service_subscribe(MINUTE_UNIT, tick_handler); 

    /* create time text */
    timeString = calloc(MAX_TIME_CHAR, sizeof(char));
    timeText = text_layer_create((GRect) { 
        .origin = { 0, 0 }, 
        .size = { bounds.size.w, 34 } 
    });
    setTextLayerDefaults(timeText);
    text_layer_set_font(timeText, 
        fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
    
    /* create date text */
    dateString = calloc(MAX_DATE_CHAR, sizeof(char));
    dateText = text_layer_create((GRect) { 
        .origin = { 0, 35 }, 
        .size = { bounds.size.w, 34 } 
    });
    setTextLayerDefaults(dateText);
    text_layer_set_font(dateText, 
        fonts_get_system_font(FONT_KEY_GOTHIC_14));

    /* update times */
    time_t currentTime = time(NULL);
    struct tm* tm = localtime(&currentTime);
    updateTime(tm);
    updateDate(tm);

    /* create recommendation text */
    recText = text_layer_create((GRect) { 
        .origin = { 0, 145 }, 
        .size = { bounds.size.w, 20 } 
    });
    setTextLayerDefaults(recText);
    text_layer_set_font(recText, 
        fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    text_layer_set_text(recText, "Drink more water!");

    /* create sidebar */
    sidebarImg = gbitmap_create_with_resource(RESOURCE_ID_SIDEBAR);
    sidebarLayer = bitmap_layer_create(GRect(117, 46, 30, 85));
    bitmap_layer_set_bitmap(sidebarLayer, sidebarImg);

    /* create food icon */
    foodIcon = gbitmap_create_with_resource(RESOURCE_ID_FOOD);
    foodIconLayer = bitmap_layer_create(GRect(119, 49, 25, 25));
    bitmap_layer_set_bitmap(foodIconLayer, foodIcon);

    /* create water icon */
    waterIcon = gbitmap_create_with_resource(RESOURCE_ID_WATER);
    waterIconLayer = bitmap_layer_create(GRect(119, 76, 25, 25));
    bitmap_layer_set_bitmap(waterIconLayer, waterIcon);

    /* create log icon */
    logIcon = gbitmap_create_with_resource(RESOURCE_ID_LOG);
    logIconLayer = bitmap_layer_create(GRect(119, 103, 25, 25));
    bitmap_layer_set_bitmap(logIconLayer, logIcon);

    /* create sprite */
    spriteImg = gbitmap_create_with_resource(RESOURCE_ID_SPRITE_IDLE);
    spriteLayer = bitmap_layer_create(GRect(10, 90, 40, 40));
    bitmap_layer_set_bitmap(spriteLayer, spriteImg);

    /* add text layers to window */
    layer_add_child(window_layer, text_layer_get_layer(timeText));
    layer_add_child(window_layer, text_layer_get_layer(dateText));
    layer_add_child(window_layer, text_layer_get_layer(recText));
    layer_add_child(window_layer, bitmap_layer_get_layer(sidebarLayer));
    layer_add_child(window_layer, bitmap_layer_get_layer(spriteLayer));
    layer_add_child(window_layer, bitmap_layer_get_layer(foodIconLayer));
    layer_add_child(window_layer, bitmap_layer_get_layer(waterIconLayer));
    layer_add_child(window_layer, bitmap_layer_get_layer(logIconLayer));
}

static void unload(Window *window) {
    text_layer_destroy(timeText);
    text_layer_destroy(dateText);
    free(timeString);
    free(dateString);

    bitmap_layer_destroy(sidebarLayer);
    gbitmap_destroy(sidebarImg);

    bitmap_layer_destroy(spriteLayer);
    gbitmap_destroy(spriteImg);

    bitmap_layer_destroy(foodIconLayer);
    gbitmap_destroy(foodIcon);

    tick_timer_service_unsubscribe();
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    if(units_changed & MINUTE_UNIT) {
        updateTime(tick_time);
    }
    if(units_changed & DAY_UNIT) {
        updateDate(tick_time);
    }
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    //TODO increment water count
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    goToFoodSelect(); // load food select window
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) { 
    goToLog(); // load log window
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
} 


static void updateTime(struct tm *tick_time) {
    strftime(timeString, MAX_TIME_CHAR, TIME_FORMAT, tick_time);

    /* remove preceding 0 if there is one */
    if ( !strncmp(timeString, "0", 1) ) {

        // skip the first character and change the time
        timeString++;
        text_layer_set_text(timeText, timeString);    
        timeString--; // to prevent free error

    } else {

        // push time string changes to display
        text_layer_set_text(timeText, timeString);

    }
}

static void updateDate(struct tm *tick_time) {
    strftime(dateString, MAX_DATE_CHAR, DATE_FORMAT, tick_time);
    text_layer_set_text(dateText, dateString);    
}

