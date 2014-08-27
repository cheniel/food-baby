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

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros
void goToLog();
void goToFoodSelect();
void setTextLayerDefaults(TextLayer *textlayer);
int randomInRange(unsigned int min, unsigned int max);

#endif // COMMON_H
