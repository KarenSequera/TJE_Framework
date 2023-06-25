#pragma once
#include "stage.h"
#define HEALTH_BAR_WIDTH 50.f
#define HEALTH_BAR_HEIGHT 10.f
#define TIME_BTW_TURNS 1.f

class NightStage : public Stage {
public:

	NightStage();


	int cur_night;
	int turns_to_day; 
	bool is_player_turn;
	bool to_day;

	bool free_cam_enabled;
	float n_angle;
	float time_between_turns;

	//Variable that contains the index of the selected zombie
	// -1 if we are not in target selection
	int selected_target;
	int zombie_attacking;
	Mesh background;
	Mesh night_hud;

	void onEnter();
	void onExit();
	void render();
	void renderCrosshair(Shader* shader);
	void renderHealthBars(Shader* shader);
	void renderBackground(Shader* shader);
	void renderPlayerStats(Shader* shader);

	void update(float dt, bool transitioning);

	void playerTurnUpdate(float dt);
	void zombieTurnUpdate(float dt);
	void cameraUpdate(float dt);
	void playerTurnRender();

	void newTurn();


	void resizeOptions(float width, float height);

};
