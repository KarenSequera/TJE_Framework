#include "entity.h"

#include "mesh.h"
#include "texture.h"
#include "shader.h"
#include "camera.h"

Entity::Entity() {
	model_matrix = Matrix44();
	parent = nullptr;
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

EntityMesh::EntityMesh() : Entity(){
	mesh = nullptr;
	texture = nullptr;
	shader = nullptr;
}

EntityMesh::EntityMesh(Mesh* in_mesh, Texture* in_texture, Shader* in_shader) : Entity(){
	mesh = in_mesh;
	texture = in_texture;
	shader = in_shader;
}

void EntityMesh::render() {
	//set flags
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	Camera* camera = Camera::current;
	
	Vector3 sphere_center = model_matrix * mesh->box.center;
	float sphere_radius = mesh->radius;

	if (camera->testSphereInFrustum(sphere_center, sphere_radius) == false
		|| camera->eye.distance(model_matrix.getTranslation()) > 10000)
		return;

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

}

void EntityMesh::update(float dt) {

}