#include "GameOverStage.h"
#include "StageManager.h"
#include "camera.h"
#include "game.h"
#include "our_utils.h"
#include "input.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <vector>

 
GameOverStage::GameOverStage()
{
	window_width = Game::instance->window_width;
	window_height = Game::instance->window_height;
	camera2D = new Camera();
	camera2D->view_matrix = Matrix44();
	camera2D->setOrthographic(0, Game::instance->window_width, 0, Game::instance->window_height, -1, 1);
	
	
	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
		option_quads[i] = new Mesh();

	resizeOptions(Game::instance->window_width, Game::instance->window_height);

	Texture* start_button = Texture::Get("data/introTextures/boton_start.tga");
	Texture* start_button_selected = Texture::Get("data/introTextures/boton_start_selected.tga");

	Texture* exit_button = Texture::Get("data/introTextures/boton_exit.tga");
	Texture* exit_button_selected = Texture::Get("data/introTextures/boton_exit_selected.tga");

	options.push_back(new MenuEntity(start_button, start_button_selected));
	options.push_back(new MenuEntity(exit_button, exit_button_selected));

	ranking_pos = -1;
	name = "";
	
	selected_option = 0;

	//TODO: SET THIS TO 0 WHEN MERGING
	stage = 0;

}


void GameOverStage::getRanking() {
	std::ifstream file("data/ranking.txt");
	std::string user, nights;

	sRankingInfo this_user;

	int user_idx = 0;
	int cur_user_nights = 0;

	if (!file) {
		std::cout << "Ranking does not exist. Creating the file..." << std::endl;
		std::ofstream createFile("data/ranking.txt");
		createFile.close(); 
	}
	else {
		std::cout << "Ranking exists. Opening the file for reading..." << std::endl;
		while ((file >> user >> nights) && (user_idx < NUM_RANKING)) {
			cur_user_nights = std::stoi(nights);

			if (ranking_pos == -1 && nights_survived >= cur_user_nights) {
				ranking_pos = user_idx;
				ranking[user_idx].user = "";
				ranking[user_idx].num_nights = nights_survived;

				user_idx++;

				if (user_idx >= NUM_RANKING)
					break;
			}

			ranking[user_idx].user = user;
			ranking[user_idx].num_nights = cur_user_nights;

			user_idx++;
		}

		if (user_idx < NUM_RANKING) {
			if (ranking_pos == -1) {
				ranking_pos = user_idx;
				ranking[user_idx].user = "";
				ranking[user_idx].num_nights = nights_survived;

				user_idx++;
			}

			for (user_idx; user_idx < NUM_RANKING; user_idx++) {
				ranking[user_idx].user = "";
				ranking[user_idx].num_nights = -1;
			}
		}
	}

	file.close();
}

void GameOverStage::updateRanking() {
	if (ranking_pos == -1)
		return;
	else {
		std::ofstream file("data/ranking.txt");

		if (file) {
			for (auto& info : ranking) {
				file << info.user << " " << info.num_nights << std::endl;
			}
		}
		else {
			std::cout << "Error opening the ranking." << std::endl;
			exit(-1);
		}

		file.close();
	}
}

void GameOverStage::onEnter()
{
	channel = Audio::Play("data/audio/gameover/gameover.wav", 0.3f, true);
	nights_survived = World::inst->number_nights;
	getRanking();
}

void GameOverStage::onExit()
{
	World::inst->resetWorld();
}

void GameOverStage::render()
{
	Vector3 white = Vector3(1.f);
	Vector3 yellow = Vector3(0.8, 0.8, 0.0);

	int offset_y = 50;
	int offset_x = 300;

	int num_nights = -1;

	switch (stage) {
		case 0:
			renderBackground(getEndingTexture());
			//Render number nights
			drawText(World::inst->window_width / 2.15f, World::inst->window_height / 4.1, std::to_string(nights_survived), Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height * 0.009);

			break;
		case 1:
			renderBackground(Texture::Get("data/gameover/enter_name.tga"));
			drawText(World::inst->window_width / 4, 3 * World::inst->window_height / 5, name, Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height * 0.01f);
			break;
		case 2:
			renderBackground(Texture::Get("data/gameover/bg.tga"));
			for (int i = 0; i < NUM_RANKING; i++) {
				num_nights = ranking[i].num_nights;
				drawText(World::inst->window_width / 4, World::inst->window_height / 6 + offset_y * i, std::to_string(i + 1) + ". ", ranking_pos == i ? yellow : white, World::inst->window_height * 0.004);

				if (num_nights > 0) {
					drawText(World::inst->window_width / 4 + offset_x / 2, World::inst->window_height / 6 + offset_y * i, ranking[i].user, ranking_pos == i ? yellow : white, World::inst->window_height * 0.004);
					drawText(World::inst->window_width / 4 + 3 * offset_x, World::inst->window_height / 6 + offset_y * i, std::to_string(num_nights), ranking_pos == i ? yellow : white, World::inst->window_height * 0.004);
				}
				else {
					drawText(World::inst->window_width / 4 + offset_x, World::inst->window_height / 6 + offset_y * i, "---", ranking_pos == i ? yellow : white, 3);
				}
			}
			renderButtons();
			break;
		default:
			break;
	}
}

