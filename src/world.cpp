#include "world.h"
#include "utils.h"
#include "our_utils.h"

#include "game.h"
#include "audio.h"

#include <filesystem>
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

	selected_option = 0;
	zombie_attacking = false;
	cur_wave = 0;

	parseSceneDay();
	parseSpawns("data/spawner.scene");
	parseItemEntities("data/items/info/items.txt");
	parseZombieInfo("data/characters/zombie_info.txt", z_info);

	createMenus("data/quad_textures/menus/menus.txt");
	changeMenu("general");

	// Cubemap
	loadSky();

	//ParseNight 
	parseSceneNight("data/nightScene.scene");
	pause_menu = new PauseMenu();

	camera2D = new Camera();
	camera2D->view_matrix = Matrix44();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);

	for(int i = 0; i < NUM_OPTIONS; i++)
		option_quads[i] = new Mesh();

	resizeOptions(Game::instance->window_width, Game::instance->window_height);

	ready_to_attack = false;
	unlimited_everything = false;

	idle = true;
	zombie_hurt = 0;
	number_nights = 0;

	//init audio
	Audio::Init();

	getSounds();

	// we should trigger the tutorial when there are no previous runs
	triggerTutorial = !existPreviousRuns();
	frozen = false;

}

// function that returns whether the player has played  the game before, i.e., if there are any previous runs
bool World::existPreviousRuns() {
	std::ifstream file("data/gameover/runs.txt");
	return file.good();
}

void World::getSounds() {
	weapon_sounds.resize(NUM_WEAPONS);
	weapon_sounds[FISTS] = "data/audio/night/fists.wav";
	weapon_sounds[BAT] = "data/audio/night/bat.wav";
	weapon_sounds[KNIFE] = "data/audio/night/knife.wav";
	weapon_sounds[GUN] = "data/audio/night/gun.wav";

	hurt_sounds.resize(NUM_DEF + HURT_SOUNDS);

	for (int i = 0; i < NUM_DEF; i++) {
		hurt_sounds[i] = "data/audio/night/defend" + std::to_string(i + 1) + ".wav";
	}

	for (int i = 0; i < HURT_SOUNDS; i++) {
		hurt_sounds[i + NUM_DEF] = "data/audio/night/hit" + std::to_string(i + 1) + ".wav";
	}
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

	items.resize(NUM_ITEMS);
	weapon_mesh_info.resize(NUM_WEAPONS);
	def_mesh_info.resize(NUM_DEF);
	
	int i;

	int arrs[] = { WEAPON, DEFENSIVE, CONSUMABLE };
	int sizes[] = { NUM_WEAPONS, NUM_DEF, NUM_CONSUMABLES };

	for (i = 0; i < NUM_ITEMS; i++)
	{
		std::vector<ItemEntity*> item_vec;
		item_vec.resize(sizes[i]);
		items[i] = item_vec;
	}


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
				Texture::Get("data/texture.tga"),
				Shader::Get("data/shaders/instanced.vs", "data/shaders/phong.fs"),
				itemType(item_type),
				subtype);

			items[item_type][subtype] = item;
			
			day_root->addChild(item);
		}
		file.ignore(1, '\n');
	}

	file.ignore(1, '\n');
	file >> data;


	weapon_mesh_info[FISTS].mesh = nullptr;

	for (int weapon_type = 1; weapon_type < NUM_WEAPONS; weapon_type++) {
		file >> data;
		sHoldableMeshData weapon_data;

		std::vector<std::string> tokens = tokenize(data, ",");

		weapon_data.mesh = Mesh::Get(tokens[0].c_str());
		weapon_data.player_offset = Vector3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		weapon_data.player_rotate = std::stoi(tokens[4]);
		weapon_data.player_angle = std::stof(tokens[5]);
		weapon_data.player_axis = Vector3(std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]));
		weapon_data.zombie_offset = Vector3(std::stof(tokens[9]), std::stof(tokens[10]), std::stof(tokens[11]));
		weapon_data.zombie_rotate = std::stoi(tokens[12]);
		weapon_data.zombie_angle = std::stof(tokens[13]);
		weapon_data.zombie_axis = Vector3(std::stof(tokens[14]), std::stof(tokens[15]), std::stof(tokens[16]));

		weapon_mesh_info[weapon_type] = weapon_data;
	}

	file.ignore(1, '\n');
	file >> data;
	def_mesh_info[ARMS].mesh = nullptr;

	for (int def_type = 1; def_type < NUM_DEF; def_type++) {
		file >> data;
		sHoldableMeshData def_data;

		std::vector<std::string> tokens = tokenize(data, ",");

		def_data.mesh = Mesh::Get(tokens[0].c_str());
		def_data.player_offset = Vector3(std::stof(tokens[1]), std::stof(tokens[2]), std::stof(tokens[3]));
		def_data.player_rotate = std::stoi(tokens[4]);
		def_data.player_angle = std::stof(tokens[5]);
		def_data.player_axis = Vector3(std::stof(tokens[6]), std::stof(tokens[7]), std::stof(tokens[8]));
		def_data.zombie_offset = Vector3(std::stof(tokens[9]), std::stof(tokens[10]), std::stof(tokens[11]));
		def_data.zombie_rotate = std::stoi(tokens[12]);
		def_data.zombie_angle = std::stof(tokens[13]);
		def_data.zombie_axis = Vector3(std::stof(tokens[14]), std::stof(tokens[15]), std::stof(tokens[16]));

		def_mesh_info[def_type] = def_data;
	}
}

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

