#pragma once
#include "player.h"

class World {
public:
	
	Player player;

	int consumable_stats[NUM_CONSUMABLES];
	int weapon_dmg[NUM_WEAPONS];
	int weapon_use_pts[NUM_WEAPONS];
	int defensive_stats[NUM_DEF];

	World();

	

};
