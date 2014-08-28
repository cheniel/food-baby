/* ========================================================================== */
/* 
 *
 *
 *
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
// #include <stdio.h>

// ---------------- Local includes  e.g., "file.h"
#include "data.h"                       

// ---------------- Constant definitions
const servingCount minRecServings = {
	.water = 6,
	.grains = 6,
	.veggies = 3,
	.fruit = 2,
	.dairy = 2,
	.protein = 2
};

const servingCount maxRecServings = {
	.water = 8,
	.grains = 11,
	.veggies = 5,
	.fruit = 4,
	.dairy = 3,
	.protein = 3
};

// ---------------- Macro definitions
// #define CONSTANT_VAR 42

// ---------------- Structures/Types

// ---------------- Private variables
servingCount userServings;

// ---------------- Private prototypes


/* ========================================================================== */

void initData() {
 	userServings = (servingCount) {
 		.water = 0,
 		.grains = 0,
 		.veggies = 0,
 		.fruit = 0,
 		.dairy = 0,
 		.protein = 0
 	};
}



