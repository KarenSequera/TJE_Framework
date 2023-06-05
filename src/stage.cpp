#include "StageManager.h"
#include "input.h"
#include "camera.h"

#include <algorithm>

Stage::Stage() {
	camera = Camera::current;
	mouse_locked = false;
}

void GameOverStage::render() {
	drawText(5, 25, "oops, you died!", Vector3(1.0f, 0.0f, 0.0f), 2);
}
