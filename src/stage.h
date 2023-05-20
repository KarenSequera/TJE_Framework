#pragma once

#define DRIFT_THRESHOLD 0.01

#include <vector>
#include "world.h"

class Camera;
class Stage {
public:

	Camera* camera;
	
	bool finished;
	bool mouse_locked;

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void onEnter() {};
	virtual void render() {};
	virtual void update(float dt) {};
};

class DayStage : public Stage {
public:

	float gamepad_sensitivity;

	const char* consumable_names[NUM_CONSUMABLES - NUM_SHIELD_ITEMS] = 
		{"Burger ", "Canned Beans ", "Apple ", "First-aid Kit ", "Painkillers ", "Bandages "};

	consumableType consumable_selected;
	float time_remaining;

	DayStage();

	void onEnter();
	void render();
	void renderConsumableMenu();
	void update(float dt);
	void updateMovement(float dt);
	void updateItemsAndStats();
};

class NightStage : public Stage {
public:

	NightStage();

	int number_nights;
	void onEnter();
	void render();
	void update(float dt);
};
