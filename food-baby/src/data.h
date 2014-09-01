/* ========================================================================== */
/* 
 * data.h
 * 
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Contains function declarations for functions that have to deal with 
 * instantiating and modifying persistent data.
 */
/* ========================================================================== */
#ifndef DATA_H
#define DATA_H

// ---------------- Prerequisites e.g., Requires "math.h"
#include "common.h"
 
// ---------------- Constants

// ---------------- Structures/Types

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