/* ========================================================================== */
/* 
 * sprite.h
 *
 * Project Name: Food Baby
 * Author: cheniel
 *
 * Declarations for public functions used to control sprites and access sprite
 * state
 */
/* ========================================================================== */
#ifndef SPRITE_H
#define SPRITE_H

// ---------------- Prerequisites e.g., Requires "math.h"

// ---------------- Constants

// ---------------- Structures/Types

// ---------------- Public Variables

// ---------------- Prototypes/Macros


void initSprite(Layer* windowLayer);

void deinitSprite();

void startAnimation();
void stopAnimation();

void happyJumps();

void wakeUp();
bool animationIsReady();

#endif // SPRITE_H
