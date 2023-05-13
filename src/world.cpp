#include "world.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "entity.h"
#include "utils.h"

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

	//for (int i = 0; i < 10; ++i) {
	//	for (int j = 0; j < 10; ++j) {
	//		entity = new EntityMesh(
	//			Mesh::Get("data/pruebas/advanced.obj"),
	//			Texture::Get("data/pruebas/skin.tga"),
	//			shader
	//		);

	//		entity->model_matrix.setTranslation(i * 400.f, j * 400.f, 0.f);

	//		day_entities.push_back(entity);
	//	}
	//}
	parseScene("data/myscene.scene");

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

struct sRenderData {
	Texture* texture = nullptr;
	Shader* shader = nullptr;
	std::vector<Matrix44> models;
};

std::map<std::string, sRenderData> meshes_to_load;

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
		//if (render_data.models.size() > 1) {
		//	InstancedEntityMesh* new_entity = new InstancedEntityMesh(Mesh::Get(mesh_name.c_str()), render_data.shader, render_data.texture);
		//	// Add all instances
		//	new_entity->models = render_data.models;
		//	// Add entity to scene root
		//	root.addChild(new_entity);
		//}
		// Create normal entity

		EntityMesh* new_entity = new EntityMesh(Mesh::Get(mesh_name.c_str()), Texture::Get("data/texture.tga"), Shader::Get("data/shaders/basic.vs", "data/shaders/texture.fs"));
		new_entity->model_matrix = render_data.models[0];
		// Add entity to scene root
		day_root->addChild(new_entity);
		
	}

	std::cout << "Scene [OK]" << " Meshes added: " << mesh_count << std::endl;
	return true;
}