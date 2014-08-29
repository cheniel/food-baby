/* ========================================================================== */
/* 
 *
 *
 *
 */
/* ========================================================================== */
#ifndef DATA_H
#define DATA_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types
typedef struct ServingCount {
	int water;
	int grains;
	int veggies;
	int fruit;
	int dairy;
	int protein;
} ServingCount;

typedef enum Foods {
	water,
	grains,
	veggies,
	fruit,
	dairy,
	protein,
	none,
} Foods;

// ---------------- Public Variables

// ---------------- Prototypes/Macros
void initData();
void saveData();
void resetDailyData();
void resetRecord();
void freeResources();
bool isNewDate(char* currentDate);
void setNewDate(char* newDate);
Foods getRecommendation();

/* ========================================================================== */

#endif // DATA_H