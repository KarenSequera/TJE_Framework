#pragma once
#include "player.h"
#define NUM_ZOMBIE_TYPES 4

class ItemEntity : public EntityCollision {
public:

	itemType item_type;
	weaponType weapon_type;
	consumableType consumable_type;
	defensiveType defensive_type; 

	ItemEntity(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, itemType i_type, int subtype);

};

struct EntitySpawner {
	itemType type;
	affectingStat affecting_stat;
	Matrix44 model;
};

enum zombieType { STANDARD, THUG, POLICEMAN, SOLDIER, NOTHING = -1 };

struct zombieInfo
{
	int max_health;
	int health;
	int dmg;
	weaponType weapon;
	weaponType weakness;
	weaponType invulnerable_to;
	std::string texture_path;
};

void parseZombieInfo(const char* filename, zombieInfo* z_info);

class ZombieEntity : public AnimatedEntity {
public:

	zombieType type;
	zombieInfo info;

	ZombieEntity(zombieType z_type, zombieInfo z_info, Matrix44 model, int idle_anim);

	int getMultiplier(weaponType weapon);
	bool alive();
};
