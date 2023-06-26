#pragma once

#include "worldEntities.h"
#include "menu.h"
#include <unordered_map>


#define MAX_ITEM_DIST 1000
#define DIFICULTY_LEVELS 4
#define NUM_ZOMBIES_WAVE 3
#define HURT_SOUNDS 5

struct sCollisionData {
	Vector3 colPoint;
	Vector3 colNormal;
};

struct sHoldableMeshData {
	Mesh* mesh;
	Vector3 player_offset;
	Vector3 zombie_offset;
	int player_rotate;
	int zombie_rotate;
	float player_angle;
	float zombie_angle;
	Vector3 player_axis;
	Vector3 zombie_axis;
};


class World {
public:
	// General variables
	static World* inst;
	Player* player;

	Mesh fullscreen_quad;
	Mesh tutorial_quad;
	bool triggerTutorial;
	bool frozen;

	float window_width;
	float window_height;

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
	std::vector<sHoldableMeshData> weapon_mesh_info;
	std::vector<sHoldableMeshData> def_mesh_info;

	std::vector<std::string> weapon_sounds;
	std::vector<std::string> hurt_sounds;

	Texture* cubemap;

	//arrays containing the probabilities
	float weapon_probabilities[NUM_WEAPONS] = { 0.0, 0.4, 0.4, 0.2 };
	float consumable_probabilities[NUM_CONSUMABLES] = {0.5, 0.375, 0.125, 0.5, 0.375, 0.125};
	float defensive_probabilities[NUM_DEF] = {0.0,0.666,0.333};

	float zombies_probabilities[DIFICULTY_LEVELS][NUM_ZOMBIE_TYPES] = {
		{1,0,0,0},
		{0,1,0,0},
		{0,0,1,0},
		{0,0,0,1}
	};

	//Night variables 
	std::vector<std::vector<ZombieEntity*>> waves;
	int cur_wave;

	zombieInfo z_info[NUM_ZOMBIE_TYPES];

	weaponType weapon;

	// Menus
	float w_width;
	float w_height;
	Mesh* option_quads[3];
	Vector2 option_uses_pos[3];

	Camera* camera2D;
	int number_nights;

	std::unordered_map<std::string, Menu*> menus;
	Menu* cur_menu;
	int selected_option;
	bool ready_to_attack;

	PauseMenu* pause_menu;

	//Night model information 
	Matrix44 night_models[3+NUM_ZOMBIES_WAVE];

	//Animation
	bool idle; 
	bool zombie_attacking;

	int zombie_hurt;

	World();
	
	bool existPreviousRuns();
	void getSounds();
	
	// functions to parse
	void parseStats(const char* filename);
	void getMeshesToLoad(const char* filename);
	void parseSceneDay();
	void parseSceneNight(const char* filename);
	void parseSpawns(const char* filename);
	void parseItemEntities(const char* filename);
	

	// General logic
	void hurtPlayer(int damage);
	void playerDefenseOff();
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

	int checkPlayerCollisions(const Vector3& target_pos, std::vector<sCollisionData>* collisions);
	void loadSky();
	
	// NIGHT  ---------------------------------------------------------------------------------------------------------------------------------------------------------------------------
	void generateZombies(int num_night);

	void playHurt(float delay, bool defend, bool dead);

	// ZOMBIE RELATED
	// This function is called when the player atacks a zombie
		//Depends on the vulnerabilities will return 0,1,2
		// 0 -> invulnerable (no damage)
		// 1 -> normal damage
		// 2 -> attack was super efective (x2 damage), player has 
	int hurtZombie(int zombie_idx);
	// Tries to hurt the player and returns whether the zombie has finished attacking the player or not
	bool attackPlayer(int zombie_idx);
	// removes a zombie from the wave vector
	void removeZombie(int zombie_idx);
	void defend(defensiveType type);
	int zombiesAlive();
	bool nextWave();

	void playWeaponSound(weaponType weapon, float delay, bool dead, bool miss);

	// MENU RELATED
	void changeMenu(std::string go_to);
	void changeOption(int to_add);
	void resizeOptions(float width, float height);

	bool selectOption();
	void selectWeapon(int w_type);
	void createMenus(std::string filename);

	// ANIMATION RELATED
	void updateAnimations(float dt);
	void playerToState(int state, float time = 0.f);
	void renderNight();
	//TODO: void zombieToState(int zombie_idx, int state, float time = 0.f);

	void resetWorld();

};
