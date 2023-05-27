#include "world.h"
#include "utils.h"
#include "our_utils.h"

//TODO: REMOVE THIS IF UNNECESSARY
#include "game.h"

#include <fstream>
#include <map>
#include <iostream>
#include <random>

World* World::inst = NULL;

World::World() {
	inst = this;

	parseStats("data/items/info/stats.txt");

	player = new Player();
	
	day_root = new Entity();
	day_entities.push_back(day_root);

	night_root = new Entity();
	night_entities.push_back(night_root);

	selected_option = 0;

	Entity* entity;
	Shader* shader = Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs");

	parseScene("data/myscene.scene");
	parseSpawns("data/spawner.scene");
	parseItemEntities("data/items/info/items.txt");
	parseZombieInfo("data/zombies/zombie_info.txt", z_info);

	createMenus("data/menus/menus.txt");
	changeMenu("general");
}

// Parsing --------------------------------------------------------------------------------------------------------------------------------------------------
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

void World::parseItemEntities(const char* filename)
{
	int i;
	for (i = 0; i < NUM_ITEMS; i++)
	{
		std::vector<ItemEntity*> item;
		items.push_back(item);
	}

	int arrs[] = { WEAPON, DEFENSIVE, CONSUMABLE };
	int sizes[] = { NUM_WEAPONS, NUM_DEF, NUM_CONSUMABLES };

	
	std::ifstream file(filename);
	if (!file.good()) {
		std::cerr << "World [ERROR]" << " Stats file not found!" << std::endl;
		exit(-1);
	}

	std::string data;
	for (auto item_type : arrs)
	{
		for (int subtype = 0; subtype < sizes[item_type]; subtype++)
		{
			file >> data;
			std::vector<std::string> tokens = tokenize(data, ",");

			ItemEntity* item = new ItemEntity(
				Mesh::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				Shader::Get("data/shaders/instanced.vs", "data/shaders/texture.fs"),
				itemType(item_type),
				subtype);

			items[item_type].push_back(item);
			day_root->addChild(item);
		}
		file.ignore(1, '\n');
	}

}

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

std::map<std::string, sRenderData> meshes_to_load;

//	Parses a scene from a .scene file
void World::parseScene(const char* filename)
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	// meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };

	std::cout << " + Scene loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Scene [ERROR]" << " File not found!" << std::endl;
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
}

// behaviour related ------------------------------------------------------------------------------------------------------------------------------------------------------------------

// GENERAL  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Hurts the player according to a specific weapon type
void World::hurtPlayer(weaponType weapon)
{
	player->affectPlayerStat(HEALTH, weapon_dmg[weapon], false);
}

