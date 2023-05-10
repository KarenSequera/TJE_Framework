#pragma once
#include "player.h"
#include "entity.h"

class World {
public:
	
	static World* world_instance;
	Player* player;

	Entity* day_root;
	std::vector<Entity*> day_entities;

	Entity* night_root;
	std::vector<Entity*> night_entities;


	int consumable_stats[NUM_CONSUMABLES];
	int weapon_dmg[NUM_WEAPONS];
	int weapon_use_pts[NUM_WEAPONS];
	int defensive_stats[NUM_DEF];

	World();
	
	void useConsumable(consumableType consumable, affectingStat stat);
	
};
