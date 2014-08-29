/* ========================================================================== */
/* 
 * common.c 
 *
 * contains common functions for food baby
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

char* getRecommendationForFood(Foods food) {
  switch (food) {
    case water:
      return "drink some water!";
      break;
    case grains:
      return "have some grains";
      break;
    case veggies:
      return "eat your veggies!";
      break;
    case fruit:
      return "try some fruit";
      break;
    case dairy:
      return "have some dairy";
      break;
    case protein:
      return "make sure to get protein";
      break;
    case none:
      return "good job!";
      break;
    default:
      return "ERR no food given";
      break;
  }
}

// CREDIT:
// thanks Ryan Reich
// http://stackoverflow.com/questions/2509679/how-to-generate-a-random-number-
// from-within-a-range
// generates a random number in a range given by min and max.
/* Would like a semi-open interval [min, max) */
int randomInRange (unsigned int min, unsigned int max)
{
  int base_random = rand(); /* in [0, RAND_MAX] */
  if (RAND_MAX == base_random) return randomInRange(min, max);
  /* now guaranteed to be in [0, RAND_MAX) */
  int range = max - min,
      remainder   = RAND_MAX % range,
      bucket      = RAND_MAX / range;
  /* There are range buckets, plus one smaller interval
     within remainder of RAND_MAX */
  if (base_random < RAND_MAX - remainder) {
    return min + base_random/bucket;
  } else {
    return randomInRange (min, max);
  }
}