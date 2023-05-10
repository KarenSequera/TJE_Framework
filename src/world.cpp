#include "world.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include <fstream>
#include <map>

World* World::world_instance = NULL;

World::World() {
	//TODO: maybe parse the stats from files.
	world_instance = this;

	parseStats("data/stats.txt");

	player = new Player();
	
	day_root = new Entity();
	day_entities.push_back(day_root);

	Entity* entity;
	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs");

	for (int i = 0; i < 10; ++i) {
		for (int j = 0; j < 10; ++j) {
			entity = new EntityMesh(
				Mesh::Get("data/pruebas/advanced.obj"),
				Texture::Get("data/pruebas/skin.tga"),
				shader
			);

			entity->model_matrix.setTranslation(i * 400.f, j * 400.f, 0.f);

			day_entities.push_back(entity);
		}
	}
}

void World::parseStats(const char* filename) {
	std::ifstream file(filename);
	if (!file.good()) {
		std::cerr << "World [ERROR]" << " Stats file not found!" << std::endl;
		exit(-1);
	}
	std::string data;
	int sizes[] = {NUM_CONSUMABLES, NUM_WEAPONS, NUM_WEAPONS, NUM_DEF, NUM_DEF };
	int* array_ptr[] = { consumable_stats , weapon_dmg , weapon_use_pts, defensive_stats, defensive_durability };
	int line = 0;
	while (file >> data && line < 5)
	{
		// Get all information.
		std::vector<std::string> tokens = tokenize(data, ",");
		for (int t = 0; t < sizes[line]; ++t) {
			array_ptr[line][t] = (int)atoi(tokens[t + 1].c_str());
		}
		line++;
	}

}


void World::useConsumable(consumableType consumable, affectingStat stat)
{
	player->consumables[consumable]--;
	int to_add = consumable_stats[consumable];
	player->affectPlayerStat(stat, to_add, true);
}

