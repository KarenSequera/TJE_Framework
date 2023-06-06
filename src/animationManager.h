#pragma once
#include "animation.h"
#include "mesh.h"

#include <vector>

enum playerStates { 
	PLAYER_PUNCH, 
	PLAYER_BAT, 
	PLAYER_KNIFE,
	PLAYER_GUN, 
	PLAYER_IDLE };

enum zombieStates{ZOMBIE_IDLE};

// all classes related to stages
class AnimationManager {
private:
	std::map<int, Animation*> states;
	int cur_state;
	int target_state;
	
	Skeleton blended_skeleton;

	float transition_counter;
	float transition_time;

public:

	AnimationManager();

	void fillPlayerAnimations();
	//void fillZombieAnimations();

	void update(float dt);

	void addAnimationState(const char* path, int state);
	void goToState(int state, float time = 0.f);
	Skeleton& getCurrentSkeleton();

};
