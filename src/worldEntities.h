#pragma once
#include "player.h"

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


