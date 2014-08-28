/* ========================================================================== */
/* 
 *
 *
 *
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "data.h"                       

// ---------------- Constant definitions
const ServingCount minRecServings = {
	.water = 6,
	.grains = 6,
	.veggies = 3,
	.fruit = 2,
	.dairy = 2,
	.protein = 2
};

const ServingCount maxRecServings = {
	.water = 8,
	.grains = 11,
	.veggies = 5,
	.fruit = 4,
	.dairy = 3,
	.protein = 3
};

// ---------------- Macro definitions
// keys for persistant storage
#define PKEY_ACT_TODAY 0
#define PKEY_ACT_RECORD 1
#define PKEY_WATER 2
#define PKEY_GRAINS 3
#define PKEY_VEG 4
#define PKEY_FRUIT 5
#define PKEY_DAIRY 6
#define PKEY_PROTEIN 7

// ---------------- Structures/Types

// ---------------- Private variables
ServingCount userServings;
int activityToday;
int activityRecord;

// ---------------- Private prototypes


/* ========================================================================== */

void initData() {
 	userServings = (ServingCount) {
 		.water = persist_exists(PKEY_WATER) ? persist_read_int(PKEY_WATER):0,
 		.grains = persist_exists(PKEY_GRAINS) ? persist_read_int(PKEY_GRAINS):0,
 		.veggies = persist_exists(PKEY_VEG) ? persist_read_int(PKEY_VEG):0,
 		.fruit = persist_exists(PKEY_FRUIT) ? persist_read_int(PKEY_FRUIT):0,
 		.dairy = persist_exists(PKEY_DAIRY) ? persist_read_int(PKEY_DAIRY):0,
 		.protein= persist_exists(PKEY_PROTEIN)?persist_read_int(PKEY_PROTEIN):0,
 	};

 	activityToday = persist_exists(PKEY_ACT_TODAY)?
 					persist_read_int(PKEY_ACT_TODAY):0;
 	activityRecord = persist_exists(PKEY_ACT_RECORD)?
 					persist_read_int(PKEY_ACT_RECORD):0;
}

void saveData() {
	persist_write_int(PKEY_ACT_TODAY, activityToday);
	persist_write_int(PKEY_ACT_RECORD, activityRecord);
	persist_write_int(PKEY_WATER, userServings.water);
	persist_write_int(PKEY_GRAINS, userServings.grains);
	persist_write_int(PKEY_VEG, userServings.veggies);
	persist_write_int(PKEY_FRUIT, userServings.fruit);
	persist_write_int(PKEY_DAIRY, userServings.dairy);
	persist_write_int(PKEY_PROTEIN, userServings.protein);
}

