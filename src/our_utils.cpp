#include "our_utils.h"
int ourMod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}

void changeOption(int to_add, int& selected_option, int max) {
    selected_option = ourMod(selected_option + to_add, max);
}

bool shouldTrigger(float& to_update, float dt)
{
    if (to_update > 0.f)
    {
        to_update -= dt;
        if (to_update <= 0.f)
            return true;
    }
    return false;
}