std::map<std::string, sRenderData> meshes_to_load;

void World::getMeshesToLoad(const char* filename)
{
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
}

//	Parses a scene from a .scene file
void World::parseSceneDay()
{
	// You could fill the map manually to add shader and texture for each mesh
	// If the mesh is not in the map, you can use the MTL file to render its colors
	// meshes_to_load["meshes/example.obj"] = { Texture::Get("texture.tga"), Shader::Get("shader.vs", "shader.fs") };

	// Iterate through meshes loaded and create corresponding entities
	// We have 2 loops with more or less the same code, but this will save us conditionals in the end

	getMeshesToLoad("data/dayScene.scene");

	std::string mesh_name, model_data;
	EntityMesh* new_entity;

	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;


		if (render_data.models.size() > 1) {
			new_entity = new EntityCollision(Mesh::Get(mesh_name.c_str()),
				Texture::Get("data/texture.tga"), Shader::Get("data/shaders/instanced.vs", "data/shaders/phong.fs"), true, false, false);

			// Add all instances
			new_entity->models = render_data.models;
			// Add entity to scene root
			day_root->addChild(new_entity);
		}
		else {
			// Create normal entity
			new_entity = new EntityCollision(Mesh::Get(mesh_name.c_str()),
				Texture::Get("data/texture.tga"), Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs"), false, false, false);

			new_entity->model_matrix = render_data.models[0];
			new_entity->models.push_back(render_data.models[0]);

			// Add entity to scene root
			day_root->addChild(new_entity);
		}
		
	}

	meshes_to_load.clear();
	getMeshesToLoad("data/dayFloors.scene");
	for (auto data : meshes_to_load) {

		mesh_name = "data/" + data.first;
		sRenderData& render_data = data.second;

		// No transforms, anything to do here
		if (render_data.models.empty())
			continue;

		if (render_data.models.size() > 1) {
			new_entity = new EntityMesh(Mesh::Get(mesh_name.c_str()),
				Texture::Get("data/texture.tga"), Shader::Get("data/shaders/instanced.vs", "data/shaders/phong.fs"), true, false);

			// Add all instances
			new_entity->models = render_data.models;
			// Add entity to scene root
			day_root->addChild(new_entity);
		}
		else {
			// Create normal entity
			new_entity = new EntityMesh(Mesh::Get(mesh_name.c_str()),
				Texture::Get("data/texture.tga"), Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs"), false, false);

			new_entity->model_matrix = render_data.models[0];
			new_entity->models.push_back(render_data.models[0]);

			// Add entity to scene root
			day_root->addChild(new_entity);
		}
	}
}


//	Parses a scene from a .scene file
void World::parseSceneNight(const char* filename)
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

	int idx = 0;
	// Read file line by line and store mesh path and model info in separated variables
	while (file >> mesh_name >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) 
		{
			model.m[t] = (float)atof(tokens[t].c_str());
		}
		night_models[idx] = model;
		idx++;
	}
	player->model_matrix = night_models[2];
}


// behaviour related ------------------------------------------------------------------------------------------------------------------------------------------------------------------

