#pragma once
#include "includes.h"
#include "utils.h"

#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include <vector> 

class Entity {
public:

	Matrix44 model_matrix;

	Entity* parent;
	std::vector<Entity*> children;

	// ctor & destructor
	Entity();

	void addChild(Entity* child);
	void removeChild(Entity* child);

	Matrix44 getGlobalMatrix();

	// methods to be overwritten
	virtual void render();
	virtual void update(float dt) {};
}; 


class EntityMesh : public Entity {
public:
	Mesh* mesh;
	Texture* texture;
	Shader* shader;

	EntityMesh();
	EntityMesh(Mesh* in_mesh, Texture* in_texture, Shader* in_shader);

	virtual void render();
	void update(float dt);
};

class InstancedEntityMesh : public EntityMesh {
public:

	std::vector<Matrix44> models;
	InstancedEntityMesh();
	InstancedEntityMesh(Mesh* in_mesh, Texture* in_texture, Shader* in_shader) : EntityMesh(in_mesh, in_texture, in_shader) {};
	void render();
	void addInstance(Matrix44 model);
};