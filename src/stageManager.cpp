#include "stageManager.h"

StageManager* StageManager::inst = NULL;


StageManager::StageManager() {
	inst = this;

	stage["day"] = new DayStage();
	stage["night"] = new NightStage();
	stage["game over"] = new GameOver();

	changeStage("day");
}

void StageManager::render() {
	cur_stage->render();
}

void StageManager::changeStage(std::string go_to)
{
	cur_stage = stage[go_to];
	cur_stage->onEnter();
}

void StageManager::update(float dt) {
	cur_stage->update(dt);
}