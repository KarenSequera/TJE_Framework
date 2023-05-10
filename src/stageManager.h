#pragma once
#include "stage.h"

// all classes related to stages
class StageManager {
public:
	Stage* cur_stage;

	StageManager();
	//~StageManager();

	void render();
	void update(float dt);
};
