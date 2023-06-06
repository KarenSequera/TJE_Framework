#pragma once

#include "worldEntities.h"
#include "nightMenu.h"
#include <unordered_map>


#define MAX_ITEM_DIST 1000
#define DIFICULTY_LEVELS 4
#define NUM_ZOMBIES_WAVE 3

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};

class World {
public:
	// General variables
	static World* inst;
	Player* player;

	bool unlimited_everything;

	int consumable_stats[NUM_CONSUMABLES];
	int weapon_dmg[NUM_WEAPONS];
	int weapon_use_pts[NUM_WEAPONS];
	int defensive_stats[NUM_DEF];
	int defensive_use_pts[NUM_DEF];

	// Day variables
	Entity* day_root;
	std::vector<Entity*> day_entities;
	std::vector<EntitySpawner*> item_spawns;

	std::vector<std::vector<ItemEntity*>> items;

	//arrays containing the probabilities
	float weapon_probabilities[NUM_WEAPONS] = { 0.0, 0.4, 0.4, 0.2 };
	float consumable_probabilities[NUM_CONSUMABLES] = {0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125, 0.125};
	float defensive_probabilities[NUM_DEF] = {0.0,0.666,0.333};

	float zombies_probabilities[DIFICULTY_LEVELS][NUM_ZOMBIE_TYPES] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};

	//Night variables 
	std::vector<ZombieEntity*> wave;
	int zombies_alive;

	zombieInfo z_info[NUM_ZOMBIE_TYPES];

	weaponType weapon;

	// Menus
	float window_width;
	float window_height;
	Mesh* option_quads[3];
	Vector2 option_uses_pos[3];

	Camera* camera2D;

	std::unordered_map<std::string, Menu*> menus;
	Menu* cur_menu;
	int selected_option;
	bool ready_to_attack;

	//Night model information 
	Matrix44 night_models[3+NUM_ZOMBIES_WAVE];


	World();
	
	// functions to parse
	void parseStats(const char* filename);
	void parseSceneDay(const char* filename);
	void parseSceneNight(const char* filename);
	void parseSpawns(const char* filename);
	void parseItemEntities(const char* filename);
	

	// General logic
	void hurtPlayer(weaponType weapon);
	void consumeHunger(int quant);

	int getConsumableQuant(consumableType consumable);
	int getWeaponUses(weaponType weapon);
	int getDefItemUses(defensiveType def);

	int useConsumable(consumableType consumable);
	void clearItems();
	void spawnerInit();
	
	bool isPlayerAlive();

	// DAY  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void getConsumable(consumableType consumable);
	void addWeaponUses(weaponType weapon);
	void addDefItemUses(defensiveType def);
	void getItem(ItemEntity* item);

	bool checkItemCollisions(const Vector3& ray_dir);
	int checkPlayerCollisions(const Vector3& target_pos, std::vector<sCollisionData>* collisions);
	
	// NIGHT  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void generateZombies(int num_night);

	// ZOMBIE RELATED
	// This function is called when the player atacks a zombie
		//Depends on the vulnerabilities will return 0,1,2
		// 0 -> invulnerable (no damage)
		// 1 -> normal damage
		// 2 -> attack was super efective (x2 damage), player has 
	int hurtZombie(int zombie_idx);
	void killZombie(int zombie_idx);
	void defend(defensiveType type);

	// MENU RELATED
	void changeMenu(std::string go_to);
	void changeOption(int to_add);
	void resizeOptions(float width, float height);

	bool selectOption();
	void createMenus(std::string filename);
};
