#pragma once
#include "includes.h"
#include "utils.h"

class Mesh;
class Texture;
class Shader;

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

	void render();
	void update(float dt);
};