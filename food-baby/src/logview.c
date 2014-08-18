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
// #define CONSTANT_VAR 42

// ---------------- Structures/Types

// ---------------- Private variables
static TextLayer *text_layer;

// ---------------- Private prototypes
static void click_config_provider(void *context);
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


static void load(Window *window) {
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  text_layer = text_layer_create((GRect) { .origin = { 0, 72 }, .size = { bounds.size.w, 20 } });
  text_layer_set_text(text_layer, "log view");
  text_layer_set_text_alignment(text_layer, GTextAlignmentCenter);
  layer_add_child(window_layer, text_layer_get_layer(text_layer));
}

static void unload(Window *window) {
  text_layer_destroy(text_layer);
}

