#include "introStage.h"
#include "StageManager.h"
#include "camera.h"
#include "game.h"
#include "our_utils.h"
#include "input.h"

#include <algorithm>
#include <cstdlib>
 
IntroStage::IntroStage()
{
	window_width = Game::instance->window_width;
	window_height = Game::instance->window_height;
	camera2D = new Camera();
	camera2D->view_matrix = Matrix44();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);


	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
		option_quads[i] = new Mesh();

	resizeOptions(Game::instance->window_width, Game::instance->window_height);

	Texture* start_button = Texture::Get("data/quad_textures/intro/boton_start.tga");
	Texture* start_button_selected = Texture::Get("data/quad_textures/intro/boton_start_selected.tga");

	Texture* exit_button = Texture::Get("data/quad_textures/intro/boton_exit.tga");
	Texture* exit_button_selected = Texture::Get("data/quad_textures/intro/boton_exit_selected.tga");

	options.push_back(new MenuEntity(start_button, start_button_selected));
	options.push_back(new MenuEntity(exit_button, exit_button_selected));
}

void IntroStage::onEnter()
{
	channel = Audio::Play("data/audio/intro/IntroSong.wav", 0.3f, true);
	selected_option = 0;
}

void IntroStage::onExit(){}

void IntroStage::render()
{

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));

	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	shader->setUniform("u_animated", false);
	shader->setUniform("u_texture", Texture::Get("data/quad_textures/intro/background.tga"), 0);
	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/intro/logo.tga"), 0);
	logo.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/intro/a_to_select.tga"), 0);
	a_to_select.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/intro/x_tutorial.tga"), 0);
	x_tutorial.render(GL_TRIANGLES);

	shader->disable();


	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
		options[i]->render(selected_option == i, option_quads[i]);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);

	if (World::inst->triggerTutorial) {
		drawText(World::inst->window_width / 2 + World::inst->window_height * 0.3, World::inst->window_height*0.9105,
			"on", Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height * 0.004);
	}
	else {
		drawText(World::inst->window_width / 2 + World::inst->window_height * 0.3, World::inst->window_height*0.9105,
			"off", Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height * 0.004);
	}
	
}

void IntroStage::update(float dt, bool transitioning)
{
	if (Input::gamepads[0].connected) {
		if (Input::gamepads[0].didDirectionChanged(FLICK_LEFT))
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);

		else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))

			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasButtonPressed(A_BUTTON))
			selectOption();
		else if (Input::wasButtonPressed(X_BUTTON))
			World::inst->triggerTutorial = !World::inst->triggerTutorial;
	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT))
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasKeyPressed(SDL_SCANCODE_C))
			selectOption();
		else if (Input::wasKeyPressed(SDL_SCANCODE_X)) {
			World::inst->triggerTutorial = !World::inst->triggerTutorial;
		}
	}
}

void  IntroStage::resizeOptions(float width, float height)
{
	//TODO: ADAPT THIS TO THE NEW ASSETS
	float size_y = 100.f * height / 1080;
	float size_x = size_y * 350.f / 100.f;

	float offset = size_x/1.7;

	option_uses_pos[0] = Vector2(width/2 - offset, size_y*1.8);
	option_uses_pos[1] = Vector2(width/2 + offset, size_y*1.8);

	option_quads[0]->createQuad(option_uses_pos[0].x, option_uses_pos[0].y, size_x, size_y, true);
	option_quads[1]->createQuad(option_uses_pos[1].x, option_uses_pos[1].y, size_x, size_y, true);
	
	for (int i = 0; i < 2; i++)
	{
		option_uses_pos[i].x += 0.1 * width;
		option_uses_pos[i].y = height - option_uses_pos[i].y + 10;
	}

	logo.createQuad(World::inst->window_width / 2, World::inst->window_height / 1.7, World::inst->window_height / 1.2, World::inst->window_height / 1.2, true);
	a_to_select.createQuad(width / 2 - offset, size_y/1.3, size_x, size_y, true);
	x_tutorial.createQuad(width / 2 + offset, size_y / 1.3, size_x, size_y, true);

}

bool IntroStage::selectOption()
{
	switch (selected_option) {
		case 0:
			StageManager::inst->changeStage("day");
			break;
		case 1:
			exit(0);
			break;
	}
}