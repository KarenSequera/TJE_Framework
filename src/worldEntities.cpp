#include "worldEntities.h"

ItemEntity::ItemEntity(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, itemType i_type, int subtype) :
	EntityCollision(in_mesh, in_texture, in_shader, true, false, false)
{
	item_type = i_type;
	switch (item_type) {
		case  WEAPON:
			weapon_type = weaponType(subtype);
			break;

		case  DEFENSIVE:
			defensive_type = defensiveType(subtype);
			break;

		case  CONSUMABLE:
			consumable_type = consumableType(subtype);
			break;
	}
};