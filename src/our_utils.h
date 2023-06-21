#pragma once

int ourMod(int a, int b);
void changeOption(int to_add, int& selected_option, int max);
// Function that subtracts dt to "to_update" and returns true if < 0
bool shouldTrigger(float& to_update, float dt);