#pragma once
#include "dayStage.h"
#include "nightStage.h"
#include "introStage.h"
#include "gameOverStage.h"

#define STAGE_TRANSITION_TIME 4.f

// all classes related to stages
class StageManager {
public:
	
	static StageManager* inst;

	Mesh transition_quad;
	float transition_time;

	Stage* cur_stage;
	std::string next;

	StageManager(float window_width, float window_height);
	//~StageManager();

	std::unordered_map<std::string, Stage*> stages;
	std::unordered_map<std::string, std::string> transition_sounds;

	void render();
	void renderStageTransition();
	void update(float dt);
	void changeStage(std::string go_to);
	void resize(float width, float height);
};
