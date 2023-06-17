#include "animationManager.h"
#include "game.h"

AnimationManager::AnimationManager()
{
	cur_state = 0;
	target_state = -1;
    
    /*cur_time = 0.f;
    target_time = 0.f;*/

	transition_counter = 0.f;
	transition_time = 0.f;


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
}

void AnimationManager::fillPlayerAnimations()
{
    addAnimationState("data/characters/animations/player/idle.skanim", PLAYER_IDLE);
    addAnimationState("data/characters/animations/player/idle_bat.skanim", PLAYER_BAT_IDLE);
    addAnimationState("data/characters/animations/player/idle_knife.skanim", PLAYER_KNIFE_IDLE);
    addAnimationState("data/characters/animations/player/idle_gun.skanim", PLAYER_GUN_IDLE);
    
    cur_state = PLAYER_IDLE;
    //addAnimationState("data/characters/animaciones/player/", PLAYER_PUNCH);
}

void AnimationManager::fillZombieAnimations(int idle_anim)
{
    addAnimationState(zombie_idle_paths[idle_anim].c_str(), ZOMBIE_IDLE);
    addAnimationState("data/characters/animations/zombie/hit.skanim", ZOMBIE_HURT);
    addAnimationState("data/characters/animations/zombie/hit_grave.skanim", ZOMBIE_HURT_GRAVE);

    cur_state = ZOMBIE_IDLE;

    //cur_time = dist(gen);
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

    states[cur_state]->assignTime(time); // cur_time);
    //cur_time += dt;

    if (target_state != -1)
    {
        states[target_state]->assignTime(time); // target_time);

        //target_time += dt;
        transition_counter += dt;

        if (transition_counter >= transition_time) {
            cur_state = target_state;

            target_state = -1;
            transition_counter = 0.f;

            //cur_time = target_time;
            //target_time = 0.f;

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