#pragma once

int ourMod(int a, int b);

// Function that subtracts dt to "to_update" and returns true if < 0
bool shouldTrigger(float& to_update, float dt);