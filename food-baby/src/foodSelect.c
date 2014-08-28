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
#include "data.h"

// ---------------- Constant definitions
char* servingSizeGrains[] = {
  "e.g. a slice of bread",
  "e.g. 1/2 cup of rice",
  "e.g. 1/2 cup of pasta",
  "e.g. 1/2 cup of oatmeal",
  "e.g. 1 oz muffin",
  "e.g. a cup of cereal",
  "e.g. 1/2 a bagel",
  "e.g. a large tortilla"
};

char* servingSizeVegetables[] = {
  "e.g. a cup of broccoli",
  "e.g. 2 cups leafy greens",
  "e.g. a sweet potato",
  "e.g. 6 spears asparagus",
  "e.g. a cup of mushrooms",
  "e.g. 8 oz tomato juice",
  "e.g. ten baby carrots",
  "e.g. an ear of corn"
};

char* servingSizeFruits[] = {
  "e.g. 1/2 a large apple",
  "e.g. a banana",
  "e.g. a large orange",
  "e.g. six strawberries",
  "e.g. fifteen grapes",
  "e.g. 1/2 a grapefruit",
  "e.g. cup of applesauce",
  "e.g. 4oz of orange juice"
};

char* servingSizeDairy[] = {
  "e.g. a cup of milk",
  "e.g. 1.5oz of cheese",
  "e.g. a cup of yogurt",
  "e.g. 1/2 cup of ice cream",
  "e.g. a cup of froyo",
  "e.g. a cup of soymilk",
  "e.g. a string cheese",
  "e.g. a slice of cheese"
};

char* servingSizeProteins[] = {
  "e.g. 3 oz of beef",
  "e.g. 3 oz of chicken",
  "e.g. 3 oz of fish",
  "e.g. 3 oz of pork",
  "e.g. a egg",
  "e.g. 1/2 cup of beans",
  "e.g. 3 oz of turkey",
  "e.g. 1 oz of nuts"
};

// ---------------- Macro definitions
#define NUM_MENU_SECTIONS 1
#define NUM_MENU_ITEMS 5
#define NUM_SERVING_EXAMPLES 8
#define MAX_COMPARE_SIZE 10
#define ANIMATED_SETTING true

// ---------------- Structures/Types

// ---------------- Private variables
static SimpleMenuLayer *simple_menu_layer;
static SimpleMenuSection menu_sections[NUM_MENU_SECTIONS];
static SimpleMenuItem menu_items[NUM_MENU_ITEMS];

extern ServingCount userServings;

// ---------------- Private prototypes
static char* getServingExample(char* foodGroup);
static void load(Window *window);
static void unload(Window *window);
static void grainSelectCallback(int index, void *ctx);
static void vegetableSelectCallback(int index, void *ctx);
static void fruitSelectCallback(int index, void *ctx);
static void dairySelectCallback(int index, void *ctx);
static void proteinSelectCallback(int index, void *ctx);

/* ========================================================================== */

Window *foodInit() {
	Window *window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
	  .load = load,
	  .unload = unload,
	});	

	return window;
}

static void grainSelectCallback(int index, void *ctx) {
  userServings.grains++;
  window_stack_pop(ANIMATED_SETTING);
}

static void vegetableSelectCallback(int index, void *ctx) {
  userServings.veggies++;
  window_stack_pop(ANIMATED_SETTING);
}

static void fruitSelectCallback(int index, void *ctx) {
  userServings.fruit++;
  window_stack_pop(ANIMATED_SETTING);
}

static void dairySelectCallback(int index, void *ctx) {
  userServings.dairy++;
  window_stack_pop(ANIMATED_SETTING);
}

static void proteinSelectCallback(int index, void *ctx) {
  userServings.protein++;
  window_stack_pop(ANIMATED_SETTING);
}


static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_frame(window_layer);

  int num_a_items = 0;

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Grains",
    .subtitle = getServingExample("Grains"),
    .callback = grainSelectCallback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Vegetables",
    .subtitle = getServingExample("Vegetables"),
    .callback = vegetableSelectCallback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Fruits",
    .subtitle = getServingExample("Fruits"),
    .callback = fruitSelectCallback,
    // This is how you would give a menu item an icon
    // .icon = menu_icon_image,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Dairy",
    .subtitle = getServingExample("Dairy"),
    .callback = dairySelectCallback,
  };

  menu_items[num_a_items++] = (SimpleMenuItem){
    .title = "Proteins",
    .subtitle = getServingExample("Proteins"),
    .callback = proteinSelectCallback,
  };

  // Bind the menu items to the corresponding menu sections
  menu_sections[0] = (SimpleMenuSection){
    .title = "Log your servings!",
    .num_items = NUM_MENU_ITEMS,
    .items = menu_items,
  };

  // Initialize the simple menu layer
  simple_menu_layer = simple_menu_layer_create(bounds, window, menu_sections, NUM_MENU_SECTIONS, NULL);

  // Add it to the window for display
  layer_add_child(window_layer, simple_menu_layer_get_layer(simple_menu_layer));

}

static void unload(Window *window) {
  simple_menu_layer_destroy(simple_menu_layer);
}


char* getServingExample(char* foodGroup) {
  char** examples;

  if (!strncmp(foodGroup, "Grains", MAX_COMPARE_SIZE)) {
    examples = servingSizeGrains;
  } else if (!strncmp(foodGroup, "Vegetables", MAX_COMPARE_SIZE)) {
    examples = servingSizeVegetables;
  } else if (!strncmp(foodGroup, "Fruits", MAX_COMPARE_SIZE)) {
    examples = servingSizeFruits;
  } else if (!strncmp(foodGroup, "Dairy", MAX_COMPARE_SIZE)) {
    examples = servingSizeDairy;
  } else if (!strncmp(foodGroup, "Proteins", MAX_COMPARE_SIZE)) {
    examples = servingSizeProteins;
  } else {
    return "ERR inv group";
  }

  int randIndex = randomInRange(0, NUM_SERVING_EXAMPLES);

  return examples[randIndex];
}