bool World::isPlayerAlive() {
	return player->health > 0;
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
void World::getItem(ItemEntity* item) {

	switch (item->item_type) {
		case WEAPON:
			// get type from entity
			getWeaponUses(item->weapon_type);
			break;

		case DEFENSIVE:
			getDefItemUses(item->defensive_type);
			break;

		case CONSUMABLE:
			getConsumable(item->consumable_type);

			break;
	}
}

bool World::checkItemCollisions(const Vector3& ray_dir)
{
	for (auto& entity : day_root->children)
	{
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

/*
* Function that checks the collisions of the player with the object in the scene
* @param target_pos: the position the playe wants to move to
* @param collisions: vector where we will store the collisions
* @returns: an int that indicates the following
* 0: no collisions
* 1: collision with an object that is not an item
* 2: collision with an item (the player collects it)
*/
int World::checkPlayerCollisions(const Vector3& target_pos, std::vector<sCollisionData>* collisions)
{
	Vector3 center = target_pos - Vector3(0.f, 0.5f, 0.f);
	float sphere_rad = 5.f;
	Vector3 colPoint, colNormal;

	for (auto& entity : day_root->children)
	{
		EntityCollision* collision = dynamic_cast<EntityCollision*>(entity);
		 

		if (!collision)
			continue;


		for (auto& model : collision->models)
		{
			if (!collision->mesh->testSphereCollision(model, center, sphere_rad, colPoint, colNormal))
				continue;

			ItemEntity* item = dynamic_cast<ItemEntity*>(collision);
			if (item) {
				getItem(item);

				// We need to erase the model matrix
				//So we find the position of the model of the object in the vector
				auto to_delete = std::find(item->models.begin(), item->models.end(), model);
				item->models.erase(to_delete);

				return 2;
			}
			else
				collisions->push_back({ colPoint, colNormal.length() > 0.01 ? colNormal.normalize() : colNormal});
				
		}
	}
	if (collisions->empty())
		return 0;

	return 1;
}

// spawns related ------------------------------------------------------------------------------------------------------------------------------------------------------------------

void  World::parseSpawns(const char* filename)
{
	//Hash map to map the spawns
	std::unordered_map<std::string, itemType> myHashMap;
	myHashMap["spawn/weapon.obj"] = WEAPON;
	myHashMap["spawn/defensive.obj"] = DEFENSIVE;
	myHashMap["spawn/consumable.obj"] = CONSUMABLE;
	
	std::cout << " + Spawns loading: " << filename << "..." << std::endl;

	std::ifstream file(filename);

	if (!file.good()) {
		std::cerr << "Spawns [ERROR]" << " File not found!" << std::endl;
	}

	std::string scene_info, spawn_type, model_data;
	for (int i = 0; i < 2; i++) {
		file >> scene_info;
	}

	int spawn_count = 0;
	// Read file line by line and store mesh path and model info in separated variables
	while (file >> spawn_type >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		EntitySpawner* itemSpawn = new EntitySpawner{ myHashMap[spawn_type], model };
		item_spawns.push_back(itemSpawn);
		spawn_count++;
	}

	std::cout << "Spawns [OK]" << " Spawns added: " << spawn_count << std::endl;
}


// Function that selects an object based on the provided probabilities
int selectObject(const float* probabilities, int numObjects) 
{

	// Obtain a random seed from the hardware
	std::random_device rd;
	// Seed the random number engine
	std::mt19937 eng(rd());

	// Create a discrete distribution based on probabilities
	std::discrete_distribution<int> dist(probabilities, probabilities + numObjects);

	// Generate a random object index based on the probabilities
	return dist(eng);
}

void World::clearItems() 
{
	int arrs[] = { WEAPON, DEFENSIVE, CONSUMABLE };
	int sizes[] = { NUM_WEAPONS, NUM_DEF, NUM_CONSUMABLES };

	for (auto& item_type : arrs)
	{	
		for (int subtype = 0; subtype < sizes[item_type]; subtype++)
		{
				items[item_type][subtype]->models.clear();
		}
	}
	int i;
}

void  World::spawnerInit()
{
	clearItems();

	for (auto spawn : item_spawns) {
		switch (spawn->type) {
			case WEAPON:
				items[WEAPON][selectObject(weapon_probabilities, NUM_WEAPONS)]->models.push_back(spawn->model);
				break;
			case DEFENSIVE:
				items[DEFENSIVE][selectObject(defensive_probabilities, NUM_DEF)]->models.push_back(spawn->model);
				break;
			case  CONSUMABLE:
				items[CONSUMABLE][selectObject(consumable_probabilities, NUM_CONSUMABLES)]->models.push_back(spawn->model);
				break;
		}
	}
}

// NIGHT  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void World::generateZombies(int num_night) 
{

	night_root->children.clear();
	wave.clear();
	ZombieEntity* zombie;
	
	int idx = min(num_night, DIFICULTY_LEVELS-1);
	float probability[NUM_ZOMBIE_TYPES];
	memcpy(probability, zombies_probabilities[idx], sizeof(probability));

	for (int i = 0; i < NUM_ZOMBIES_WAVE; i++) 
	{
		
		zombieType type = zombieType(selectObject(probability, NUM_ZOMBIE_TYPES));

		zombie = new ZombieEntity(type, z_info);

		if (type == STANDARD) {
			zombie->info.weakness = weaponType((std::rand() % 3) + 1);
		}
		night_root->addChild(zombie);
		wave.push_back(zombie);
	}

	zombies_alive = NUM_ZOMBIES_WAVE;

};


int World::hurtZombie(int zombie_idx)
{
	ZombieEntity* zombie = wave[zombie_idx];
	int multiplier = zombie->getMultiplier(weapon);
	zombie->info.health -= weapon_dmg[weapon] * multiplier;


	if (!zombie->alive())
		killZombie(zombie_idx);
	
	return multiplier;
}

void World::killZombie(int zombie_idx)
{
	if (zombie_idx >= 0 && zombie_idx < zombies_alive )
	{
		printf("|| zombie killed \n");
		wave.erase(std::next(wave.begin(), zombie_idx));
		zombies_alive--;
	}
}

// MENU RELATED ---------------------------------------------------------------------------------------
void World::changeMenu(std::string go_to)
{
	cur_menu = menus[go_to];
	selected_option = 0;
}

void World::changeOption(int to_add)
{
	int start = cur_menu->start_visible;
	int end = cur_menu->end_visible;
	int num_options = cur_menu->options.size();

	// Change the visible options, sliding the window accordingly
	selected_option = ourMod(selected_option + to_add, num_options);

	if (num_options == 3)
		return;

	if (selected_option == 0)
	{
		cur_menu->start_visible = 0;
		cur_menu->end_visible = 2;
	}
	else if (selected_option == num_options - 1)
	{
		cur_menu->start_visible = selected_option - 2;
		cur_menu->end_visible = selected_option;
	}
	else if (selected_option > end) {
		cur_menu->start_visible++;
		cur_menu->end_visible++;
	}
	else if (selected_option < start)
	{
		cur_menu->start_visible--;
		cur_menu->end_visible--;
	}
	
}

bool World::selectOption()
{
	return cur_menu->onSelect(selected_option);
}

void World::createMenus(std::string filename)
{
	Menu* general = new Menu();
	menus["general"] = general;

	std::ifstream file(filename);
	if (!file.good()) {
		std::cerr << "World [ERROR]" << " Stats file not found!" << std::endl;
		exit(-1);
	}

	std::string data;
	for (int option = 0; option < NUM_GENERAL_OPTIONS; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		general->options.push_back(
			new GeneralMenuEntity(
				Texture::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				tokens[3]
			)
		);
	}

	file.ignore(1, '\n');

	//Consumable menu 
	Menu* consumables = new Menu();
	menus["consumables"] = consumables;

	for (int option = 0; option < NUM_CONSUMABLES; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		consumables->options.push_back(
			new ConsumableMenuEntity(
				Texture::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				consumableType(std::stoi(tokens[3]))
			)
		);
	}

	file.ignore(1, '\n');

	//Weapon menu 
	Menu* weapon = new Menu();
	menus["weapon"] = weapon;

	for (int option = 0; option < NUM_WEAPONS; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		weapon->options.push_back(
			new WeaponMenuEntity(
				Texture::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				weaponType(std::stoi(tokens[3]))
			)
		);
	}

	file.ignore(1, '\n');

	//Defend menu
	Menu* defensive = new Menu();
	menus["defensive"] = defensive;

	for (int option = 0; option < NUM_DEF; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		defensive->options.push_back(
			new DefensiveMenuEntity(
				Texture::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				defensiveType(std::stoi(tokens[3]))
			)
		);
	}
}
