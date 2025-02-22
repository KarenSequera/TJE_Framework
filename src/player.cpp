#include "player.h"
#include "our_utils.h"
#include "audio.h"

Player::Player() {
	health = MAX_HEALTH;
	shield = 0;
	hunger = MAX_HUNGER;
	mitigates = 0;
	defensive = 0;
	til_def_broken = 0.f;
	def_broken = false;
	// Render related
	mesh = Mesh::Get("data/characters/character.MESH");
	texture = Texture::Get("data/characters/player.tga");
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");

	anim_manager = new AnimationManager();
	anim_manager->fillPlayerAnimations();

	//All the inventory is set to zero
	int i;
	for (i = 0; i < NUM_DEF; ++i) {
		def_uses[i] = 0;
	}

	for (i = 0; i < NUM_WEAPONS; ++i) {
		weapon_uses[i] = 0;
	}

	for (i = 0; i < NUM_CONSUMABLES; ++i) {
		consumables[i] = 0;
	}
}

//	Adds weapon uses
void Player::addWeaponUses(weaponType type, int uses)
{
	weapon_uses[type] += uses;
	#if DEBUG
	printf("weapon %d %d\n", type, weapon_uses[type]);
	#endif
}

//	Adds defensive item uses
void Player::addDefUses(defensiveType type, int uses)
{
	def_uses[type] += uses;
	#if DEBUG
	printf("defensive %d %d\n", type, def_uses[type]);
	#endif
}

/*
* Tries to affect one of the player's stats.
* @param stat: the stat we want to modify
* @param amount: the amount we want the stat to change
* @param add: whether we are adding or subtracting
* @return: a bool determining whether the change could be made (true) or not (false)
* return is used to give appropriate error messages
*/
bool Player::affectPlayerStat(affectingStat stat, int amount, bool add)
{
	int mult = add ? 1 : -1;
	switch (stat) {
	case HUNGER:
		if (add) {
			if (hunger == MAX_HUNGER)
				return false;
			
			Audio::Play("data/audio/hunger_cons.wav", 1.f, false);
		}
		hunger = clamp(hunger + mult * amount, 0, MAX_HUNGER);
		break;
	case HEALTH:
		if (add) {
			if (health == MAX_HEALTH)
				return false;
			
			Audio::Play("data/audio/health_cons.wav", 1.f, false);
		}
		health = clamp(health + mult * amount, 0, MAX_HEALTH);
		break;
	case SHIELD:
		if (add) {
			if(health == MAX_SHIELD)
				return false;
			
			Audio::Play("data/audio/shield_cons.wav", 1.f, false);
		}

		shield = clamp(shield + mult * amount, 0, MAX_SHIELD);
		break;
	}
	return true;
}

void Player::hurtAnimation(float delay)
{
	toStateDelayed(PLAYER_HURT, delay, TRANSITION_TIME);
	if(def_broken)
		til_def_broken = delay;
}

bool Player::hasWeapon()
{
	return ((anim_manager->cur_state <= SHOOT && anim_manager->cur_state >= BAT_SWING)
		|| (anim_manager->target_state <= SHOOT && anim_manager->target_state >= BAT_SWING));
}

void Player::updateAnim(float dt) {
	AnimatedEntity::updateAnim(dt);
	if (shouldTrigger(til_def_broken, dt)) {
		if(defensive)
			Audio::Play("data/audio/night/crack_shield.wav", 1.f, false);
		defensive = 0;
	}
}