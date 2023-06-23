#pragma once
#include "animation.h"
#include "mesh.h"

#include <vector>

#define NUM_ZOMBIE_IDLES 3
#define START_IDLES 4
#define TRANSITION_TIME 0.75f


enum playerStates {
	PLAYER_HURT = -1,
	PLAYER_FISTS_IDLE = 5,
	PLAYER_BAT_IDLE = 6,
	PLAYER_KNIFE_IDLE = 7,
	PLAYER_GUN_IDLE = 8,
	PLAYER_DEFEND = 9
};

enum zombieStates {
	ZOMBIE_DODGE = -2,
	ZOMBIE_HURT = -1,
	ZOMBIE_PUNCH = 0,
	ZOMBIE_KNIFE = 2,
	ZOMBIE_GUN = 3,
	ZOMBIE_HURT_GRAVE = -2
};

enum commonStates {
	PUNCH = 0,
	BAT_SWING = 1,
	STAB = 2,
	SHOOT = 3,
	IDLE = 4,
	DYING = -3
};

// all classes related to stages
class AnimationManager {
private:
	int delayed_target_state;

	Skeleton blended_skeleton;

	float cur_time;
	float target_time;

	float transition_counter;
	float transition_time;
	float delayed_transition_time;
	float time_to_start;

public:
	std::map<int, Animation*> states;

	std::vector<std::string> zombie_idle_paths;

	int cur_state;
	int target_state;

	AnimationManager();

	void fillCommonAnimations();
	void fillPlayerAnimations();
	void fillZombieAnimations(int idle);

	void update(float dt);

	void addAnimationState(const char* path, int state);
	float goToState(int state, float time = 0.f);
	float goToStateDelayed(int state, float to_start, float time = 0.f);
	Skeleton& getCurrentSkeleton();

	bool isIdle();

};
