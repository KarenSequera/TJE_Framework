#pragma once

#define DRIFT_THRESHOLD 0.01
#define DAY_TIME 60.f

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
	virtual void render() {};
	virtual void update(float dt) {};
};

class DayStage : public Stage {
public:

	float gamepad_sensitivity;

	const char* consumable_names[NUM_CONSUMABLES - NUM_SHIELD_ITEMS] = 
		{ "Bandages ", "Painkillers ", "First-aid Kit ", "Apple ", "Canned Beans ", "Burger "};

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


	int cur_night;
	int turns_to_day; 
	bool is_player_turn;

	//Variable that contains the index of the selected zombie
	// -1 if we are not in target selection
	int selected_target;

	void onEnter();
	void render();
	void update(float dt);

	void playerTurnUpdate();
	void zombieTurnUpdate();
	void playerTurnRender();
	void zombieTurnRender();

	void debugZombies();

	void newTurn();

};

class GameOverStage : public Stage
{
public:
	GameOverStage() {};
	void render();
	void update(float dt) {};
};
