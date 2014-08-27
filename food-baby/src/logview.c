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
// static const int = 5

// ---------------- Macro definitions
#define NUM_ROWS 9
#define COL_WIDTH 50
#define ROW_HEIGHT 16
#define TABLE_XPOS 10
#define NORMAL_FONT fonts_get_system_font(FONT_KEY_GOTHIC_14)

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer* column1[NUM_ROWS];
static TextLayer* column2[NUM_ROWS];

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);

/* ========================================================================== */

Window *logInit() {
	Window *window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
	  .load = load,
	  .unload = unload,
	});	

	return window;
}

static void initializeTable() {
  for (int row = 0; row < NUM_ROWS; row++) {
    column1[row] = text_layer_create((GRect) { 
      .origin = { TABLE_XPOS, ROW_HEIGHT * row }, 
      .size = { COL_WIDTH, ROW_HEIGHT } 
    });
    
    column2[row] = text_layer_create((GRect) { 
      .origin = { TABLE_XPOS + COL_WIDTH, ROW_HEIGHT * row }, 
      .size = { COL_WIDTH, ROW_HEIGHT } 
    });

    // set font for columns
    text_layer_set_font(column1[row], NORMAL_FONT);
    text_layer_set_font(column2[row], NORMAL_FONT);
  }
}

static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);  

  // set up header rows
  TextLayer* foodHeader = text_layer_create((GRect) { 
    .origin = { 0, 0 }, // row 0
    .size = { bounds.size.w, ROW_HEIGHT } 
  });
  text_layer_set_text(foodHeader, "Food Servings");

  TextLayer* activityHeader = text_layer_create((GRect) { 
    .origin = { 0, ROW_HEIGHT * 6 }, // row 6
    .size = { bounds.size.w, ROW_HEIGHT } 
  });
  text_layer_set_text(activityHeader, "Activity");

  // initialize table
  initializeTable();

  // add table to window
  for (int row = 0; row < NUM_ROWS; row++) {
    text_layer_set_text(column1[row], "testing");
    text_layer_set_text(column2[row], "1234");
    layer_add_child(window_layer, text_layer_get_layer(column1[row]));
    layer_add_child(window_layer, text_layer_get_layer(column2[row]));
  }

  // add header rows to table
  layer_add_child(window_layer, text_layer_get_layer(foodHeader));
  layer_add_child(window_layer, text_layer_get_layer(activityHeader));
}

static void unload(Window *window) {
  
  // destroy table
  for (int row = 0; row < NUM_ROWS; row++) {
    text_layer_destroy(column1[row]);
    text_layer_destroy(column2[row]);
  }
}

