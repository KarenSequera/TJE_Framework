#include "animationManager.h"
#include "game.h"
#include "our_utils.h"

AnimationManager::AnimationManager()
{
	cur_state = 0;
	target_state = -9999;
    delayed_target_state = -9999;

    cur_time = 0.f;
    target_time = 0.f;

	transition_counter = 0.f;
	transition_time = 0.f;
    delayed_transition_time = 0.f;
    time_to_start = 0.f;

    zombie_idle_paths.resize(NUM_ZOMBIE_IDLES);
    zombie_idle_paths[0] = "data/characters/animations/zombie/idle_1.skanim";
    zombie_idle_paths[1] = "data/characters/animations/zombie/idle_2.skanim";
    zombie_idle_paths[2] = "data/characters/animations/zombie/idle_3.skanim";
}

void AnimationManager::fillCommonAnimations() {
    addAnimationState("data/characters/animations/punch.skanim", PUNCH);
    addAnimationState("data/characters/animations/bat.skanim", BAT_SWING);
    addAnimationState("data/characters/animations/stab.skanim", STAB);
    addAnimationState("data/characters/animations/shoot.skanim", SHOOT);
    addAnimationState("data/characters/animations/dying.skanim", DYING);
}

void AnimationManager::fillPlayerAnimations()
{
    addAnimationState("data/characters/animations/player/idle.skanim", IDLE);
    addAnimationState("data/characters/animations/player/idle_fists.skanim", PLAYER_FISTS_IDLE);
    addAnimationState("data/characters/animations/player/idle_bat.skanim", PLAYER_BAT_IDLE);
    addAnimationState("data/characters/animations/player/idle_knife.skanim", PLAYER_KNIFE_IDLE);
    addAnimationState("data/characters/animations/player/idle_gun.skanim", PLAYER_GUN_IDLE);
    addAnimationState("data/characters/animations/player/defend.skanim", PLAYER_DEFEND);
    addAnimationState("data/characters/animations/hit.skanim", PLAYER_HURT);
    
    fillCommonAnimations();

    cur_state = IDLE;
    //addAnimationState("data/characters/animaciones/player/", PLAYER_PUNCH);
}

void AnimationManager::fillZombieAnimations(int idle_anim)
{
    addAnimationState(zombie_idle_paths[idle_anim].c_str(), IDLE);
    addAnimationState("data/characters/animations/zombie/hit.skanim", ZOMBIE_HURT);
    addAnimationState("data/characters/animations/zombie/hit_grave.skanim", ZOMBIE_HURT_GRAVE);
    addAnimationState("data/characters/animations/zombie/dodge.skanim", ZOMBIE_DODGE);

    fillCommonAnimations();

    cur_state = IDLE;
}

void AnimationManager::addAnimationState(const char* path, int state) {
    states[state] = Animation::Get(path);
}

float AnimationManager::goToState(int state, float time)
{
    float anim_duration = states[state]->duration;

    if (time == 0.f)
    {
        cur_state = state;
        return 0.0;
    }

    if (target_state == state)
        return 0.0;

    target_state = state;
    transition_time = time;

    if (state < IDLE)
        goToStateDelayed(IDLE, anim_duration + time, time);

    return anim_duration;
}


float AnimationManager::goToStateDelayed(int state, float to_start, float time) {

    time_to_start = to_start;
    delayed_target_state = state;
    delayed_transition_time = time;

    return states[state]->duration + to_start;
}

void AnimationManager::update(float dt)
{
    //float time = Game::instance->time;
    cur_time += dt;

    states[cur_state]->assignTime(cur_time);

    if (shouldTrigger(time_to_start, dt)) {
        goToState(delayed_target_state, delayed_transition_time);
        return;
    }

    if (target_state != -9999)
    {
        states[target_state]->assignTime(target_time);

        target_time += dt;
        transition_counter += dt;

        if (transition_counter >= transition_time) {
            cur_state = target_state;

            target_state = -9999;
            transition_counter = 0.f;

            cur_time = target_time;
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
    if (target_state != -9999)
        return blended_skeleton;

    return states[cur_state]->skeleton;
}

bool AnimationManager::isIdle() {
    return cur_state >= IDLE && (target_state == -9999 || target_state >= IDLE);
}