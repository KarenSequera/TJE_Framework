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

void Player::useConsumable(consumableType consumable)
{
}
