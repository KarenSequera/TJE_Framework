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
	Matrix44 model;
};

enum zombieType { STANDARD, THUG, POLICEMAN, SOLDIER, NOTHING = -1 };

struct zombieInfo
{
	int health;
	weaponType weapon;
	weaponType weakness;
	weaponType invulnerable_to;
	std::string texture_path;
};

void parse_zombie_info(const char* filename, zombieInfo* z_info);

class ZombieEntity : public EntityMesh {
public:

	zombieType type;
	zombieInfo info;
	bool selected;
	

	ZombieEntity(zombieType z_type, zombieInfo* z_info);

	
	bool zombie_alive();
};
