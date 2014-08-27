/* ========================================================================== */
/* 
 *
 *
 *
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"

// ---------------- Constant definitions
char* servingSizeGrains[] = {
  "1 slice of bread",
};

char* servingSizeVegetables[] = {
  "1 cup of broccoli",
};

char* servingSizeFruits[] = {
  "1/2 a large apple",
  "a banana",
};

char* servingSizeDairy[] = {
  "a cup of milk",
};

char* servingSizeMeat[] = {
  "3 oz of meat",
};

// ---------------- Macro definitions
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5

// ---------------- Structures/Types

// ---------------- Private variables
static SimpleMenuLayer *simple_menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem menu_items[NUM_MENU_ITEMS];

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);

/* ========================================================================== */

Window *foodInit() {
	Window *window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
	  .load = load,
	  .unload = unload,
	});	

	return window;
}

// You can capture when the user selects a menu icon with a menu item select callback
static void menu_select_callback(int index, void *ctx) {
  // Here we just change the subtitle to a literal string
  menu_items[index].subtitle = "You've hit select here!";
  // Mark the layer to be updated
  layer_mark_dirty(simple_menu_layer_get_layer(simple_menu_layer));
}

static void load(Window *window) {
  int num_a_items = 0;

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Grains",
    .subtitle = "e.g. 1 slice of bread",
    .callback = menu_select_callback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Vegetable",
    .subtitle = "e.g. 1 cup of broccoli",
    .callback = menu_select_callback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Fruit",
    .subtitle = "e.g. 1/2 a large apple",
    .callback = menu_select_callback,
    // This is how you would give a menu item an icon
    // .icon = menu_icon_image,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Dairy",
    .subtitle = "e.g. a cup of milk",
    .callback = menu_select_callback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Protein",
    .subtitle = "e.g. 3 oz of meat",
    .callback = menu_select_callback,
  };

  // Bind the menu items to the corresponding menu sections
  menu_sections[0] = (SimpleMenuSection){
    .title = "Log your servings!",
    .num_items = NUM_MENU_ITEMS,
    .items = menu_items,
  };

  // Now we prepare to initialize the simple menu layer
  // We need the bounds to specify the simple menu layer's viewport size
  // In this case, it'll be the same as the window's
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  // Initialize the simple menu layer
  simple_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);

  // Add it to the window for display
  layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));

}

static void unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer);
}

