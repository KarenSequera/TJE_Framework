#include "player.h"

Player::Player() {
	health = MAX_HEALTH;
	shield = 0;
	hunger = MAX_HUNGER;

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
	printf("%d %d\n", type, weapon_uses[type]);
	#endif
}

//	Adds defensive item uses
void Player::addDefUses(defensiveType type, int uses)
{
	def_uses[type] += uses;
	#if DEBUG
	printf("%d %d\n", type, def_uses[type]);
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
		if (add && hunger == MAX_HUNGER)
			return false;

		hunger = clamp(hunger + mult * amount, 0, MAX_HUNGER);
		break;
	case HEALTH:
		if (add && health == MAX_HEALTH)
			return false;

		health = clamp(health + mult * amount, 0, MAX_HEALTH);
		break;
	case SHIELD:
		if (add && shield == MAX_SHIELD)
			return false;

		shield = clamp(shield + mult * amount, 0, MAX_SHIELD);
		break;
	}
	return true;
}