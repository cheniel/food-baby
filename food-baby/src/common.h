/* ========================================================================== */
/* 
 *
 */
/* ========================================================================== */
#ifndef COMMON_H
#define COMMON_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants
#define ANIMATION_SETTING true

#define BACKGROUND_COLOR GColorBlack
#define FONT_COLOR GColorWhite

#define MAX_TIME_CHAR 10
#define MAX_DATE_CHAR 30

#define PEBBLE_WIDTH 144
#define PEBBLE_HEIGHT 168

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros
void goToLog();
void goToFoodSelect();
void setTextLayerDefaults(TextLayer *textlayer);
int randomInRange(unsigned int min, unsigned int max);

#endif // COMMON_H