// GENERAL  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
//	Hurts the player for a spefific amount of damage
void World::hurtPlayer(int damage)
{
	int dmg = damage;
	if (player->mitigates) {
		dmg = max(damage - player->mitigates, 0);
		player->mitigates = max(player->mitigates - damage, 0);
		player->def_broken = true;
	}

	int diff = player->shield - dmg;
	player->affectPlayerStat(SHIELD, dmg, false);

	if (diff < 0)
		player->affectPlayerStat(HEALTH, -1 * diff, false);
}

void World::playerDefenseOff()
{
	player->mitigates = 0;
	player->defensive = 0;
	player->toState(IDLE, TRANSITION_TIME);
}

bool World::isPlayerAlive() {
	return player->health > 0;
}

//	Consumes a specific hunger from the user
void World::consumeHunger(int quant)
{
	player->affectPlayerStat(HUNGER, quant, false);
}

//	Returns the number of consumables we have of a specific type
int World::getConsumableQuant(consumableType consumable)
{
	return player->consumables[consumable];
}

int World::getWeaponUses(weaponType weapon)
{
	return player->weapon_uses[weapon];
}

int World::getDefItemUses(defensiveType def)
{
	return player->def_uses[def];
}

/*
	Function that tries to use a specific consumable.
	@param consumable: type of consumable we want to use.
	@return: an int indicating whether the consumable could be used or not
	0 if the consumable was used without issue
	1 if there are no consumables of that type
	2 if the stat the consumable affects is already at the maximum
*/
int World::useConsumable(consumableType consumable)
{
	if (unlimited_everything || player->consumables[consumable])
	{
		int to_add = consumable_stats[consumable];

		if (!player->affectPlayerStat(affectingStat(consumable / 3), to_add, true)) {
			Audio::Play("data/audio/error.wav", 1.f, false);
			return 2;
		}

		if(!unlimited_everything)
			player->consumables[consumable]--;

		return 0;
	}
	else {
		Audio::Play("data/audio/error.wav", 1.f, false);
		return 1;
	}
}