Texture* GameOverStage::getEndingTexture() {
	if (nights_survived == 0 && ranking_pos != -1)
		return Texture::Get("data/gameover/bad_ending.tga");
	else if (ranking_pos < 3)
		return Texture::Get("data/gameover/good_ending.tga");
	else
		return Texture::Get("data/gameover/neutral_ending.tga");
}

void GameOverStage::renderBackground(Texture* texture) 
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", texture, 0);

	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glDisable(GL_BLEND);
}

void GameOverStage::renderButtons()
{
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
	{
		options[i]->render(selected_option == i, option_quads[i]);
	}
	glDisable(GL_BLEND);
}

void GameOverStage::update(float dt, bool transitioning)
{
	if (Input::gamepads[0].connected) {
		if (Input::wasButtonPressed(A_BUTTON))
		{
			switch (stage) {
			case 0:
				if (ranking_pos == -1)
					stage = 2;
				else
					stage = 1;
				break;
			case 1:
				stage++;
				break;
			case 2:
				selectOption();
				break;
			}
		}

		if (stage == 2) {
			if (Input::gamepads[0].didDirectionChanged(FLICK_LEFT))
				changeOption(-1, selected_option, OPTIONS_INTRO_MENU);

			else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))
				changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		}
	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_C))
		{
			switch (stage) {
				case 0:
					if (ranking_pos == -1)
						stage = 2;
					else
						stage = 1;
					break;
				case 1:
					stage++;
					break;
				case 2:
					selectOption();
					break;
			}
		}

		if (stage == 2) {
			if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT))
				changeOption(-1, selected_option, OPTIONS_INTRO_MENU);
			else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
				changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		}
	}
}

void GameOverStage::onKeyDown(SDL_KeyboardEvent event)
{
	const char* key_name;
	if (stage != 1)
		return;

	SDL_Keycode keyCode = event.keysym.sym;

	if (keyCode == SDLK_BACKSPACE) {
		if (!name.empty())
			name.pop_back();
	}
	else if (keyCode == SDLK_RETURN) {
		if (name.size()) {
			ranking[ranking_pos].user = name;
			updateRanking();
			stage++;
		}
		else {
			Audio::Play("data/audio/error.wav", 1.f, false);
		}
	}
	else if (name.size() >= MAX_NAME_SIZE)
		Audio::Play("data/audio/error.wav", 1.f, false);

	else {
		if (keyCode == SDLK_SPACE)
			name.push_back('_');
		else if ((keyCode >= SDLK_a && keyCode <= SDLK_z) ||
			(keyCode >= SDLK_0 && keyCode <= SDLK_9) ||
			(keyCode >= SDLK_KP_1 && keyCode <= SDLK_KP_9 &&
				!event.keysym.mod & KMOD_SHIFT))
		{
			const char* keyName = SDL_GetKeyName(keyCode);

			if (keyName != nullptr) {
				name.push_back(keyName[0]);
			}
		}
		else {
			Audio::Play("data/audio/error.wav", 1.f, false);
		}
	}


}

void  GameOverStage::resizeOptions(float width, float height)
{
	//TODO: ADAPT THIS TO THE NEW ASSETS
	float size_y = 100.f * height / 1080;
	float size_x = size_y * 350.f / 100.f;

	float offset = size_x / 1.7;

	option_quads[0]->createQuad(width / 2 - offset, size_y * 1.8, size_x, size_y, true);
	option_quads[1]->createQuad(width / 2 + offset, size_y * 1.8, size_x, size_y, true);
}


bool GameOverStage::selectOption()
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
