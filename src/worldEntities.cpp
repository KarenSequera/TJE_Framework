#include "worldEntities.h"
#include "world.h"
#include <fstream>

ItemEntity::ItemEntity(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, itemType i_type, int subtype) :
	EntityCollision(in_mesh, in_texture, in_shader, true, false, false)
{
	item_type = i_type;
	switch (item_type) {
		case  WEAPON:
			weapon_type = weaponType(subtype);
			defensive_type = defensiveType(0);
			consumable_type = consumableType(0);
			break;

		case  DEFENSIVE:
			weapon_type = weaponType(0);
			defensive_type = defensiveType(subtype);
			consumable_type = consumableType(0);
			break;

		case  CONSUMABLE:
			weapon_type = weaponType(0);
			consumable_type = consumableType(subtype);
			defensive_type = defensiveType(0);
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

		z_info[z_type].max_health = std::stoi(tokens[1]);
		z_info[z_type].health = std::stoi(tokens[1]);
		z_info[z_type].dmg = std::stoi(tokens[2]);
		z_info[z_type].weapon = weaponType(std::stoi(tokens[3]));
		z_info[z_type].weakness = weaponType(std::stoi(tokens[4]));
		z_info[z_type].invulnerable_to = weaponType(std::stoi(tokens[5]));
		z_info[z_type].texture_path = tokens[6];
	}
}

// ZombieEntity------------------------------------------------------------------------------------------------------------------------------
ZombieEntity::ZombieEntity(zombieType z_type, zombieInfo z_info, Matrix44 model, int idle_anim)
{
	mesh = Mesh::Get("data/characters/character.MESH");
	shader = Shader::Get("data/shaders/skinning.vs", "data/shaders/texture.fs");
	type = z_type;
	model_matrix = model;
	info = z_info;
	texture = Texture::Get(z_info.texture_path.c_str());
	
	anim_manager = new AnimationManager();

	anim_manager->fillZombieAnimations(idle_anim);
	time_til_death = 0.f;
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

