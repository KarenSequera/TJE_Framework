#pragma once

#define DRIFT_THRESHOLD 0.01

#include <vector>


class Camera;
class Stage {
public:

	Camera* camera;
	
	bool mouse_locked;

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void render() {};
	virtual void update(float dt) {};
};

class DayStage : public Stage {

public:

	float gamepad_sensitivity;

	DayStage();

	void render();
	void update(float dt);
};