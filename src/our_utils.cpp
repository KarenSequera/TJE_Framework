#include "our_utils.h"
int ourMod(int a, int b) {
    int r = a % b;
    return r < 0 ? r + b : r;
}