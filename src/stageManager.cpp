#include "stageManager.h"

StageManager* StageManager::inst = NULL;

StageManager::StageManager() {
	inst = this;

	cur_stage = nullptr;

	stage["day"] = new DayStage();
	stage["night"] = new NightStage();
	stage["game over"] = new GameOverStage();
	stage["intro stage"] = new IntroStage();
	cur_stage = stage["intro stage"];
	cur_stage->onEnter();
//#if DEBUG
//	changeStage("night");
//#else
//	changeStage("day");
//#endif
}

void StageManager::render() {
	cur_stage->render();
}

void StageManager::changeStage(std::string go_to)
{
	cur_stage->onExit();
	cur_stage = stage[go_to];
	cur_stage->onEnter();
}

void StageManager::update(float dt) {
	cur_stage->update(dt);
}