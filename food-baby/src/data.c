/* ========================================================================== */
/* 
 * data.c
 *
 * functions that have to do with persistent storage
 */
/* ========================================================================== */

// ---------------- Open Issues

// ---------------- System includes e.g., <stdio.h>
#include <pebble.h>

// ---------------- Local includes  e.g., "file.h"
#include "data.h"   
#include "common.h"                    

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
#define PKEY_FOOD_COUNT 1105551231
#define PKEY_PREV_DATE 1108424242

// number of food groups that need to be satisfied for sprite to be content
#define SATISFIED_THRES 3

// ---------------- Structures/Types

// ---------------- Private variables
ServingCount userServings;
int activityToday;
int activityRecord;
bool minSatisfied;
char *previousDate;
int minutesSinceLastActivity;
SpriteState currentSpriteState;

// ---------------- Private prototypes
static bool spriteIsContent();

/* ========================================================================== */

/*
 * read persistent data from pebble, set if unassigned.
 */
void initData() {
	if (persist_exists(PKEY_FOOD_COUNT)) {
		persist_read_data(PKEY_FOOD_COUNT, &userServings, sizeof(userServings));
	} else {
	 	userServings = (ServingCount) {
	 		.water = 0,
	 		.grains = 0,
	 		.veggies = 0,
	 		.fruit = 0,
	 		.dairy = 0,
	 		.protein= 0,
	 	};		
	}

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

 	minSatisfied = false;

	APP_LOG(APP_LOG_LEVEL_DEBUG, "previousDate is %s", previousDate);
}

/*
 * returns the sprite state based on the persistent data
 */
SpriteState getSpriteState() {

	// if the pebble has been idle for TIME_TO_SLEEP minutes
	// set sprite to asleep
	if (minutesSinceLastActivity >= TIME_TO_SLEEP) {
		currentSpriteState = spriteAsleep;

	// if all of the minimum recommendations are satisfied
	// set sprite to happy
	} else if (minSatisfied) {
		currentSpriteState = spriteHappy;

	// if sprite should be content, set sprite as content
	} else if (spriteIsContent()) {
		currentSpriteState = spriteContent;

	// else just set sprite as sad
	} else {
		currentSpriteState = spriteSad;
	}

	return currentSpriteState;
}

/*
 * determines if sprite should be content
 * returns true if at least SATISFIED_THRES food groups are satisfied.
 */
static bool spriteIsContent() {
	int remainingGroups = NUM_OF_FOOD_GROUPS;
	int satisfiedGroups = 0;

	double pwater = (double) userServings.water / 
					(double) minRecServings.water;
	if (pwater >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	double pgrains = (double) userServings.grains / 
					(double) minRecServings.grains;
	if (pgrains >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	double pveggies = (double) userServings.veggies / 
					(double) minRecServings.veggies;
	if (pveggies >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	double pfruit = (double) userServings.fruit / 
					(double) minRecServings.fruit;
	if (pfruit >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	double pdairy = (double) userServings.dairy / 
					(double) minRecServings.dairy;
	if (pdairy >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	double pprotein = (double) userServings.protein / 
					(double) minRecServings.protein;
	if (pprotein >= 1.0) { satisfiedGroups++; }
	if (satisfiedGroups >= SATISFIED_THRES) { return true; }
	remainingGroups--;
	if (remainingGroups + satisfiedGroups < SATISFIED_THRES) { return false; }

	return false;
}

/*
 * gets recommendation for next food to eat.
 * returns the food group with the lowest user eated / recommendation ratio, 
 * unless all max recommendations have been satisfied
 */
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

		// if not all proportions are satisfied
		if (!allSatisfied) {

			// return the food group with the lowest proportion
			double lowestP = 1000.0;

			if (pwater < lowestP) {
				recommendedFood = water;
				lowestP = pwater;
			} 

			if (pgrains < lowestP) {
				recommendedFood = grains;
				lowestP = pgrains;
			} 

			if (pveggies < lowestP) {
				recommendedFood = veggies;
				lowestP = pveggies;
			} 

			if (pfruit < lowestP) {
				recommendedFood = fruit;
				lowestP = pfruit;
			} 

			if (pdairy < lowestP) {
				recommendedFood = dairy;
				lowestP = pdairy;
			} 

			if (pprotein < lowestP) {
				recommendedFood = protein;
				lowestP = pprotein;
			} 

			return recommendedFood;

		// if min are satisfied (and all proportions are satisfied)
		// means max recommendations are satisfied
		} else if (minSatisfied) {
			return none; 

		// else, that means the min has been satisfied, so set min satisfied
		// to true to check proportions again
		} else {
			minSatisfied = true;
		}
	}
}

/* 
 * function that resets daily data
 * saves data
 */
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
 	minSatisfied = false;

 	saveData();
}

/*
 * resets record activity data
 * saves record at 0
 */
void resetRecord() {
	activityRecord = 0;
	persist_write_int(PKEY_ACT_RECORD, activityRecord);
}

/*
 *	saves all persistent data
 */
void saveData() {
	APP_LOG(APP_LOG_LEVEL_DEBUG, "saving data");
	persist_write_int(PKEY_ACT_TODAY, activityToday);
	persist_write_int(PKEY_ACT_RECORD, activityRecord);
	persist_write_data(PKEY_FOOD_COUNT, &userServings, sizeof(userServings));
	persist_write_string(PKEY_PREV_DATE, previousDate);
}

/*
 * frees the strings
 */
void freeResources() {
	free(previousDate);
}

/*
 * check if the date in the parameter is different from the date that was last
 * stored in persistent data
 */
bool isNewDate(char* currentDate) {
	return strncmp(currentDate, previousDate, MAX_DATE_CHAR);
}

/*
 * set a new date in persistent data.
 */
void setNewDate(char* newDate) {
	strncpy(previousDate, newDate, MAX_DATE_CHAR);
	persist_write_string(PKEY_PREV_DATE, previousDate);
}
