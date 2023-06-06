#pragma once
#include "stage.h"
#define HEALTH_BAR_WIDTH 50.f
#define HEALTH_BAR_HEIGHT 10.f

class NightStage : public Stage {
public:

	NightStage();


	int cur_night;
	int turns_to_day; 
	bool is_player_turn;

	bool free_cam_enabled;
	float n_angle;

	//Variable that contains the index of the selected zombie
	// -1 if we are not in target selection
	int selected_target;

	void onEnter();
	void render();
	void renderCrosshair(Shader* shader);
	void renderHealthBars(Shader* shader);
	void renderHealthBar(Vector3 position, float hp_ratio, Shader* shader);

	void update(float dt);

	void playerTurnUpdate(float dt);
	void zombieTurnUpdate(float dt);
	void cameraUpdate(float dt);
	void playerTurnRender();
	void zombieTurnRender();

	void debugZombies();

	void newTurn();

};
