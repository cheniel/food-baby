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
#include "data.h"
#include "home.h"
#include "sprite.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define TIME_FORMAT "%I:%M"
#define DATE_FORMAT "%a, %b %e"
#define SIDEBAR_DISPLAY_TIME 2
#define SIDEBAR_XPOS 117
#define SIDEBAR_YPOS 55

#define RECOMMENDATION_XPOS 0
#define RECOMMENDATION_YPOS 145

#define DATE_XPOS 0 
#define DATE_YPOS 35

#define TIME_XPOS 0
#define TIME_YPOS 0

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer *timeText;
static TextLayer *dateText;
static TextLayer *recText;
static char *timeString;
static char *dateString;
static int secondsSinceLastAction;
extern int minutesSinceLastActivity;
static bool sidebarVisible;

static BitmapLayer *sidebarLayer;
static GBitmap *sidebarImg;

static BitmapLayer *foodIconLayer;
static GBitmap *foodIcon;

static BitmapLayer *waterIconLayer;
static GBitmap *waterIcon;

static BitmapLayer *logIconLayer;
static GBitmap *logIcon;

extern ServingCount userServings;

static Layer *windowLayer;
static GRect bounds;

// ---------------- Private prototypes
static void select_click_handler(ClickRecognizerRef recognizer, void *context);
static void up_click_handler(ClickRecognizerRef recognizer, void *context);
static void down_click_handler(ClickRecognizerRef recognizer, void *context);
static void click_config_provider(void *context);
static void tick_handler(struct tm *tick_time, TimeUnits units_changed);
static void addDateAndTime();
static void addLayersToWindow();
static void showSidebar();
static void hideSidebar();
static void createSidebar(int x, int y);

static void load(Window *window);
static void unload(Window *window);
static void updateTime(struct tm *tick_time);
static void updateDate(struct tm *tick_time);
static void secondHandler(struct tm *tick_time);

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
    windowLayer = window_get_root_layer(window);
    bounds = layer_get_bounds(windowLayer);

    /* initialize services */
    tick_timer_service_subscribe(SECOND_UNIT, tick_handler); 

    addDateAndTime();

    makeRecommendation();
    initSprite(windowLayer);
    createSidebar(SIDEBAR_XPOS, SIDEBAR_YPOS);

    secondsSinceLastAction = 0;
    minutesSinceLastActivity = 0;
    sidebarVisible = false;

    addLayersToWindow();
}

static void unload(Window *window) {
    free(timeString);
    free(dateString);
    text_layer_destroy(timeText);
    text_layer_destroy(dateText);
    text_layer_destroy(recText);

    bitmap_layer_destroy(sidebarLayer);
    gbitmap_destroy(sidebarImg);

    bitmap_layer_destroy(foodIconLayer);
    gbitmap_destroy(foodIcon);

    bitmap_layer_destroy(waterIconLayer);
    gbitmap_destroy(waterIcon);  
    
    bitmap_layer_destroy(logIconLayer);
    gbitmap_destroy(logIcon);  

    deinitSprite();

    tick_timer_service_unsubscribe();
}

static void addDateAndTime() {
    /* create time text */
    timeString = calloc(MAX_TIME_CHAR, sizeof(char));
    timeText = text_layer_create((GRect) { 
        .origin = { TIME_XPOS, TIME_YPOS }, 
        .size = { bounds.size.w, 34 } 
    });
    setTextLayerDefaults(timeText);
    text_layer_set_font(timeText, 
        fonts_get_system_font(FONT_KEY_BITHAM_34_MEDIUM_NUMBERS));
    
    /* create date text */
    dateString = calloc(MAX_DATE_CHAR, sizeof(char));
    dateText = text_layer_create((GRect) { 
        .origin = { DATE_XPOS, DATE_YPOS }, 
        .size = { bounds.size.w, 16 } 
    });
    setTextLayerDefaults(dateText);
    text_layer_set_font(dateText, 
        fonts_get_system_font(FONT_KEY_GOTHIC_14));

    /* update times */
    time_t currentTime = time(NULL);
    struct tm* tm = localtime(&currentTime);
    updateTime(tm);
    updateDate(tm);
}

void makeRecommendation() {
    Foods recommendedFood = getRecommendation();

    if (!recText) {
        /* create recommendation text */
        recText = text_layer_create((GRect) { 
            .origin = { RECOMMENDATION_XPOS, RECOMMENDATION_YPOS }, 
            .size = { bounds.size.w, 20 } 
        });
        setTextLayerDefaults(recText);
        text_layer_set_font(recText, 
            fonts_get_system_font(FONT_KEY_GOTHIC_14_BOLD));
    }

    text_layer_set_text(recText, getRecommendationForFood(recommendedFood));
}

