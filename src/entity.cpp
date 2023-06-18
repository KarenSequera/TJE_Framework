#include "entity.h"

#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

Entity::Entity(bool moves) {
	model_matrix = Matrix44();
	parent = nullptr;
	does_move = moves;
}

void Entity::addChild(Entity* child) {
	children.push_back(child);
	child->parent = this;
}

void Entity::removeChild(Entity* child) {
	children.erase( find(children.begin(), children.end(), child) );
	child->parent = nullptr;
}

Matrix44 Entity::getGlobalMatrix() {
	if (parent)
		return model_matrix * parent->getGlobalMatrix();
	else
		return model_matrix;
}

//TODO: method for computing the global matrix of entities that do NOT move -> maybe add another members
// for the global matrix;

void Entity::render() {
	for (auto& child : children) {
		child->render();
	}
}

EntityMesh::EntityMesh() : Entity(){
	mesh = nullptr;
	texture = nullptr;
	shader = nullptr;
	is_instanced = false;
}

EntityMesh::EntityMesh(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, bool instanced, bool moves) : Entity(moves){
	mesh = in_mesh;
	texture = in_texture;
	shader = in_shader;
	is_instanced = instanced;
}

void EntityMesh::render()
{
	if (is_instanced)
		render_instanced();
	else
		render_simple();
}

void EntityMesh::render_simple() {
	
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Camera* camera = Camera::current;
	
	//TODO: ACTIVATE FOR FINAL DELIVERY
	/*Vector3 sphere_center = model_matrix * mesh->box.center;
	float sphere_radius = mesh->radius;

	if (camera->testSphereInFrustum(sphere_center, sphere_radius) == false
		|| camera->eye.distance(model_matrix.getTranslation()) > 10000)
		return;*/

	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_model", model_matrix);

		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_time", time);
		shader->setUniform("u_camera_pos", camera->center);
		shader->setUniform("u_light_color", Vector3(1.0, 1.0, 0.9));
		shader->setUniform("u_light_dir", Vector3(0.0, 0.1, 0.1));
		shader->setUniform("u_Ia", Vector3(0.5, 0.5, 0.5));

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
	Entity::render();
}

void EntityMesh::addInstance(Matrix44 model) {
	models.push_back(model);
}

void EntityMesh::render_instanced() {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Camera* camera = Camera::current;

	// if there are no models, then we do not need to do anything
	if (shader && models.size())
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_time", time);
		shader->setUniform("u_camera_pos", camera->center);
		shader->setUniform("u_light_color", Vector3(1.0, 1.0, 1.0));
		shader->setUniform("u_light_dir", Vector3(0.0, 0.75, 0.75));
		shader->setUniform("u_Ia", Vector3(0.5, 0.5, 0.5));

		//do the draw call
		mesh->renderInstanced(GL_TRIANGLES, models.data(), models.size());
		//disable shader
		shader->disable();
	}

	Entity::render();
}

EntityCollision::EntityCollision(Mesh* in_mesh, Texture* in_texture, Shader* in_shader, bool is_instanced, bool dynamic, bool moves)
	: EntityMesh(in_mesh, in_texture, in_shader, is_instanced, moves)
{
	is_dynamic = dynamic;
}

AnimatedEntity::AnimatedEntity()
{
	anim_manager = nullptr;
}

void AnimatedEntity::render()
{
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Camera* camera = Camera::current;

	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", getGlobalMatrix());
		shader->setUniform("u_time", time);

		//do the draw call
		mesh->renderAnimated(GL_TRIANGLES, &anim_manager->getCurrentSkeleton());

		//disable shader
		shader->disable();
	}
	Entity::render();
}

void AnimatedEntity::renderWeapon(Mesh* mesh, Camera* camera, Vector3 offset, bool rotate, float rot_angle, Vector3 rot_axis) {

	if (!mesh)
		return;

	Shader* shader = Shader::Get("data/shaders/basic.vs", "data/shaders/phong.fs");

	// get model
	Matrix44 model = this->getBoneMatrix("mixamorig_RightHandIndex2");

	model = model * model_matrix;

	Vector3 pos = model.getTranslation() + offset;
	model.setTranslation(pos, false);
	
	if(rotate)
		model.rotate(rot_angle, rot_axis);

	shader->enable();
	shader->setUniform("u_color", Vector4(1, 1, 1, 1));
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_texture", texture, 0);
	shader->setUniform("u_time", time);
	shader->setUniform("u_model", model);
	shader->setUniform("u_camera_pos", camera->center);
	shader->setUniform("u_light_color", Vector3(1.0, 1.0, 1.0));
	shader->setUniform("u_light_dir", Vector3(0.0, 0.75, 0.75));
	shader->setUniform("u_Ia", Vector3(0.5, 0.5, 0.5));

	mesh->render(GL_TRIANGLES);

	shader->disable();
}

// Returns: wheter the entity is idle
void AnimatedEntity::updateAnim(float dt)
{
	anim_manager->update(dt);
}

void AnimatedEntity::triggerDeath(float delay)
{
	time_til_death = toStateDelayed(DYING, delay, 0.75f) * 3.f / 4.f;
}

float AnimatedEntity::toState(int state, float time)
{
	return anim_manager->goToState(state, time);
}

float AnimatedEntity::toStateDelayed(int state, float to_start, float time)
{
	return anim_manager->goToStateDelayed(state, to_start, time);
}


Matrix44 AnimatedEntity::getBoneMatrix(const char* name)
{
	return anim_manager->getCurrentSkeleton().getBoneMatrix(name, false);
}

bool AnimatedEntity::isIdle()
{
	return anim_manager->isIdle();
}

