#pragma once

#include "utils.h"
#include "entity.h"

#define DEBUG 1

#define NUM_ITEMS 3
#define NUM_WEAPONS 4
#define NUM_CONSUMABLES 6
#define NUM_CONS_PER_TYPE 3
#define NUM_DEF 3
#define MAX_HUNGER 100
#define MAX_HEALTH 200

enum itemType { WEAPON, DEFENSIVE, CONSUMABLE};
enum weaponType { FISTS, BAT, KNIFE, GUN};
enum consumableType { BANDAGES, PAINKILLER, AID_KIT, APPLE, CANNED_BEANS, BURGER};
enum defensiveType { ARMS, WOODEN_DOOR, METAL_SHIELD};
enum affectingStat { HEALTH, HUNGER };

// all classes related to stages
class Player : public AnimatedEntity{
public:
	
	Vector3 position;
	Vector3 velocity;

	//Inicialisating the statistics
	int health;
	int hunger;
	int mitigates;
	int defensive;
	float til_def_broken;
	bool def_broken;

	//array containing the uses 
	int weapon_uses[NUM_WEAPONS];

	//array containinng the inventory of consumables 
	int consumables[NUM_CONSUMABLES];

	//array containing the inventory of defensive items
	int def_uses[NUM_DEF];

	Player();
	void addWeaponUses(weaponType type, int uses);
	void addDefUses(defensiveType type, int uses);
	bool affectPlayerStat(affectingStat stat, int amount, bool add);
	void hurtAnimation(float delay);
	bool hasWeapon();
	void updateAnim(float dt);
};
