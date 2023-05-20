#include "stageManager.h"

StageManager::StageManager() {
	day = new DayStage();
	night = new NightStage();
	
	cur_stage = day;

	next_stage[day] = night;
	next_stage[night] = day;

	day->onEnter();
}

void StageManager::render() {
	cur_stage->render();
}

void StageManager::changeStage()
{
	cur_stage->finished = false;
	cur_stage = next_stage[cur_stage];
	cur_stage->onEnter();
}

void StageManager::update(float dt) {
	cur_stage->update(dt);

	if (cur_stage->finished)
		changeStage();
}