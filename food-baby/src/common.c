/* ========================================================================== */
/* 
 *
 */
/* ========================================================================== */
// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>    
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "common.h"

// ---------------- Constant definitions

// ---------------- Macro definitions

// ---------------- Structures/Types

// ---------------- Private variables
Window *home;
Window *foodSelect;
Window *logview;

// ---------------- Private prototypes

/* ========================================================================== */

void goToLog() {
  window_stack_push(logview, ANIMATION_SETTING);
}

void goToFoodSelect() {
  window_stack_push(foodSelect, ANIMATION_SETTING);
}

void setTextLayerDefaults(TextLayer *textlayer) {
  text_layer_set_text_color(textlayer, FONT_COLOR);
  text_layer_set_background_color(textlayer, BACKGROUND_COLOR);
  text_layer_set_text_alignment(textlayer, GTextAlignmentCenter);
}

