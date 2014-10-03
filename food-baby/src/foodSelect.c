/* ========================================================================== */
/* 
 * foodSelect.c
 * 
 * Project Name: Food Baby
 * Author: cheniel
 *
 * initialization and functions for food select window
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "data.h"
#include "servingSizeExamples.h"
#include "home.h"
#include "sprite.h"
#include "strap/strap.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define NUM_MENU_SECTIONS 2
#define NUM_FOOD_GROUPS 5
#define NUM_SETTINGS 2
#define MAX_COMPARE_SIZE 10 // for strings
#define ANIMATED_SETTING true

// ---------------- Structures/Types

// ---------------- Private variables
static SimpleMenuLayer *menuLayer;
static SimpleMenuSection menuSections[NUM_MENU_SECTIONS];
static SimpleMenuItem foodGroups[NUM_FOOD_GROUPS];
static SimpleMenuItem settings[NUM_SETTINGS];
static bool foodSelected;

extern ServingCount userServings;

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);

static void grainSelectCallback(int index, void *ctx);
static void vegetableSelectCallback(int index, void *ctx);
static void fruitSelectCallback(int index, void *ctx);
static void dairySelectCallback(int index, void *ctx);
static void proteinSelectCallback(int index, void *ctx);
static void resetDailyCallback(int index, void *ctx);
static void resetRecordCallback(int index, void *ctx);

static char* getServingExample(char* foodGroup);

/* ========================================================================== */

/*
 * initializes and returns food select window
 */
Window *foodInit() {
	Window *window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
	  .load = load,
	  .unload = unload,
	});	

	return window;
}

/*
 * triggered when grain selection is made.
 * increment grain count, get new recommendation, pop window
 */ 
static void grainSelectCallback(int index, void *ctx) {
  strap_log_event("/grain-select");
  userServings.grains++;
  foodSelected = true;
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when vegetable selection is made.
 * increment vegetable count, get new recommendation, pop window
 */ 
static void vegetableSelectCallback(int index, void *ctx) {
  strap_log_event("/vegetable-select");
  userServings.veggies++;
  foodSelected = true;
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when fruit selection is made.
 * increment fruit count, get new recommendation, pop window
 */ 
static void fruitSelectCallback(int index, void *ctx) {
  strap_log_event("/fruit-select");
  userServings.fruit++;
  foodSelected = true;
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when dairy selection is made.
 * increment dairy count, get new recommendation, pop window
 */ 
static void dairySelectCallback(int index, void *ctx) {
  strap_log_event("/dairy-select");
  userServings.dairy++;
  foodSelected = true;
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when protein selection is made.
 * increment protein count, get new recommendation, pop window
 */ 
static void proteinSelectCallback(int index, void *ctx) {
  strap_log_event("/protein-select");
  userServings.protein++;
  foodSelected = true;
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when reset daily data selection is made.
 * reset nutrition and daily activity data, get new recommendation, pop window
 */ 
static void resetDailyCallback(int index, void *ctx) {
  resetDailyData();
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * triggered when reset record selection is made.
 * reset activity record, get new recommendation, pop window
 */ 
static void resetRecordCallback(int index, void *ctx) {
  resetRecord();
  makeRecommendation();
  window_stack_pop(ANIMATED_SETTING);
}

/*
 * set up menu layer, put in buttons
 */
static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // nutrition callbacks
  int menuItem = 0;
  foodGroups[menuItem++] = (SimpleMenuItem) {
    .title = "grains",
    .subtitle = getServingExample("grains"),
    .callback = grainSelectCallback,
  };

  foodGroups[menuItem++] = (SimpleMenuItem) {
    .title = "vegetables",
    .subtitle = getServingExample("vegetables"),
    .callback = vegetableSelectCallback,
  };

  foodGroups[menuItem++] = (SimpleMenuItem) {
    .title = "fruits",
    .subtitle = getServingExample("fruits"),
    .callback = fruitSelectCallback,
  };

  foodGroups[menuItem++] = (SimpleMenuItem) {
    .title = "dairy",
    .subtitle = getServingExample("dairy"),
    .callback = dairySelectCallback,
  };

  foodGroups[menuItem++] = (SimpleMenuItem) {
    .title = "proteins",
    .subtitle = getServingExample("proteins"),
    .callback = proteinSelectCallback,
  };

  // reset data callbacks
  menuItem = 0;
  settings[menuItem++] = (SimpleMenuItem) {
    .title = "reset daily data",
    .subtitle = "eating and activity data",
    .callback = resetDailyCallback,
  };

  settings[menuItem++] = (SimpleMenuItem) {
    .title = "reset record",
    .subtitle = "activity record",
    .callback = resetRecordCallback,
  };

  // Bind the menu items to the corresponding menu sections
  menuSections[0] = (SimpleMenuSection) {
    .title = "log your servings!",
    .num_items = NUM_FOOD_GROUPS,
    .items = foodGroups,
  };

  menuSections[1] = (SimpleMenuSection) {
    .title = "settings",
    .num_items = NUM_SETTINGS,
    .items = settings,
  };

  // Initialize the simple menu layer
  menuLayer = simple_menu_layer_create(bounds, window, menuSections, 
    NUM_MENU_SECTIONS, NULL);

  // used in unload to determine what function to call for home window
  foodSelected = false; 

  // Add it to the window for display
  layer_add_child(window_layer, simple_menu_layer_get_layer(menuLayer));
}

/*
 * called when window is unloaded. calls happy jump if a food was selected,
 * starts animation based on state otherwise.
 */
static void unload(Window *window) {
  simple_menu_layer_destroy(menuLayer);

  if (foodSelected) {
    happyJumps();
  } else {
    startAnimation();
  }
}

/*
 * gets random serving example for food group given by string in parameter
 */
char* getServingExample(char* foodGroup) {
  char** examples;

  if (!strncmp(foodGroup, "grains", MAX_COMPARE_SIZE)) {
    examples = servingSizeGrains;
  } else if (!strncmp(foodGroup, "vegetables", MAX_COMPARE_SIZE)) {
    examples = servingSizeVegetables;
  } else if (!strncmp(foodGroup, "fruits", MAX_COMPARE_SIZE)) {
    examples = servingSizeFruits;
  } else if (!strncmp(foodGroup, "dairy", MAX_COMPARE_SIZE)) {
    examples = servingSizeDairy;
  } else if (!strncmp(foodGroup, "proteins", MAX_COMPARE_SIZE)) {
    examples = servingSizeProteins;
  } else {
    return "ERR inv group";
  }

  int randIndex = randomInRange(0, NUM_SERVING_EXAMPLES);

  return examples[randIndex];
}
