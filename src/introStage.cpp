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

	Texture* start_button = Texture::Get("data/menus/introMenu/PLAY_BUTTON.tga");
	Texture* start_button_selected = Texture::Get("data/menus/introMenu/PLAY_BUTTON_SELECTED.tga");;

	Texture* exit_button = Texture::Get("data/menus/introMenu/EXIT_BUTTON.tga");
	Texture* exit_button_selected = Texture::Get("data/menus/introMenu/EXIT_BUTTON_SELECTED.tga");

	options.push_back(new MenuEntity(start_button, start_button_selected));
	options.push_back(new MenuEntity(exit_button, exit_button_selected));
	
}

void IntroStage::onEnter()
{
	selected_option = 0;
}

void IntroStage::onExit(){}

void IntroStage::render()
{
	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
	{
		options[i]->render(selected_option == i, option_quads[i]);
	}
}

void IntroStage::update(float dt)
{
	if (Input::gamepads[0].connected) {
		if (Input::gamepads[0].didDirectionChanged(FLICK_UP)) {
			Audio::Play("data/audio/change_option.wav", 1.f, false);
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);
		}
		else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))
		{
			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
			Audio::Play("data/audio/change_option.wav", 1.f, false);
		}
		else if (Input::wasButtonPressed(A_BUTTON))
			selectOption();

	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_W) || Input::wasKeyPressed(SDL_SCANCODE_UP)) {
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);
			Audio::Play("data/audio/change_option.wav", 1.f, false);
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasKeyPressed(SDL_SCANCODE_DOWN)) {
			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
			Audio::Play("data/audio/change_option.wav", 1.f, false);
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_C))
			selectOption();
	}
}

void  IntroStage::resizeOptions(float width, float height)
{
	//TODO: ADAPT THIS TO THE NEW ASSETS
	float size_y = 100.f * height / 1080;
	float size_x = size_y * 350.f / 100.f;

	float offset = 0.05 * height;

	option_uses_pos[0] = Vector2(width/2, 2 * size_y +  offset);
	option_uses_pos[1] = Vector2(width/2, 1 * size_y);


	option_quads[0]->createQuad(option_uses_pos[0].x, option_uses_pos[0].y, size_x, size_y, true);
	option_quads[1]->createQuad(option_uses_pos[1].x, option_uses_pos[1].y, size_x, size_y, true);
	

	for (int i = 0; i < 2; i++)
	{
		option_uses_pos[i].x += 0.1 * width;
		option_uses_pos[i].y = height - option_uses_pos[i].y + 10;
	}
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