#pragma once
#include "animation.h"
#include "mesh.h"

#include <vector>

enum playerStates { 
	PLAYER_PUNCH, 
	PLAYER_BAT, 
	PLAYER_KNIFE,
	PLAYER_GUN, 
	PLAYER_IDLE 
};

enum zombieStates {
	ZOMBIE_HURT = -2,
	ZOMBIE_IDLE = -1,
	ZOMBIE_PUNCH = 0,
	ZOMBIE_KNIFE = 2,
	ZOMBIE_GUN = 3
};

// all classes related to stages
class AnimationManager {
private:
	int target_state;
	
	Skeleton blended_skeleton;

	float cur_time;
	float target_time;

	float transition_counter;
	float transition_time;

public:
	std::map<int, Animation*> states;
	int cur_state;

	AnimationManager();

	void fillPlayerAnimations();
	void fillZombieAnimations();

	void update(float dt);

	void addAnimationState(const char* path, int state);
	float goToState(int state, float time = 0.f);
	Skeleton& getCurrentSkeleton();

};
