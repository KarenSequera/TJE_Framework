#pragma once
#include "includes.h"
#include "utils.h"

#include "mesh.h"
#include "texture.h"
#include "shader.h"

#include "animationManager.h"

#include <vector> 

class Entity {
public:
	// members
	Matrix44 model_matrix;

	Entity* parent;
	std::vector<Entity*> children;
	bool does_move;

	// ctor & destructor
	Entity(bool moves = true);

	void addChild(Entity* child);
	void removeChild(Entity* child);

	Matrix44 getGlobalMatrix();

	// methods to be overwritten
	virtual void render();
	virtual void update(float dt) {};
}; 


class EntityMesh : public Entity {
public:
	// members
	Mesh* mesh;
	Texture* texture;
	Shader* shader;

	Vector4 color;

	bool is_instanced;
	std::vector<Matrix44> models;

	//methods
	EntityMesh();
	EntityMesh(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, bool is_instanced, bool moves = true);

	virtual void render();
	void render_simple();
	void render_instanced();

	void addInstance(Matrix44 model);
};

class EntityCollision : public EntityMesh {
public:
	bool is_dynamic;

	EntityCollision(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, bool is_instanced, bool dynamic, bool moves = true);
};

class AnimatedEntity : public EntityMesh {
public:
	AnimationManager* anim_manager;
	AnimatedEntity();

	float time_til_death;

	void render();
	void renderHolding(Mesh* mesh, Camera* camera, Vector3 offset, bool rotate, float rot_angle, Vector3 rot_axis, bool right_hand);
	virtual void updateAnim(float dt);
	void triggerDeath(float delay);
	float toState(int state, float time = 0.f);
	float toStateDelayed(int state, float to_start, float time = 0.f);
	bool isIdle();
	Matrix44 getBoneMatrix(const char* name);

};