void World::applyShields()
{
	if (player->shield < MAX_SHIELD)
	{
		while (player->consumables[VEST] > 0 && player->shield + consumable_stats[VEST] <= MAX_SHIELD)
			useConsumable(VEST);

		while (player->consumables[HELMET] > 0 && player->shield + consumable_stats[HELMET] <= MAX_SHIELD)
			useConsumable(HELMET);
	}
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
void World::addWeaponUses(weaponType weapon) {
	player->addWeaponUses(weapon, weapon_use_pts[weapon]);
}

//	Adds uses to a specific type of defensive items
void World::addDefItemUses(defensiveType def) {
	player->addDefUses(def, defensive_use_pts[def]);
}

//	Tries to get an item from the world and add it to the player's inventory
void World::getItem(ItemEntity* item) {
	Audio::Play("data/audio/day/get_item.wav", 1.f, false);
	switch (item->item_type) {
		case WEAPON:
			// get type from entity
			addWeaponUses(item->weapon_type);
			break;

		case DEFENSIVE:
			addDefItemUses(item->defensive_type);
			break;

		case CONSUMABLE:
			getConsumable(item->consumable_type);

			break;
	}
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
	Vector3 center = target_pos - Vector3(0.f, 0.0f, 0.f);
	float sphere_rad = 40.f;
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
	int stat_type = 0;
	// Read file line by line and store mesh path and model info in separated variables
	while (file >> spawn_type >> stat_type >> model_data)
	{
		// Get all 16 matrix floats
		std::vector<std::string> tokens = tokenize(model_data, ",");

		// Fill matrix converting chars to floats
		Matrix44 model;
		for (int t = 0; t < tokens.size(); ++t) {
			model.m[t] = (float)atof(tokens[t].c_str());
		}

		EntitySpawner* itemSpawn = new EntitySpawner{ myHashMap[spawn_type], affectingStat(stat_type), model};
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
}

void  World::spawnerInit()
{
	for (auto spawn : item_spawns) {
		switch (spawn->type) {
			case WEAPON:
				items[WEAPON][selectObject(weapon_probabilities, NUM_WEAPONS)]->models.push_back(spawn->model);
				break;
			case DEFENSIVE:
				items[DEFENSIVE][selectObject(defensive_probabilities, NUM_DEF)]->models.push_back(spawn->model);
				break;
			case  CONSUMABLE:
				float probabilities[3];
				int offset = spawn->affecting_stat * NUM_CONS_PER_TYPE;
				for (int i = 0; i < NUM_CONS_PER_TYPE; i++)
				{
					if (offset + i >= NUM_CONSUMABLES)
						probabilities[i] = 0.0;
					else
						probabilities[i] = consumable_probabilities[offset + i];
				}
				int selected = selectObject(probabilities, NUM_CONS_PER_TYPE);
				items[CONSUMABLE][selected + offset]->models.push_back(spawn->model);
				break;
		}
	}
}

void World::loadSky()
{
	cubemap = new Texture();
	cubemap->loadCubemap("sky", {
			"data/cubemap/sky.tga",
			"data/cubemap/sky.tga",
			"data/cubemap/sky.tga",
			"data/cubemap/sky.tga",
			"data/cubemap/sky.tga",
			"data/cubemap/sky.tga"
	});

}

// NIGHT  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------

void World::generateZombies(int num_night) 
{
	cur_wave = 0;
	for (auto& wave : waves)
		wave.clear();

	ZombieEntity* zombie;
	
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<int> distribution(0, 100);

	int idle_anim = distribution(gen);

	int idx = min(num_night, DIFICULTY_LEVELS-1);
	float probability[NUM_ZOMBIE_TYPES];
	memcpy(probability, zombies_probabilities[idx], sizeof(probability));

	int num_waves = num_night / 5 + 1;
	waves.resize(num_waves);

	for (int wave_idx = 0; wave_idx < num_waves; wave_idx++) {
		waves[wave_idx].resize(NUM_ZOMBIES_WAVE);
		for (int i = 0; i < NUM_ZOMBIES_WAVE; i++)
		{

			zombieType type = zombieType(selectObject(probability, NUM_ZOMBIE_TYPES));

			zombie = new ZombieEntity(type, z_info[type], night_models[3 + i], (idle_anim + i) % NUM_ZOMBIE_IDLES);

			if (type == STANDARD) {
				zombie->info.weakness = weaponType((distribution(gen) % 3) + 1);

				switch (zombie->info.weakness) {
				case BAT:
					zombie->color = Vector4(0.5f, 1.f, 1.f, 1.f);
					break;
				case KNIFE:
					zombie->color = Vector4(1.f, 0.5f, 1.f, 1.f);
					break;
				case GUN:
					zombie->color = Vector4(1.f, 1.f, 0.5f, 1.f);
					break;
				}
			}
			waves[wave_idx][i] = zombie;
		}
	}
};

void World::playHurt(float delay, bool defend, bool dead) {
	int sound;

	if (defend && !dead) {
		sound = player->defensive;
		Audio::PlayDelayed(hurt_sounds[sound].c_str(), 1.f, delay, 0, 0.f);
	}
	else {
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<int> distribution(0, 100);

		sound = distribution(gen) % HURT_SOUNDS + NUM_DEF;

		Audio::PlayDelayed(hurt_sounds[sound].c_str(), 1.f, delay, 0, 0.f);

		sound = (sound + 1) % HURT_SOUNDS + NUM_DEF;
	}

	if(!dead)
		Audio::PlayDelayed(hurt_sounds[sound].c_str(), 1.f, delay * 2.75f, 0, 0.f);

}

void World::playWeaponSound(weaponType weapon, float delay, bool miss, bool dead) {
	int repeat = 1;
	if (dead || weapon == GUN)
		repeat = 0;

	if (miss)
		Audio::PlayDelayed("data/audio/night/miss.wav", 1.f, delay, repeat, delay * 2);
	else
		Audio::PlayDelayed(weapon_sounds[weapon].c_str(), 1.f, delay, repeat, delay * 2);
		
}

int World::hurtZombie(int zombie_idx)
{
	ZombieEntity* zombie = waves[cur_wave][zombie_idx];
	int multiplier = zombie->getMultiplier(weapon);
	zombie->info.health -= weapon_dmg[weapon] * multiplier;

	player->addWeaponUses(weapon, -1);

	idle = false;
	float delay = player->toState(weapon, TRANSITION_TIME) / 3;
	
	// If the zombie is dead -> trigger their death
	if (!zombie->alive()) {
		if (multiplier == 2)
			Audio::Play("data/audio/night/crit.wav", 1.f, false);
		zombie->triggerDeath(delay * 1.5);
		player->toStateDelayed(IDLE, delay * 2.5, TRANSITION_TIME);
	}

	// Trigger one animation or the other depending on the effectiveness of the attack
	else if (multiplier == 1) {
		zombie->toStateDelayed(ZOMBIE_HURT, delay, TRANSITION_TIME);
	}

	else if (multiplier == 2) {
		Audio::Play("data/audio/night/crit.wav", 1.f, false);
		zombie->toStateDelayed(ZOMBIE_HURT_GRAVE, delay, TRANSITION_TIME);
	}
	
	else 
		zombie->toStateDelayed(ZOMBIE_DODGE, delay, TRANSITION_TIME);
	
	playWeaponSound(weapon, 1.5 * delay, multiplier == 0, !zombie->alive());

	return multiplier;
}

bool World::attackPlayer(int zombie_idx)
{
	assert(zombie_idx < waves[cur_wave].size());

	ZombieEntity* zombie = waves[cur_wave][zombie_idx];

	if (!zombie->isAttacking())
	{
		bool mitigating = player->mitigates;
		if (zombie_attacking) {
			zombie_attacking = false;
			return true;
		}

		hurtPlayer(zombie->info.dmg);

		float delay = zombie->toState(zombie->info.weapon, TRANSITION_TIME) / 3.f;

		if (!isPlayerAlive()) {
			player->triggerDeath(delay * 1.5);
			playHurt(delay * 1.5, false, true);
			zombie->toStateDelayed(IDLE, delay * 2.5, TRANSITION_TIME);
		}
		else if(!mitigating){
			player->hurtAnimation(delay);
			playHurt(delay * 1.5, false, false);
		}
		else 
			playHurt(delay * 1.5, true, false);


		zombie_attacking = true;
	}
	return false;
}

void World::removeZombie(int zombie_idx)
{
	if (zombie_idx >= 0 && zombie_idx < waves[cur_wave].size())
	{
		ZombieEntity* zombie = waves[cur_wave][zombie_idx];
		waves[cur_wave].erase(std::next(waves[cur_wave].begin(), zombie_idx));
		delete zombie->anim_manager;
		delete zombie;
	}
}

void World::defend(defensiveType type)
{
	player->mitigates = defensive_stats[type];
	player->def_uses[type]--;
	player->defensive = type;
	player->toState(PLAYER_DEFEND, TRANSITION_TIME);
}

int World::zombiesAlive() {
	return waves[cur_wave].size();
}

bool World::nextWave() {

	if (cur_wave + 1 >= waves.size())
		return true;
	cur_wave++;
	return false;
}

// NIGHTMENU RELATED ---------------------------------------------------------------------------------------
void World::changeMenu(std::string go_to)
{
	cur_menu = menus[go_to];
	selected_option = 0;

	cur_menu->start_visible = 0;
	cur_menu->end_visible = 2;
}

void World::changeOption(int to_add)
{
	Audio::Play("data/audio/menu/change_option.wav", 1.f, false);

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

void World::selectWeapon(int w_type)
{
	ready_to_attack = true;
	player->toState(PLAYER_FISTS_IDLE + w_type, TRANSITION_TIME / 2.f);
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

	general->options.resize(NUM_GENERAL_OPTIONS);

	std::string data;
	for (int option = 0; option < NUM_GENERAL_OPTIONS; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		general->options[option] = new GeneralMenuEntity(
			Texture::Get(tokens[1].c_str()),
			Texture::Get(tokens[2].c_str()),
			tokens[3]);
	}

	file.ignore(1, '\n');

	//Consumable menu 
	Menu* consumables = new Menu();
	menus["consumables"] = consumables;

	consumables->options.resize(NUM_CONSUMABLES);

	for (int option = 0; option < NUM_CONSUMABLES; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		consumables->options[option] = new ConsumableMenuEntity(
			Texture::Get(tokens[1].c_str()),
			Texture::Get(tokens[2].c_str()),
			consumableType(std::stoi(tokens[3])));
	}

	file.ignore(1, '\n');

	//Weapon menu 
	Menu* weapon = new Menu();
	menus["weapon"] = weapon;

	weapon->options.resize(NUM_WEAPONS);
	for (int option = 0; option < NUM_WEAPONS; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		weapon->options[option] = new WeaponMenuEntity(
			Texture::Get(tokens[1].c_str()),
			Texture::Get(tokens[2].c_str()),
			weaponType(std::stoi(tokens[3]))
		);
	}

	file.ignore(1, '\n');

	//Defend menu
	Menu* defensive = new Menu();
	menus["defensive"] = defensive;
	defensive->options.resize(NUM_DEF);

	for (int option = 0; option < NUM_DEF; option++)
	{
		file >> data;
		std::vector<std::string> tokens = tokenize(data, ",");

		defensive->options[option] = new DefensiveMenuEntity(
				Texture::Get(tokens[1].c_str()),
				Texture::Get(tokens[2].c_str()),
				defensiveType(std::stoi(tokens[3]))
			);
	}
}

void World::resizeOptions(float width, float height)
{

	window_width = width;
	window_height = height;

	fullscreen_quad.createQuad(window_width / 2.f, window_height / 2.f, window_width, window_height, true);

	float size_y = 100.f * height / 1080;
	float size_x = size_y * 350.f / 100.f;

	float offset = 0.05 * width;

	option_uses_pos[0] = Vector2(0.85 * width, 3 * size_y + 2 * offset);
	option_uses_pos[1] = Vector2(0.85 * width, 2 * size_y + offset);
	option_uses_pos[2] = Vector2(0.85 * width, 1 * size_y);

	option_quads[0]->createQuad(option_uses_pos[0].x, option_uses_pos[0].y, size_x, size_y, true);
	option_quads[1]->createQuad(option_uses_pos[1].x, option_uses_pos[1].y, size_x, size_y, true);
	option_quads[2]->createQuad(option_uses_pos[2].x, option_uses_pos[2].y, size_x, size_y, true);

	for (int i = 0; i < 3; i++)
	{
		option_uses_pos[i].x += 0.4 * size_x;
		option_uses_pos[i].y = height - option_uses_pos[i].y;
	}
		
	size_x = 2.f * width / 3.f;
	size_y = size_x * 281.f / 762.f;
	float position_x = width / 2;
	float position_y = 0.55 * size_y;

	tutorial_quad.createQuad(position_x, position_y, size_x, size_y, true);
	pause_menu->resize(width, height);
}

// Animation related
void World::updateAnimations(float dt)
{
	player->updateAnim(dt);

	bool local = player->isIdle();

	std::vector<int> to_remove;

	for (int i = 0; i < waves[cur_wave].size(); i++) {
		ZombieEntity* zombie = waves[cur_wave][i];
		zombie->updateAnim(dt);

		if (shouldTrigger(zombie->time_til_death, dt))
			to_remove.push_back(i);

		local = local && zombie->isIdle();
	}

	for (auto& idx : to_remove)
		removeZombie(idx);

	idle = local;
}

void World::playerToState(int state, float time)
{
	player->toState(state, time);
}

void World::renderNight()
{
	Camera* camera = Camera::current;
	// Player
	player->render();

	if (ready_to_attack || player->hasWeapon())
		player->renderHolding(weapon_mesh_info[weapon].mesh, 
			camera,
			weapon_mesh_info[weapon].player_offset, 
			weapon_mesh_info[weapon].player_rotate, 
			weapon_mesh_info[weapon].player_angle, 
			weapon_mesh_info[weapon].player_axis,
			true
		);
	else if (player->defensive) {
		int idx = player->defensive;
		player->renderHolding(def_mesh_info[idx].mesh,
			camera,
			def_mesh_info[idx].player_offset,
			def_mesh_info[idx].player_rotate,
			def_mesh_info[idx].player_angle,
			def_mesh_info[idx].player_axis,
			false
		);
	}
	// Zombies
	for (auto& zombie : World::inst->waves[cur_wave])
	{
		zombie->render();
		int zombie_weapon = zombie->info.weapon;
		zombie->renderHolding(weapon_mesh_info[zombie_weapon].mesh,
			camera,
			weapon_mesh_info[zombie_weapon].zombie_offset,
			weapon_mesh_info[zombie_weapon].zombie_rotate,
			weapon_mesh_info[zombie_weapon].zombie_angle,
			weapon_mesh_info[zombie_weapon].zombie_axis,
			true
		);
	}
}

void World::resetWorld()
{
	frozen = false;
	player = new Player();
	number_nights = 0;
}