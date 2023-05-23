#include "worldEntities.h"
#include <fstream>

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

void parseZombieInfo(const char* filename, zombieInfo* z_info)
{

	std::ifstream file(filename);
	if (!file.good()) {
		std::cerr << "World [ERROR]" << " Stats file not found!" << std::endl;
		exit(-1);
	}

	std::string data;
	for (int z_type = 0; z_type < NUM_ZOMBIE_TYPES; z_type++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		z_info[z_type].health = std::stoi(tokens[1]);
		z_info[z_type].weapon = weaponType(std::stoi(tokens[2]));
		z_info[z_type].weakness = weaponType(std::stoi(tokens[3]));
		z_info[z_type].invulnerable_to = weaponType(std::stoi(tokens[3]));
		z_info[z_type].texture_path = tokens[5];
	}
}

// ZombieEntity------------------------------------------------------------------------------------------------------------------------------
ZombieEntity::ZombieEntity(zombieType z_type, zombieInfo* z_info)
{
	mesh = Mesh::Get("data/zombies/zombie.obj");
	shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");
	type = z_type;
	selected = false;

	info = z_info[z_type];
	texture = Texture::Get(z_info->texture_path.c_str());

}

int ZombieEntity::getMultiplier(weaponType weapon) 
{
	if (weapon == info.invulnerable_to)
	{
		std::cout << "The zombie is invulnerable, better luck next time (if u survive ;) )";
		return 0;
	}
	else if (weapon == info.weakness)
	{
		std::cout << "THAT was super effective, DO IT AGAIN!";
		return 2;
	}
	else
	{
		std::cout << "Kinda dull...  The zombie took some damage tho";
		return 1;
	}
}

bool ZombieEntity::alive()
{
	return (info.health > 0);
} 