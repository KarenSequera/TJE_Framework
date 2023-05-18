#pragma once

#include "player.h"
#define MAX_ITEM_DIST 1000

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};


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

	// General logic
	void hurtPlayer(weaponType weapon);
	void consumeHunger(int quant);

	int getConsumableQuant(consumableType consumable);
	int useConsumable(consumableType consumable);

	// Day logic
	void getConsumable(consumableType consumable);
	void getWeaponUses(weaponType weapon);
	void getDefItemUses(defensiveType def);
	void getItem(const Vector3& ray);

	bool checkItemCollisions(const Vector3& ray_dir);
	bool checkPlayerCollisions(const Vector3& target_pos, std::vector<sCollisionData>* collisions);

};
