#include "StageManager.h"
#include "input.h"
#include "camera.h"

#include <algorithm>

Stage::Stage() {
	camera = Camera::current;
	mouse_locked = false;
}

void Stage::renderHealthBar(Vector3 position, float hp_ratio, Shader* shader, float width, float height)
{
	Mesh quad1;
	Mesh quad2;

	//Creation of the first quad, the background one.
	quad1.createQuad(position.x, position.y, width, height, true);

	//Creation of the second quad. This one contains the life information. 
	float greenBarWidth = width * hp_ratio;
	float offset = (width - greenBarWidth) * 0.5f;
	quad2.createQuad(position.x - offset, position.y, greenBarWidth, height, true);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/redTexture.tga"), 0);
	quad1.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/greenTexture.tga"), 0);
	quad2.render(GL_TRIANGLES);

}

void Stage::renderHungerBar(Vector3 position, float hunger_ratio, Shader* shader, float width, float height)
{
	Mesh quad1;
	Mesh quad2;

	//Creation of the first quad, the background one.
	quad1.createQuad(position.x, position.y, width, height, true);

	//Creation of the second quad. This one contains the life information. 
	float greenBarWidth = width * hunger_ratio;
	float offset = (width - greenBarWidth) * 0.5f;
	quad2.createQuad(position.x - offset, position.y, greenBarWidth, height, true);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/grayTexture.tga"), 0);
	quad1.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/orangeTexture.tga"), 0);
	quad2.render(GL_TRIANGLES);

}

void Stage::stopMusic() {
	Audio::Stop(channel);
}