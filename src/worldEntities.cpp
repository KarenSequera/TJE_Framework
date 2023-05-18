#include "worldEntities.h"

ItemEntity::ItemEntity(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, itemType stat, int type) :
	EntityCollision(in_mesh, in_texture, in_shader, true, false, false)
{
	switch (stat) {
		case  WEAPON:
			weapon_type = weaponType(type);
			break;

		case  DEFENSIVE:
			defensive_type = defensiveType(type);
			break;

		case  CONSUMABLE:
			consumable_type = consumableType(type);
			break;
	}
};