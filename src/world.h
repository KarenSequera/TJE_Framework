#pragma once

#include "worldEntities.h"

#define MAX_ITEM_DIST 1000

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};


class World {
public:
	
	static World* inst;
	Player* player;

	// Day variables
	Entity* day_root;
	std::vector<Entity*> day_entities;
	std::vector<EntitySpawner*> item_spawns;
	std::vector<WorldItem*> world_items;

	//arrays containing the probabilities
	float weapon_probabilities[NUM_WEAPONS] = { 0.25, 0.25 ,0.25, 0.25 };
	float consumable_probabilities[NUM_CONSUMABLES] = {0.125,0.125 ,0.125 ,0.125 ,0.125 ,0.125 ,0.125 ,0.125};
	float defensive_probabilities[NUM_DEF] = {0.3333,0.333,0.333};


	//Night variables 
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
	bool parseSpawns(const char* filename);

	// General logic
	void hurtPlayer(weaponType weapon);
	void consumeHunger(int quant);

	int getConsumableQuant(consumableType consumable);
	int useConsumable(consumableType consumable);
	void spawnerInit();

	// Day logic
	void getConsumable(consumableType consumable);
	void getWeaponUses(weaponType weapon);
	void getDefItemUses(defensiveType def);
	void getItem(ItemEntity* item);

	bool checkItemCollisions(const Vector3& ray_dir);
	int checkPlayerCollisions(const Vector3& target_pos, std::vector<sCollisionData>* collisions);

};
