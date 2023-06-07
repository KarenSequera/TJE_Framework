#pragma once
#include <vector>
#include "world.h"

class Camera;
class Stage {
public:

	Camera* camera;
	
	bool mouse_locked;

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void render() {};
	virtual void update(float dt) {};
};

class GameOverStage : public Stage
{
public:
	GameOverStage() {};
	void render();
	void update(float dt) {};
};

