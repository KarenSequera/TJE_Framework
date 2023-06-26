#pragma once
#include "stage.h"

#define DRIFT_THRESHOLD 0.01
#define DAY_TIME 30.f
#define TUT_SLIDES_DAY 3

class DayStage : public Stage {
public:

	Texture sky;
	Shader* sky_shader;

	float gamepad_sensitivity;

	Mesh HUD_quad;
	Mesh instructions_quad;
	consumableType consumable_selected;
	float time_remaining;

	DayStage();

	void onEnter();
	void onExit();
	void render();
	void renderSky();
	void renderHUD();
	void update(float dt, bool transitioning = false);
	void updateMovement(float dt);
	void updateItemsAndStats();


	void getSlides();
	void resizeOptions(float width, float height);
};
