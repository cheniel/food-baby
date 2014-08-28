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
typedef struct servingCount {
	int water;
	int grains;
	int veggies;
	int fruit;
	int dairy;
	int protein;
} servingCount;

// ---------------- Public Variables

// ---------------- Prototypes/Macros
void initData();

/* ========================================================================== */

#endif // DATA_H