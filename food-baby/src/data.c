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
#define PKEY_ACT_TODAY 1100424242
#define PKEY_ACT_RECORD 1101424242
#define PKEY_WATER 1102424242
#define PKEY_GRAINS 1103424242
#define PKEY_VEG 1104424242
#define PKEY_FRUIT 1105424242
#define PKEY_DAIRY 1106424242
#define PKEY_PROTEIN 1107424242
#define PKEY_PREV_DATE 1108424242

// ---------------- Structures/Types

// ---------------- Private variables
ServingCount userServings;
int activityToday;
int activityRecord;
bool minSatisfied;
char *previousDate;

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
 					persist_read_int(PKEY_ACT_RECORD):activityToday;

 	previousDate = calloc(MAX_DATE_CHAR, sizeof(char));
 	if (!persist_exists(PKEY_PREV_DATE)) {
 		APP_LOG(APP_LOG_LEVEL_DEBUG, "No previousDate found.");
 		strncpy(previousDate, "unset", MAX_DATE_CHAR);
 		persist_write_string(PKEY_PREV_DATE, previousDate);
 	} else {
 		persist_read_string(PKEY_PREV_DATE, previousDate, MAX_DATE_CHAR);
 	}

	APP_LOG(APP_LOG_LEVEL_DEBUG, "previousDate is %s", previousDate);
}

Foods getRecommendation() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "getting recommendation");

	double pwater;
	double pgrains;
	double pveggies;
	double pfruit;
	double pdairy;
	double pprotein;

	ServingCount servings;
	Foods recommendedFood = none;

	while (true) {
		
		// pick the number of servings to use
		if (minSatisfied) {
			servings = maxRecServings;
		} else {
			servings = minRecServings;
		}

		// get proportions
		pwater = (double) userServings.water / (double) servings.water;
		pgrains = (double) userServings.grains / (double) servings.grains;
		pveggies = (double) userServings.veggies / (double) servings.veggies;
		pfruit = (double) userServings.fruit / (double) servings.fruit;
		pdairy = (double) userServings.dairy / (double) servings.dairy;
		pprotein = (double) userServings.protein / (double) servings.protein;

		bool allSatisfied = pwater >= 1.0 && pgrains >= 1.0 && pveggies >= 1.0 
						&& pfruit >= 1.0 && pdairy >= 1.0 && pprotein >= 1.0;

		if (!allSatisfied) {

			double lowestP = 1000.0;

			if (pwater < lowestP) {
				recommendedFood = water;
				lowestP = pwater;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pwater at %d", (int) pwater * 100);

			} 

			if (pgrains < lowestP) {
				recommendedFood = grains;
				lowestP = pgrains;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pgrains at %d", (int) pgrains * 100);

			} 

			if (pveggies < lowestP) {
				recommendedFood = veggies;
				lowestP = pveggies;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pveggies at %d", (int) pveggies * 100);

			} 

			if (pfruit < lowestP) {
				recommendedFood = fruit;
				lowestP = pfruit;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pfruit at %d", (int) pfruit * 100);

			} 

			if (pdairy < lowestP) {
				recommendedFood = dairy;
				lowestP = pdairy;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pdairy at %d", (int) pdairy * 100);

			} 

			if (pprotein < lowestP) {
				recommendedFood = protein;
				lowestP = pprotein;
				APP_LOG(APP_LOG_LEVEL_DEBUG, "lowest set at pprotein at %d", (int) pprotein * 100);
			} 

			return recommendedFood;

		} else if (minSatisfied) {
			return none;
		} else {
			minSatisfied = true;
		}
	}
}

void resetDailyData() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "reset data called");
 	userServings = (ServingCount) {
 		.water = 0,
 		.grains = 0,
 		.veggies = 0,
 		.fruit = 0,
 		.dairy = 0,
 		.protein = 0
 	};

 	activityToday = 0;

 	saveData();
}

void resetRecord() {
	activityRecord = 0;
	saveData();
}

void saveData() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "saving data");
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_ACT_TODAY, activityToday));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_ACT_RECORD, activityRecord));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_WATER, userServings.water));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_GRAINS, userServings.grains));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_VEG, userServings.veggies));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_FRUIT, userServings.fruit));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_DAIRY, userServings.dairy));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", (int) persist_write_int(PKEY_PROTEIN, userServings.protein));
	APP_LOG(APP_LOG_LEVEL_DEBUG, "result %d", persist_write_string(PKEY_PREV_DATE, previousDate));

}

void freeResources() {
	free(previousDate);
}

bool isNewDate(char* currentDate) {
	return strncmp(currentDate, previousDate, MAX_DATE_CHAR);
}

void setNewDate(char* newDate) {
	strncpy(previousDate, newDate, MAX_DATE_CHAR);
	saveData();
}
