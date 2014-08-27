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

const servingCount minRecServings = {
	.grains = 6,
	.veggies = 3,
	.fruit = 2,
	.protein = 2,
	.dairy = 2
}

const servingCount maxRecServings = {
	.grains = 11,
	.veggies = 5,
	.fruit = 4,
	.protein = 3,
	.dairy = 3
}

// ---------------- Structures/Types
typedef struct servingCount {
	int grains;
	int veggies;
	int fruit;
	int protein;
	int dairy;
} servingCount;

// ---------------- Public Variables
servingCount userServings;


// ---------------- Prototypes/Macros
void initData();

/* ========================================================================== */

#endif // DATA_H