#pragma once
#include "globals.h"

bool loadWav(const char *path, WavBuffer &out);
void updateProximitySound(float distanceCm, Zone zone);
void stopProximitySound();
