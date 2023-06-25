#include "StageManager.h"
#include "input.h"
#include "camera.h"

#include <algorithm>

Stage::Stage() {
	camera = Camera::current;
	post_fx = false;
	renderTarget = nullptr;
	mouse_locked = false;

	num_slides = 0;
	cur_slide = 0;
	in_tutorial = false;

	if (!renderTarget) {
		renderTarget = new RenderToTexture();
		renderTarget->create(RENDER_TARGET_RES, RENDER_TARGET_RES);
	}
}

void Stage::renderTutorial() {
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	// render the textbox
	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_animated", false);
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", Texture::Get("data/quad_textures/tutorial/overlay.tga"), 0);
	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/tutorial/message.tga"), 0);
	World::inst->tutorial_quad.render(GL_TRIANGLES);

	// render the text
	shader->setUniform("u_texture", slides[cur_slide], 0);
	World::inst->tutorial_quad.render(GL_TRIANGLES);

	shader->disable();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
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

	shader->setUniform("u_animated", false);
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
	shader->setUniform("u_animated", false);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/grayTexture.tga"), 0);
	quad1.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/NightTextures/orangeTexture.tga"), 0);
	quad2.render(GL_TRIANGLES);

}


void Stage::nextSlide() {
	Audio::Play("data/audio/messages/continue.wav", 1.f, false);

	cur_slide++;
	if (cur_slide >= num_slides)
		in_tutorial = false;
}

void Stage::updateTutorial() {
	//We check if the gamepad is connected:
	if (Input::gamepads[0].connected) {
		if (Input::wasButtonPressed(A_BUTTON)) {
			nextSlide();
		}
	}
	else
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_A)) {
			nextSlide();

		}
	}
}

void Stage::stopMusic() {
	Audio::Stop(channel);
}