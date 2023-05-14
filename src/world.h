#pragma once


#include "entity.h"
#include "player.h"

class World {
public:
	
	static World* inst;
	Player* player;

	Entity* day_root;

	std::vector<Entity*> day_entities;

	Entity* night_root;
	std::vector<Entity*> night_entities;


	int consumable_stats[NUM_CONSUMABLES];
	int weapon_dmg[NUM_WEAPONS];
	int weapon_use_pts[NUM_WEAPONS];
	int defensive_stats[NUM_DEF];
	int defensive_use_pts[NUM_DEF];

	World();
	
	// function to parse stats
	void parseStats(const char* filename);
	bool parseScene(const char* filename);

	void hurtPlayer(weaponType weapon);
	void consumeHunger(int quant);

	int useConsumable(consumableType consumable);
	void getConsumable(consumableType consumable);

	void getItem();
	void getWeapon(weaponType weapon);
	void getDefItem(defensiveType def);

	int getConsumableQuant(consumableType consumable);
};
