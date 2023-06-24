#pragma once
#include "stage.h"
#define OPTIONS_INTRO_MENU 2

class GameOverStage : public Stage {
public:

	GameOverStage();

	// Menus
	float window_width;
	float window_height;
	Mesh* option_quads[OPTIONS_INTRO_MENU];
	Vector2 option_uses_pos[OPTIONS_INTRO_MENU];

	Camera* camera2D;

	std::vector<MenuEntity*> options;

	int selected_option;
	void onEnter();
	void onExit();
	void render();
	void renderNights();

	void update(float dt);

	void resizeOptions(float width, float height);
	void changeOption(int to_add);
	bool selectOption();

};
