#pragma once

#include "utils.h"

#define DEBUG 1

#define NUM_WEAPONS 4
#define NUM_CONSUMABLES 8
#define NUM_SHILED_ITEMS 2
#define NUM_DEF 2
#define MAX_HUNGER 100
#define MAX_HEALTH 100
#define MAX_SHIELD 75

enum itemType { WEAPON, DEFENSIVE, CONSUMABLE };
enum weaponType { FISTS, BAT, KNIFE, GUN };
enum consumableType { BURGER, CANNED_BEANS, APPLE, AID_KIT, PAINKILLER, BANDAGES, VEST, HELMET };
enum defensiveType { WOODEN_DOOR, METAL_SHIELD };
enum affectingStat { HUNGER, HEALTH, SHIELD };

// all classes related to stages
class Player {
public:
	
	Vector3 position;

	//Inicialisating the statistics
	int health;
	int shield;
	int hunger;
	
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
};
