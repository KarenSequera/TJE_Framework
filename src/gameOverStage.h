#pragma once
#include "stage.h"
#define OPTIONS_INTRO_MENU 2
#define MAX_NAME_SIZE 15

struct ScoreUpdateResult {
	int maximumScore;
	bool isInTopThree;
};

class GameOverStage : public Stage {
public:

	GameOverStage();

	// Menus
	float window_width;
	float window_height;
	Mesh* option_quads[OPTIONS_INTRO_MENU];
	Vector2 option_uses_pos[OPTIONS_INTRO_MENU];


	std::string name;
	Camera* camera2D;

	std::vector<MenuEntity*> options;

	ScoreUpdateResult result;

	int selected_option;
	void onEnter();
	void onExit();
	void render();
	void renderNights();

	void update(float dt, bool transitioning);

	void onKeyDown(SDL_KeyboardEvent event);

	void resizeOptions(float width, float height);
	bool selectOption();
	ScoreUpdateResult updateScores(int number_nights);
};

