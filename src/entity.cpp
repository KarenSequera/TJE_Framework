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
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_model", getGlobalMatrix());
		shader->setUniform("u_time", time);

		//do the draw call
		mesh->render(GL_TRIANGLES);

		//disable shader
		shader->disable();
	}
	Entity::render();
}

void EntityMesh::update(float dt) {

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

	if (shader)
	{
		//enable shader
		shader->enable();

		//upload uniforms
		shader->setUniform("u_color", Vector4(1, 1, 1, 1));
		shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
		shader->setUniform("u_texture", texture, 0);
		shader->setUniform("u_time", time);

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