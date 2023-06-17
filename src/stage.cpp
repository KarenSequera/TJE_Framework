#include "StageManager.h"
#include "input.h"
#include "camera.h"

#include <algorithm>

Stage::Stage() {
	camera = Camera::current;
	mouse_locked = false;
}

