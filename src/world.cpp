#include "world.h"
#include "mesh.h"
#include "texture.h"
#include "shader.h"

World* World::world_instance = NULL;

World::World() {
	//TODO: maybe parse the stats from files.
	world_instance = this;
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

void World::useConsumable(consumableType consumable, affectingStat stat)
{
	player->consumables[consumable]--;
	int to_add = consumable_stats[consumable];
	player->affectPlayerStat(stat, to_add, true);
}

