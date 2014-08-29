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
#define NUM_MENU_SECTIONS 2
#define NUM_FOOD_GROUPS 5
#define NUM_SERVING_EXAMPLES 8
#define NUM_SETTINGS 2
#define MAX_COMPARE_SIZE 10
#define ANIMATED_SETTING true

// ---------------- Structures/Types

// ---------------- Private variables
static SimpleMenuLayer *menuLayer;
static SimpleMenuSection menuSections[NUM_MENU_SECTIONS];
static SimpleMenuItem foodGroups[NUM_FOOD_GROUPS];
static SimpleMenuItem settings[NUM_SETTINGS];

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

  menuItem = 0;
  settings[menuItem++] = (SimpleMenuItem) {
    .title = "reset daily data",
    .subtitle = "eating and activity data",
    .callback = resetDailyData,
  };

  settings[menuItem++] = (SimpleMenuItem) {
    .title = "reset record",
    .subtitle = "activity record",
    .callback = resetRecord,
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

  // Add it to the window for display
  layer_add_child(window_layer, simple_menu_layer_get_layer(menuLayer));

}

static void unload(Window *window) {
  simple_menu_layer_destroy(menuLayer);
}


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
