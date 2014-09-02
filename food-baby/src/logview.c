/* ========================================================================== */
/* 
 * logview.c
 *
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Contains functions used to display log view window, which is where the user
 * sees what food he/she has logged so far in the day, and how active he/she
 * has been. 
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"
#include "data.h"
#include "sprite.h"

// ---------------- Constant definitions

// ---------------- Macro definitions
#define NUM_ROWS 10
#define COL_WIDTH 85
#define ROW_HEIGHT 16
#define TABLE_XPOS 20
#define NORMAL_FONT fonts_get_system_font(FONT_KEY_GOTHIC_14)
#define MAX_COUNT_LENGTH 6

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer* column1[NUM_ROWS];
static TextLayer* column2[NUM_ROWS];
static char* col2values[NUM_ROWS];
static TextLayer* foodHeader;
static TextLayer* activityHeader;
extern ServingCount userServings;
extern int activityToday;
extern int activityRecord;

// ---------------- Private prototypes
static void load(Window *window);
static void unload(Window *window);
static void assignRow(int rowNumber, char* col1, int col2);
static void initializeTable();
static void setUpHeaders(GRect bounds);
static void addTextLayersToWindow(Layer* windowLayer);

/* ========================================================================== */

Window *logInit() {
	Window *window = window_create();
	window_set_window_handlers(window, (WindowHandlers) {
	  .load = load,
	  .unload = unload,
	});	

  window_set_fullscreen(window, true);
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

    col2values[row] = calloc(MAX_COUNT_LENGTH, sizeof(char));

    // set font for columns
    text_layer_set_font(column1[row], NORMAL_FONT);
    text_layer_set_font(column2[row], NORMAL_FONT);
  }
}

static void setUpHeaders(GRect bounds) {
  // set up header rows
  foodHeader = text_layer_create((GRect) { 
    .origin = { 5, 0 }, // row 0
    .size = { bounds.size.w, ROW_HEIGHT } 
  });
  text_layer_set_text(foodHeader, "food servings");

  activityHeader = text_layer_create((GRect) { 
    .origin = { 5, ROW_HEIGHT * 7 }, // row 6
    .size = { bounds.size.w, ROW_HEIGHT } 
  });
  text_layer_set_text(activityHeader, "activity");  
}

static void load(Window *window) {
  Layer *windowLayer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(windowLayer);  

  // initialize table
  initializeTable();
  setUpHeaders(bounds);

  int foodGroups = 1;
  assignRow(foodGroups++, "water (8 oz)", userServings.water);
  assignRow(foodGroups++, "grains", userServings.grains);
  assignRow(foodGroups++, "veggies", userServings.veggies);
  assignRow(foodGroups++, "fruits", userServings.fruit);
  assignRow(foodGroups++, "dairy", userServings.dairy);
  assignRow(foodGroups++, "protein", userServings.protein);

  int activityGroups = 8;
  assignRow(activityGroups++, "today", activityToday);
  assignRow(activityGroups++, "best", activityRecord); 

  addTextLayersToWindow(windowLayer);
}

static void unload(Window *window) {
  text_layer_destroy(foodHeader);
  text_layer_destroy(activityHeader);

  // destroy table
  for (int row = 0; row < NUM_ROWS; row++) {
    text_layer_destroy(column1[row]);
    text_layer_destroy(column2[row]);
    free(col2values[row]);
  }

  startAnimation();
}

static void addTextLayersToWindow(Layer* windowLayer) {
  // add table to window
  for (int row = 0; row < NUM_ROWS; row++) {
    layer_add_child(windowLayer, text_layer_get_layer(column1[row]));
    layer_add_child(windowLayer, text_layer_get_layer(column2[row]));
  }

  // add header rows to table
  layer_add_child(windowLayer, text_layer_get_layer(foodHeader));
  layer_add_child(windowLayer, text_layer_get_layer(activityHeader));
}

static void assignRow(int rowNumber, char* col1, int col2) {
  snprintf(col2values[rowNumber], MAX_COUNT_LENGTH, "%d", col2);
  text_layer_set_text(column1[rowNumber], col1);
  text_layer_set_text(column2[rowNumber], col2values[rowNumber]);
}

