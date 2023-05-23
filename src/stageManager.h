#pragma once
#include "stage.h"

// all classes related to stages
class StageManager {
public:
	
	DayStage* day;
	NightStage* night;

	Stage* cur_stage;

	StageManager();
	//~StageManager();

	std::unordered_map<Stage*, Stage*> next_stage;

	void render();
	void update(float dt);
	void changeStage();
};
