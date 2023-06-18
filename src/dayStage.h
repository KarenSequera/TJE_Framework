#pragma once
#include "stage.h"

#define DRIFT_THRESHOLD 0.01
#define DAY_TIME 60.f

class DayStage : public Stage {
public:

	Texture sky;
	Shader* sky_shader;

	float gamepad_sensitivity;

	const char* consumable_names[NUM_CONSUMABLES - NUM_SHIELD_ITEMS] = 
		{ "Bandages ", "Painkillers ", "First-aid Kit ", "Apple ", "Canned Beans ", "Burger "};

	Mesh HUD_quad;
	consumableType consumable_selected;
	float time_remaining;

	DayStage();

	void onEnter();
	void onExit();
	void render();
	void renderConsumableMenu();
	void renderSky();
	void renderHUD(Shader* shader);
	void update(float dt);
	void updateMovement(float dt);
	void updateItemsAndStats();


	void resizeOptions(int width, int height);
};
