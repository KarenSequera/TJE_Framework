#include "animationManager.h"
#include "game.h"

AnimationManager::AnimationManager()
{
	cur_state = 0;
	target_state = -1;

	transition_counter = 0.f;
	transition_time = 0.f;
}

void AnimationManager::fillPlayerAnimations()
{
    addAnimationState("data/characters/idle_player.skanim", PLAYER_IDLE);
}

void AnimationManager::addAnimationState(const char* path, int state) {
    states[state] = Animation::Get(path);
}

void AnimationManager::goToState(int state, float time)
{
   
    if (time == 0.f)
    {
        cur_state = state;
        return;
    }
    if (target_state == state)
        return;
}

void AnimationManager::update(float dt)
{
    states[cur_state]->assignTime(Game::instance->time);
    if (target_state != -1)
    {
        states[target_state]->assignTime(Game::instance->time);

        transition_counter += dt;

        if (transition_counter >= transition_time) {
            cur_state = target_state;
            target_state = -1;
            return;
        }

        blendSkeleton(
            &states[cur_state]->skeleton,
            &states[target_state]->skeleton,
            transition_counter / transition_time,
            &blended_skeleton
        );
    }
};

Skeleton& AnimationManager::getCurrentSkeleton()
{
    if (target_state != -1)
        return blended_skeleton;
    return states[cur_state]->skeleton;
}