#pragma once
#include "stage.h"
#define OPTIONS_INTRO_MENU 2
#define MAX_NAME_SIZE 15

#define NUM_RANKING 10

struct sRankingInfo {
	std::string user;
	int num_nights;
};

class GameOverStage : public Stage {
public:

	GameOverStage();

	// Menus
	float window_width;
	float window_height;
	int ranking_pos;
	Mesh* option_quads[OPTIONS_INTRO_MENU];
	Vector2 option_uses_pos[OPTIONS_INTRO_MENU];


	std::string name;
	Camera* camera2D;

	std::vector<MenuEntity*> options;

	int nights_survived;
	int stage;
	sRankingInfo ranking[NUM_RANKING];

	int selected_option;
	void onEnter();
	void onExit();
	void render();

	void update(float dt, bool transitioning);

	void onKeyDown(SDL_KeyboardEvent event);

	void resizeOptions(float width, float height);
	bool selectOption();
	void getRanking();
	void updateRanking();
};

