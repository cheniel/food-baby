/* ========================================================================== */
/* 
 * File: sprite.h
 *
 *
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

#endif // SPRITE_H
