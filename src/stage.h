#pragma once
#include <vector>
#include "world.h"
#include "audio.h"

class Camera;
class Stage {
public:

	Camera* camera;
	HCHANNEL channel;
	
	bool mouse_locked;

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void render() {};
	virtual void update(float dt, bool transitioning = false) {};
	virtual void resizeOptions(float width, float height) {};
	void stopMusic();
	void renderHealthBar(Vector3 position, float hp_ratio, Shader* shader, float width, float height);
	void renderHungerBar(Vector3 position, float hunger_ratio, Shader* shader, float width, float height);


};
