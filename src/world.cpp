#include "world.h"
#include "utils.h"

#include <fstream>
#include <map>

World* World::inst = NULL;

World::World() {
	//TODO: maybe parse the stats from files.
	inst = this;

	parseStats("data/stats.txt");

	player = new Player();
	
	day_root = new Entity();
	day_entities.push_back(day_root);

	Entity* entity;
	Shader* shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");

	parseScene("data/myscene.scene");
}

//	Parses the stats from a specific file
void World::parseStats(const char* filename) {
	std::ifstream file(filename);
	if (!file.good()) {
		std::cerr << "World [ERROR]" << " Stats file not found!" << std::endl;
		exit(-1);
	}
	std::string dump, data;
	int sizes[] = {NUM_CONSUMABLES, NUM_WEAPONS, NUM_WEAPONS, NUM_DEF, NUM_DEF };
	int* array_ptr[] = { consumable_stats , weapon_dmg , weapon_use_pts, defensive_stats, defensive_use_pts };
	int line = 0;

	file >> dump;
	while (file >> data && line < 5)
	{
		// Get all information.
		std::vector<std::string> tokens = tokenize(data, ",");
		for (int t = 0; t < sizes[line]; ++t) {
			array_ptr[line][t] = (int)atoi(tokens[t + 1].c_str());
		}
		line++;
		file >> dump;
		file.ignore(1, '\n');
	}

}

// render related --------------------------------------------------------------------------------------------------------------------------------------------------
struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

std::map<std::string, sRenderData> meshes_to_load;

//	Parses a scene from a .scene file
bool World::parseScene(const char* filename)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	// meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };

	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
		return false;
	}

	std::string scene_info, mesh_name, model_data;
	for (int i = 0; i < 2; i++) {
		file >> scene_info;
	}

	int mesh_count = 0;

	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		// Add model to mesh list (might be instanced!)
		sRenderData& render_data = meshes_to_load[mesh_name];
		render_data.models.push_back(model);
		mesh_count++;
	}

	// Iterate through meshes loaded and create corresponding entities
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;

		// Create instanced entity
		if (render_data.models.size() > 1) {
			EntityCollision* new_entity = new EntityCollision(Mesh::Get(mesh_name.c_str()),
				Texture::Get("data/texture.tga"), Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs"), true, false, false);

			// Add all instances
			new_entity->models = render_data.models;
			// Add entity to scene root
			day_root->addChild(new_entity);
		}
		else{
			// Create normal entity
			EntityCollision* new_entity = new EntityCollision(Mesh::Get(mesh_name.c_str()),
			Texture::Get("data/texture.tga"), Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs"), false, false, false);

			new_entity->model_matrix = render_data.models[0];
			new_entity->models.push_back(render_data.models[0]);

			// Add entity to scene root
			day_root->addChild(new_entity);
		}
		
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}

// behaviour related ------------------------------------------------------------------------------------------------------------------------------------------------------------------

// GENERAL  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Hurts the player according to a specific weapon type
void World::hurtPlayer(weaponType weapon)
{
	player->affectPlayerStat(HEALTH, weapon_dmg[weapon], false);
}

//	Consumes a specific hunger from the user
void World::consumeHunger(int quant)
{
	player->affectPlayerStat(HUNGER, quant, false);
}

/* 
	Function that tries to use a specific consumable.
	@param consumable: type of consumable we want to use.
	@return: an int indicating whether the consumable could be used or not
	0 if the consumable was used without issue
	1 if there are no consumables of that type
	2 if the stat the consumable affects is already at the maximum
*/

//	Returns the number of consumables we have of a specific type
int World::getConsumableQuant(consumableType consumable)
{
	return player->consumables[consumable];
}

int World::useConsumable(consumableType consumable)
{
	if (player->consumables[consumable])
	{
		int to_add = consumable_stats[consumable];
		if(!player->affectPlayerStat(affectingStat(consumable / 3), to_add, true)) return 2;
		player->consumables[consumable]--;
		return 0;
	}
	else return 1;

}


// DAY  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Function that adds one consumable of a specific type to the player's inventory
void World::getConsumable(consumableType consumable)
{
	// If the consumable is a shield, and it cat be added directly to the player's stats, do nothing
	if (consumable / 3 == SHIELD && player->affectPlayerStat(SHIELD, consumable_stats[consumable], true)) {}
	// otherwise add one consumable of that type
	else player->consumables[consumable]++;

}

//	Adds weapon uses of a specific type
void World::getWeaponUses(weaponType weapon) {
	player->addWeaponUses(weapon, weapon_use_pts[weapon]);
}

//	Adds uses to a specific type of defensive items
void World::getDefItemUses(defensiveType def) {
	player->addDefUses(def, defensive_use_pts[def]);
}

//	Tries to get an item from the world and add it to the player's inventory
void World::getItem(const Vector3& ray) {
	if (!checkItemCollisions(ray))
		return;

	// if collided with item mesh:
	itemType type = rand() % 2 == 0 ? WEAPON : DEFENSIVE;
	#if DEBUG
	printf("%s: ", type ? "def" : "weapon");
	#endif
	weaponType weapon_type = KNIFE;
	defensiveType def_type = WOODEN_DOOR;
	consumableType consumable_type = AID_KIT;


	switch (type) {
		case WEAPON:
			// get type from entity
			getWeaponUses(weapon_type);
			break;

		case DEFENSIVE:
			getDefItemUses(def_type);
			break;

		case CONSUMABLE:
			getConsumable(consumable_type);
			break;
	}
}

bool World::checkItemCollisions(const Vector3& ray_dir)
{
	for (auto& entity : day_root->children)
	{
		//TODO: uncomment when itemEntity is implemented
		/*
		* ItemEntity* item = dynamic_cast< ItemEntity* > (entity);
		* if(item)
		* {
		*	//replace by the code in "if collision"
		* }
		*/
		EntityCollision* collision = dynamic_cast<EntityCollision*>(entity);
		if (!collision)
			continue;

		for (auto& model : collision->models)
		{
			if (!collision->mesh->testRayCollision(
				model,
				player->position,
				ray_dir,
				Vector3(),
				Vector3(),
				MAX_ITEM_DIST,
				false
			))
			#if DEBUG
			{

				printf("NO collision\n");
				continue;
			}
			printf("Collided with item!\n");
			#else
				continue;
			#endif
			return true;
		}
		
	}
	return false;
}

// NIGHT  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------