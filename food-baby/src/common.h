/* ========================================================================== */
/* 
 * common.h
 *
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Contains declarations for common functions, structures and datatypes.
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

#define TIME_TO_SLEEP 4
#define NUM_OF_FOOD_GROUPS 6

// ---------------- Structures/Types
typedef enum Foods {
	water,
	grains,
	veggies,
	fruit,
	dairy,
	protein,
	none,
} Foods;

typedef struct ServingCount {
	int water;
	int grains;
	int veggies;
	int fruit;
	int dairy;
	int protein;
} ServingCount;

typedef enum SpriteState {
    spriteAsleep,
    spriteSad,
    spriteContent,
    spriteHappy,
} SpriteState;

// ---------------- Public Variables

// ---------------- Prototypes/Macros
void goToLog();
void goToFoodSelect();
void setTextLayerDefaults(TextLayer *textlayer);
char* getRecommendationForFood(Foods food);
int randomInRange(unsigned int min, unsigned int max);

#endif // COMMON_H
