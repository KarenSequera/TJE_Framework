#pragma once

#define DRIFT_THRESHOLD 0.01

#include "entity.h"

#include <vector>


class Camera;
class Stage {
public:

	Camera* camera;
	Entity* root;
	std::vector<Entity*> entities;
	bool mouse_locked; //tells if the mouse is locked (not seen)

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void render();
	virtual void update(float dt) {};
};

class PlayStage : public Stage {

public:

	float gamepad_sensitivity;


	PlayStage();

	void update(float dt);
};