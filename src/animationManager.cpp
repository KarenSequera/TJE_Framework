#include "animationManager.h"
#include "game.h"

#include <random>

AnimationManager::AnimationManager()
{
	cur_state = 0;
	target_state = -1;
    
    cur_time = 0.f;
    target_time = 0.f;

	transition_counter = 0.f;
	transition_time = 0.f;
}

void AnimationManager::fillPlayerAnimations()
{
    addAnimationState("data/characters/animations/player/idle.skanim", PLAYER_IDLE);
    addAnimationState("data/characters/animations/player/punch.skanim", PLAYER_PUNCH);
    addAnimationState("data/characters/animations/player/bat.skanim", PLAYER_BAT);
    addAnimationState("data/characters/animations/player/punch.skanim", PLAYER_KNIFE);
    addAnimationState("data/characters/animations/player/punch.skanim", PLAYER_GUN);
    
    cur_state = PLAYER_IDLE;
    //addAnimationState("data/characters/animaciones/player/", PLAYER_PUNCH);
}

void AnimationManager::fillZombieAnimations()
{
    addAnimationState("data/characters/animations/zombie/idle.skanim", ZOMBIE_IDLE);
    addAnimationState("data/characters/animations/zombie/hit.skanim", ZOMBIE_HURT);
    cur_state = ZOMBIE_IDLE;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dist(0, 100);

    cur_time = dist(gen);
}

void AnimationManager::addAnimationState(const char* path, int state) {
    states[state] = Animation::Get(path);
}

float AnimationManager::goToState(int state, float time)
{
    if (time == 0.f)
    {
        cur_state = state;
        return states[cur_state]->duration;
    }

    if (target_state == state)
        return 0.f;

    target_state = state;
    transition_time = time;
    return states[target_state]->duration;
}

void AnimationManager::update(float dt)
{
    float time = Game::instance->time;

    states[cur_state]->assignTime(cur_time);
    cur_time += dt;

    if (target_state != -1)
    {
        states[target_state]->assignTime(target_time);

        target_time += dt;
        transition_counter += dt;

        if (transition_counter >= transition_time) {
            cur_state = target_state;

            target_state = -1;
            cur_time = target_time;
            transition_counter = 0.f;
            target_time = 0.f;

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