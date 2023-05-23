#pragma once
#include "stage.h"

// all classes related to stages
class StageManager {
public:
	
	static StageManager* inst;

	DayStage* day;
	NightStage* night;

	Stage* cur_stage;

	StageManager();
	//~StageManager();

	std::unordered_map<std::string, Stage*> stage;

	void render();
	void update(float dt);
	void changeStage(std::string go_to);
};
