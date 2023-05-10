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
		weapon_uses[i];
	}

	for (i = 0; i < NUM_CONSUMABLES; ++i) {
		consumables[i] = 0;
	}

}

void Player::addWeaponUses(weaponType type, int uses)
{
	weapon_uses[type] += uses;
}

void Player::useConsumable(consumableType consumable)
{
	consumables[consumable]--;
}

void Player::affectPlayerStat(affectingStat stat, int amount, bool add)
{
	int mult = add ? 1 : -1;
	switch (stat) {
	case HEALTH:
		health = clamp(health + mult * amount, 0, MAX_HEALTH);
		break;
	case SHIELD:
		shield = clamp(health + mult * amount, 0, MAX_SHIELD);
		break;
	case HUNGER:
		hunger = clamp(health + mult * amount, 0, MAX_HUNGER);
		break;
	}
}