static void createSidebar(int x, int y) {
    /* create sidebar */
    sidebarImg = gbitmap_create_with_resource(RESOURCE_ID_SIDEBAR);
    sidebarLayer = bitmap_layer_create(GRect(x, y, 30, 85));
    bitmap_layer_set_bitmap(sidebarLayer, sidebarImg);

    /* create food icon */
    foodIcon = gbitmap_create_with_resource(RESOURCE_ID_FOOD);
    foodIconLayer = bitmap_layer_create(GRect(x + 2, y + 3, 25, 25));
    bitmap_layer_set_bitmap(foodIconLayer, foodIcon);

    /* create water icon */
    waterIcon = gbitmap_create_with_resource(RESOURCE_ID_WATER);
    waterIconLayer = bitmap_layer_create(GRect(x + 2, y + 30, 25, 25));
    bitmap_layer_set_bitmap(waterIconLayer, waterIcon);

    /* create log icon */
    logIcon = gbitmap_create_with_resource(RESOURCE_ID_LOG);
    logIconLayer = bitmap_layer_create(GRect(x + 2, y + 57, 25, 25));
    bitmap_layer_set_bitmap(logIconLayer, logIcon);
}

static void showSidebar() {
    APP_LOG(APP_LOG_LEVEL_DEBUG, "showing sidebar");

    layer_add_child(windowLayer, bitmap_layer_get_layer(sidebarLayer));
    layer_add_child(windowLayer, bitmap_layer_get_layer(foodIconLayer));
    layer_add_child(windowLayer, bitmap_layer_get_layer(waterIconLayer));
    layer_add_child(windowLayer, bitmap_layer_get_layer(logIconLayer));    

    secondsSinceLastAction = 0;
    sidebarVisible = true;
}

static void addLayersToWindow() {
    /* add text layers to window */
    layer_add_child(windowLayer, text_layer_get_layer(timeText));
    layer_add_child(windowLayer, text_layer_get_layer(dateText));
    layer_add_child(windowLayer, text_layer_get_layer(recText));
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed) {
    if (units_changed & SECOND_UNIT) { secondHandler(tick_time); }
    if (units_changed & MINUTE_UNIT) { updateTime(tick_time); }
    if (units_changed & DAY_UNIT) { updateDate(tick_time); }
}

static void secondHandler(struct tm *tick_time) {
    if (sidebarVisible) {
        secondsSinceLastAction++;
        if (secondsSinceLastAction > SIDEBAR_DISPLAY_TIME) { hideSidebar(); }
    }
}

static void hideSidebar() {
    layer_remove_from_parent(bitmap_layer_get_layer(sidebarLayer));
    layer_remove_from_parent(bitmap_layer_get_layer(foodIconLayer));
    layer_remove_from_parent(bitmap_layer_get_layer(waterIconLayer));
    layer_remove_from_parent(bitmap_layer_get_layer(logIconLayer));  
    sidebarVisible = false;
}

static void up_click_handler(ClickRecognizerRef recognizer, void *context) {
    wakeUp();
    if (sidebarVisible) { 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "showing food select window");
        stopAnimation();
        goToFoodSelect(); // load food select window
    } else { 
        showSidebar(); // or show sidebar
    } 
}

static void select_click_handler(ClickRecognizerRef recognizer, void *context) {
    wakeUp();
    if (sidebarVisible) { 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "incrementing water");
        userServings.water++; // increment water count
        happyJumps();
    } else { 
        showSidebar(); // or show sidebar
    } 

    makeRecommendation();
}

static void down_click_handler(ClickRecognizerRef recognizer, void *context) { 
    wakeUp();
    if (sidebarVisible) { 
        APP_LOG(APP_LOG_LEVEL_DEBUG, "opening logview");
        stopAnimation();
        goToLog(); // load log window
    } else { 
        showSidebar(); // or show sidebar 
    }    
}

static void click_config_provider(void *context) {
    window_single_click_subscribe(BUTTON_ID_SELECT, select_click_handler);
    window_single_click_subscribe(BUTTON_ID_UP, up_click_handler);
    window_single_click_subscribe(BUTTON_ID_DOWN, down_click_handler);
} 


static void updateTime(struct tm *tick_time) {
    minutesSinceLastActivity++;

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
    APP_LOG(APP_LOG_LEVEL_DEBUG, "updating date");
    strftime(dateString, MAX_DATE_CHAR, DATE_FORMAT, tick_time);
    for(int i = 0; dateString[i]; i++){
      dateString[i] = tolower((unsigned char) dateString[i]);
    }
    text_layer_set_text(dateText, dateString);    

    if (isNewDate(dateString)) {
        APP_LOG(APP_LOG_LEVEL_DEBUG, "new date detected!");
        setNewDate(dateString);
        resetDailyData();
        saveData();
        makeRecommendation();
    }
}

