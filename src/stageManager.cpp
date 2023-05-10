#include "stageManager.h"

StageManager::StageManager() {
	cur_stage = new PlayStage();
}

void StageManager::render() {
	cur_stage->render();
}

void StageManager::update(float dt) {
	cur_stage->update(dt);
}