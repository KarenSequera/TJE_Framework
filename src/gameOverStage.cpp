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
	
}

void GameOverStage::onEnter()
{
	nights_survived = World::inst->number_nights;
	selected_option = 0;
	result = updateScores(World::inst->number_nights);
}

void GameOverStage::onExit()
{
	World::inst->resetWorld();
}

void GameOverStage::render()
{
	renderNumNights();
	renderButtons();
}


void GameOverStage::renderNumNights() 
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

	if (nights_survived == 1 && nights_survived != result.maximumScore) {
		shader->setUniform("u_texture", Texture::Get("data/gameover/bad_ending.tga"), 0);
		World::inst->fullscreen_quad.render(GL_TRIANGLES);
	}
	else if (nights_survived == result.maximumScore) {
		shader->setUniform("u_texture", Texture::Get("data/gameover/record_ending.tga"), 0);
		World::inst->fullscreen_quad.render(GL_TRIANGLES);
	}
	else if(result.isInTopThree) {
		shader->setUniform("u_texture", Texture::Get("data/gameover/good_ending.tga"), 0);
		World::inst->fullscreen_quad.render(GL_TRIANGLES);
	}
	else{
		shader->setUniform("u_texture", Texture::Get("data/gameover/neutral_ending.tga"), 0);
		World::inst->fullscreen_quad.render(GL_TRIANGLES);
	}
	//Render number nights and maximum score 
	drawText(World::inst->window_width / 2.05, World::inst->window_height / 4.1, std::to_string(World::inst->number_nights), Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height*0.009);

	//TODO: IF RANKING DELETE
	drawText(World::inst->window_width / 4, World::inst->window_height / 2,  std::to_string(result.maximumScore), Vector3(1.0f, 1.0f, 1.0f), 2);

}

void GameOverStage::renderButtons()
{
	for (int i = 0; i < OPTIONS_INTRO_MENU; i++)
	{
		options[i]->render(selected_option == i, option_quads[i]);
	}
}

void GameOverStage::update(float dt, bool transitioning)
{
	if (Input::gamepads[0].connected) {
		if (Input::gamepads[0].didDirectionChanged(FLICK_LEFT))
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);

		else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))

			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasButtonPressed(A_BUTTON))
			selectOption();

	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT))
			changeOption(-1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
			changeOption(1, selected_option, OPTIONS_INTRO_MENU);
		else if (Input::wasKeyPressed(SDL_SCANCODE_C))
			selectOption();
	}
}


void  GameOverStage::resizeOptions(float width, float height)
{
	//TODO: ADAPT THIS TO THE NEW ASSETS
	float size_y = 100.f * height / 1080;
	float size_x = size_y * 350.f / 100.f;

	float offset = size_x / 1.7;

	option_uses_pos[0] = Vector2(width / 2 - offset, size_y * 1.8);
	option_uses_pos[1] = Vector2(width / 2 + offset, size_y * 1.8);

	option_quads[0]->createQuad(option_uses_pos[0].x, option_uses_pos[0].y, size_x, size_y, true);
	option_quads[1]->createQuad(option_uses_pos[1].x, option_uses_pos[1].y, size_x, size_y, true);

	for (int i = 0; i < 2; i++)
	{
		option_uses_pos[i].x += 0.1 * width;
		option_uses_pos[i].y = height - option_uses_pos[i].y + 10;
	}

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

ScoreUpdateResult GameOverStage::updateScores(int number_nights)
{
	ScoreUpdateResult result;

	// Open the file for input
	std::ifstream inputFile("data/gameover/runs.txt");

	std::vector<int> scores(3, 0);  // Vector to store the top 3 runs

	// Read the runs from the file
	for (int i = 0; i < 3; i++) {
		inputFile >> scores[i];
	}

	inputFile.close();

	// The runs are sorted in ascending order, TOP 3 is in the first line

	// If the number of nights of the current run is greater than the top 3, it enters the ranking
	if (number_nights > scores[0])
	{
		scores[0] = number_nights;
		std::sort(scores.begin(), scores.end());

		// Open the file for output
		std::ofstream outputFile("data/gameover/runs.txt");

		// Now we write the updated scores to the file
		for (int i = 0; i < 3; i++) {
			outputFile << scores[i] << '\n';
		}

		outputFile.close();

		result.isInTopThree = true;
	}
	else {
		result.isInTopThree = false;
	}

	// Get the maximum score
	result.maximumScore = *std::max_element(scores.begin(), scores.end());
	// We return the top 3 scores.
	return result;
}