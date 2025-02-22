#pragma once
#include "stage.h"
#define OPTIONS_INTRO_MENU 2

class IntroStage : public Stage {
public:

	IntroStage();

	// Menus
	float window_width;
	float window_height;
	Mesh* option_quads[OPTIONS_INTRO_MENU];
	Vector2 option_uses_pos[OPTIONS_INTRO_MENU];
	Mesh logo; 
	Mesh a_to_select;
	Camera* camera2D;

	std::vector<MenuEntity*> options;

	int selected_option;
	void onEnter();
	void onExit();
	void render();

	void update(float dt, bool transitioning = false);

	void resizeOptions(float width, float height);
	bool selectOption();

};
