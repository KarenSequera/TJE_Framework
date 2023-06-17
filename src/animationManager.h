#pragma once
#include "animation.h"
#include "mesh.h"

#include <vector>

#define NUM_ZOMBIE_IDLES 3
#define START_IDLES 4

enum playerStates { 
	PLAYER_IDLE = 4,
	PLAYER_BAT_IDLE = 5,
	PLAYER_KNIFE_IDLE = 6,
	PLAYER_GUN_IDLE = 7,
};

enum zombieStates {
	ZOMBIE_PUNCH = 0,
	ZOMBIE_KNIFE = 2,
	ZOMBIE_GUN = 3,
	ZOMBIE_IDLE = 4,
	ZOMBIE_HURT = 5,
	ZOMBIE_HURT_GRAVE = 6
};

enum commonStates {
	PUNCH = 0,
	BAT_SWING = 1,
	STAB = 2,
	SHOOT = 3
};

// all classes related to stages
class AnimationManager {
private:
	int target_state;
	
	Skeleton blended_skeleton;

	//float cur_time;
	//float target_time;

	float transition_counter;
	float transition_time;

public:
	std::map<int, Animation*> states;

	std::vector<std::string> zombie_idle_paths;

	int cur_state;

	AnimationManager();

	void fillCommonAnimations();
	void fillPlayerAnimations();
	void fillZombieAnimations(int idle);

	void update(float dt);

	void addAnimationState(const char* path, int state);
	float goToState(int state, float time = 0.f);
	Skeleton& getCurrentSkeleton();

};
