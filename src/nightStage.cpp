#include "StageManager.h"
#include "input.h"
#include "camera.h"
#include "our_utils.h"
#include "game.h"

#include <algorithm>
 
NightStage::NightStage() : Stage()
{
	mouse_locked = true;

	is_player_turn = true;

	selected_target = 0;
	turns_to_day = 0;

	free_cam_enabled = false;
	n_angle = 0.f;
	
	zombie_attacking = 0;

	time_between_turns = TIME_BTW_TURNS;
	to_day = false;

	fx_shader = Shader::Get("data/shaders/screen.vs", "data/shaders/postfx.fs");
	error_shader = Shader::Get("data/shaders/screen.vs", "data/shaders/error.fs");


	resizeOptions(World::inst->window_width, World::inst->window_height);

	in_tutorial = false;
	num_slides = TUT_SLIDES_NIGHT;
	getSlides();
}

void NightStage::onEnter() {
	
	channel = Audio::Play("data/audio/night/night.wav", 0.05f, true);

	// position the player
	World::inst->player->model_matrix = World::inst->night_models[2];

	// generate zombies
	World::inst->generateZombies(World::inst->number_nights);

	// reset some variables
	is_player_turn = true;
	selected_target = 0;
	zombie_attacking = 0;
	time_between_turns = TIME_BTW_TURNS;

	//TODO: adjust formula so that it is enjoyable
	turns_to_day = 10 + World::inst->number_nights;

	Camera::current->lookAt(World::inst->night_models[0].getTranslation(), World::inst->night_models[1].getTranslation(), Vector3(0.0f, 1.0f, 0.0f));
	camera->Camera::current;

	if (World::inst->triggerTutorial) {
		Audio::PlayDelayed("data/audio/messages/appear.wav", 1.f, 0.75f, 0, 0.f);
		in_tutorial = true;
	}
	std::cout << World::inst->number_nights;
}

void NightStage::onExit()
{
	World::inst->triggerTutorial = false;
}


void NightStage::render()
{
	if (World::inst->error)
	{
		Shader* shader;

		renderTarget->enable();

		shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
		shader->enable();
		shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
		shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));

		renderBackground(shader);

		shader->disable();
		// render what must be rendered always
		World::inst->renderNight();
		renderTarget->disable();

		errorTarget->enable();

		glDisable(GL_DEPTH_TEST);
		renderTarget->ourToViewport(Vector3(in_tutorial || World::inst->frozen ? 1.f : 0.f, 1.f, 1.f), fx_shader);
		glEnable(GL_DEPTH_TEST);


		if (in_tutorial)
		{
			renderTutorial();
		}
		else
		{
			shader->enable();

			renderHealthBars(shader);
			renderPlayerStats(shader);

			shader->disable();


			if (World::inst->ready_to_attack) {
				shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
				shader->enable();
				shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
				shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
				renderCrosshair(shader);
				shader->disable();
			}

			if (is_player_turn && World::inst->idle)
				playerTurnRender();
		}

		errorTarget->disable();

		glDisable(GL_DEPTH_TEST);
		errorTarget->toViewport(error_shader);
		glEnable(GL_DEPTH_TEST);

		World::inst->user_error();
	}
	else {
		Shader* shader;

		renderTarget->enable();

		shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
		shader->enable();
		shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
		shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));

		renderBackground(shader);

		shader->disable();
		// render what must be rendered always
		World::inst->renderNight();
		renderTarget->disable();

		glDisable(GL_DEPTH_TEST);
		renderTarget->ourToViewport(Vector3(in_tutorial || World::inst->frozen ? 1.f : 0.f, 1.f, 1.f), fx_shader);
		glEnable(GL_DEPTH_TEST);

		if (in_tutorial)
		{
			renderTutorial();
		}
		else
		{
			shader->enable();

			renderHealthBars(shader);
			renderPlayerStats(shader);

			shader->disable();


			if (World::inst->ready_to_attack) {
				shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
				shader->enable();
				shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
				shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
				renderCrosshair(shader);
				shader->disable();
			}

			if (is_player_turn && World::inst->idle)
				playerTurnRender();
		}
	}
	
}

void NightStage::renderCrosshair(Shader* shader)
{
	float anim_speed = 7.f;

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ZombieEntity* selected = World::inst->waves[World::inst->cur_wave][selected_target];
	Matrix44 model = selected->getBoneMatrix("mixamorig_Spine") * selected->model_matrix;

	Vector3 position = model.getTranslation();
	position.x -= 10.f;
	position.y += 10.f;
	position = camera->project(position, Game::instance->window_width, Game::instance->window_height);

	Mesh quad;
	quad.createQuad(position.x, position.y, 65.f, 65.f, true);

	shader->setUniform("u_animated", true);
	shader->setUniform("u_texture",Texture::Get("data/quad_textures/night/crosshair_anim.tga"), 0);
	shader->setUniform("u_ratio", 1.f / (float) NUM_STATES_CROSSHAIR);
	shader->setUniform("u_state", int(Game::instance->time * anim_speed) % NUM_STATES_CROSSHAIR);

	quad.render(GL_TRIANGLES);
	glDisable(GL_BLEND);
}


void NightStage::renderHealthBars(Shader* shader)
{
	glDisable(GL_DEPTH_TEST);
	Matrix44 model;
	Vector3 position;
	int total_health;
	int actual_health;
	float ratio;

	//Health bar of the zombies
	for (auto& zombie : World::inst->waves[World::inst->cur_wave]) {

		model = zombie->model_matrix;

		position = model.getTranslation();
		position.y -= 15.f;
		position = camera->project(position, Game::instance->window_width, Game::instance->window_height);

		total_health = zombie->info.max_health;
		actual_health = max(0.f, zombie->info.health);

		ratio = (float)actual_health / total_health;

		renderBar(position, ratio, shader, HEALTH_BAR_WIDTH, HEALTH_BAR_HEIGHT,
			Texture::Get("data/quad_textures/night/redTexture.tga"), Texture::Get("data/quad_textures/night/greenTexture.tga"));
	}
	glEnable(GL_DEPTH_TEST);
}

void NightStage::playerTurnRender() {
	// Render menus -> prep options
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//menu render
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	World::inst->cur_menu->render(World::inst->selected_option);

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_animated", false);
	shader->setUniform("u_texture", Texture::Get("data/quad_textures/night/turns_left.tga"), 0);

	World::inst->remaining_info_quad.render(GL_TRIANGLES);

	shader->disable();

	drawText(World::inst->window_width * 0.95, World::inst->window_height - World::inst->window_height * 0.8105, std::to_string(turns_to_day),
		Vector3(1.0f, 1.0f, 1.0f), World::inst->window_height * 0.0035);

	// Render menus -> prep options
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void NightStage::renderBackground(Shader* shader)
{
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/night/background.tga"), 0);
	shader->setUniform("u_animated", false);

	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void NightStage::renderPlayerStats(Shader* shader)
{
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/night/hud_night.tga"), 0);
	night_hud.render(GL_TRIANGLES);


	Vector3 position = Vector3(Game::instance->window_width * 0.2, Game::instance->window_height * 0.86, 0);
	float width = Game::instance->window_height / 3;
	float height = (width * 15 / 90);

	renderBar(position,
		(float)(World::inst->player->health) / MAX_HEALTH, shader, width, height,
		Texture::Get("data/quad_textures/night/redTexture.tga"), Texture::Get("data/quad_textures/night/greenTexture.tga"));

	position = Vector3(Game::instance->window_width * 0.2, Game::instance->window_height * 0.75, 0);

	renderBar(position, (float)(World::inst->player->hunger) / MAX_HUNGER, shader, width, height,
		Texture::Get("data/quad_textures/night/grayTexture.tga"), Texture::Get("data/quad_textures/night/orangeTexture.tga"));

	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
}

void NightStage::update(float dt, bool transitioning)
{
	if (!transitioning) {
		if (in_tutorial) {
			updateTutorial();
		}
		else {
			if (World::inst->zombiesAlive() <= 0)
				if (World::inst->nextWave())
					StageManager::inst->changeStage("day");

			if (Input::wasButtonPressed(Y_BUTTON) || Input::wasKeyPressed(SDL_SCANCODE_P)) {
				Audio::Play("data/audio/messages/appear.wav", 1.f, false);
				World::inst->frozen = !World::inst->frozen;
			}
#if DEBUG
			else if (Input::wasKeyPressed(SDL_SCANCODE_N))
				StageManager::inst->changeStage("day");

			else if (Input::wasKeyPressed(SDL_SCANCODE_U))
				World::inst->unlimited_everything = !World::inst->unlimited_everything;
			else if (Input::wasKeyPressed(SDL_SCANCODE_G))
			{
				if (free_cam_enabled)
					Camera::current->lookAt(World::inst->night_models[0].getTranslation(), World::inst->night_models[1].getTranslation(), Vector3(0.0f, 1.0f, 0.0f));
				free_cam_enabled = !free_cam_enabled;
			}

			if (free_cam_enabled)
				cameraUpdate(dt);

			else if (!World::inst->frozen && World::inst->idle)
			{
				if (is_player_turn)
					playerTurnUpdate(dt);
				else
					zombieTurnUpdate(dt);
			}

			if (!World::inst->isPlayerAlive()) {
				if (shouldTrigger(World::inst->player->time_til_death, dt))
					StageManager::inst->changeStage("game over");
			}

#else
			if (!World::inst->frozen World::inst->idle)
			{
				if (is_player_turn)
					playerTurnUpdate(dt);
				else
					zombieTurnUpdate(dt);
			}

			if (!World::inst->isPlayerAlive()) {
				if (shouldTrigger(World::inst->player->time_til_death, dt))
					StageManager::inst->changeStage("game over");
			}
#endif
		}
	}
	World::inst->updateAnimations(dt);;
}

void NightStage::playerTurnUpdate(float dt)
{

	if(to_day)
		StageManager::inst->changeStage("day");

	// if null pointer, the user has chosen to attack
	if (World::inst->ready_to_attack)
	{
		if (Input::gamepads[0].connected) {

			if (Input::gamepads[0].didDirectionChanged(FLICK_LEFT)) {
				Audio::Play("data/audio/menu/change_option.wav", 1.f, false);
				selected_target = ourMod(selected_target - 1, World::inst->zombiesAlive());
			}

			else if (Input::gamepads[0].didDirectionChanged(FLICK_RIGHT)) {
				selected_target = ourMod(selected_target + 1, World::inst->zombiesAlive());
				Audio::Play("data/audio/menu/change_option.wav", 1.f, false);
			}
			else if (Input::wasButtonPressed(A_BUTTON)) {
				int result = World::inst->hurtZombie(selected_target);

				// if the attack is not super effective then we move onto the zombie's turn
				if (result != 2)
					is_player_turn = false;
				

				// otherwise we give the player another action
				// TODO: message of super efective, perhaps -> with a bool and a specific UI element for it, for example
				World::inst->ready_to_attack = false;
				World::inst->changeMenu("general");
				selected_target = 0;
				turns_to_day--;
				World::inst->consumeHunger(5);
			}
			else if (Input::wasButtonPressed(B_BUTTON))
			{
				Audio::Play("data/audio/menu/go_back.wav", 1.f, false);

				World::inst->ready_to_attack = false;
				World::inst->playerToState(IDLE, TRANSITION_TIME / 2.f);
			}
			
			
		}
		else {
			if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT)) {
				selected_target = ourMod(selected_target - 1, World::inst->zombiesAlive());
				Audio::Play("data/audio/menu/change_option.wav", 1.f, false);
			}

			else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT)){
				selected_target = ourMod(selected_target + 1, World::inst->zombiesAlive());
				Audio::Play("data/audio/menu/change_option.wav", 1.f, false);
			}
			else if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
				int result = World::inst->hurtZombie(selected_target);

				// if the attack is not super effective then we move onto the zombie's turn
				if (result != 2)
					is_player_turn = false;

				// otherwise we give the player another action
				// TODO: message of super efective, perhaps -> with a bool and a specific UI element for it, for example
				World::inst->ready_to_attack = false;
				World::inst->changeMenu("general");
				selected_target = 0;
				turns_to_day--;
				World::inst->consumeHunger(5);
			}
			else if (Input::wasKeyPressed(SDL_SCANCODE_Z))
			{
				Audio::Play("data/audio/menu/go_back.wav", 1.f, false);
				World::inst->ready_to_attack = false;
				World::inst->playerToState(IDLE, TRANSITION_TIME / 2.f);
			}
		}
		
	}
	else
	{
		if (Input::gamepads[0].connected) {

			if (Input::gamepads[0].didDirectionChanged(FLICK_UP))
				World::inst->changeOption(-1);

			else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))
				World::inst->changeOption(1);

			else if (Input::wasButtonPressed(A_BUTTON))
			{
				if (World::inst->selectOption())
					is_player_turn = false;
			}
			else if (Input::wasButtonPressed(B_BUTTON)) {
				Audio::Play("data/audio/menu/go_back.wav", 1.f, false);
				World::inst->changeMenu("general");
			}
		}
		else {
			if (Input::wasKeyPressed(SDL_SCANCODE_W) || Input::wasKeyPressed(SDL_SCANCODE_UP))
				World::inst->changeOption(-1);

			else if (Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasKeyPressed(SDL_SCANCODE_DOWN))
				World::inst->changeOption(1);

			else if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
				if (World::inst->selectOption())
					is_player_turn = false;
			}

			else if (Input::wasKeyPressed(SDL_SCANCODE_Z)) {
				Audio::Play("data/audio/menu/go_back.wav", 1.f, false);
				World::inst->changeMenu("general");
			}
		}
		
	}

	#if DEBUG	
	if (Input::wasKeyPressed(SDL_SCANCODE_J))
		World::inst->hurtPlayer(20);

	else if (Input::wasKeyPressed(SDL_SCANCODE_K))
		World::inst->consumeHunger(10);
	#endif
};

void NightStage::zombieTurnUpdate(float dt)
{
	time_between_turns -= dt;

	if (time_between_turns <= 0.f)
	{
		if (World::inst->attackPlayer(zombie_attacking))
		{
			zombie_attacking++;
			if (zombie_attacking >= World::inst->waves[World::inst->cur_wave].size())
			{
				is_player_turn = true;
				newTurn();
			}
		}
	}

}

void NightStage::cameraUpdate(float dt)
{
	float speed = dt * 1000.f; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	n_angle += (float)dt * 10.0f;

	//mouse input to rotate the cam
	if ((Input::mouse_state & SDL_BUTTON_LEFT) || mouse_locked) //is left button pressed?
	{
		camera->rotate(Input::mouse_delta.x * 0.005f, Vector3(0.0f, -1.0f, 0.0f));
		camera->rotate(Input::mouse_delta.y * 0.005f, camera->getLocalVector(Vector3(-1.0f, 0.0f, 0.0f)));
	}

	//async input to move the camera around
	if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP)) camera->move(Vector3(0.0f, 0.0f, 1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN)) camera->move(Vector3(0.0f, 0.0f, -1.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT)) camera->move(Vector3(1.0f, 0.0f, 0.0f) * speed);
	if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT)) camera->move(Vector3(-1.0f, 0.0f, 0.0f) * speed);

	//to navigate with the mouse fixed in the middle
	if(mouse_locked)
		Input::centerMouse();
}

void NightStage::getSlides(){
	slides.resize(TUT_SLIDES_NIGHT);

	for (int i = 0; i < TUT_SLIDES_NIGHT; i++)
		slides[i] = Texture::Get(("data/quad_textures/tutorial/night" + std::to_string(i + 1) + ".tga").c_str());

}

void NightStage::newTurn() 
{
	World::inst->changeMenu("general");

	turns_to_day--;
	
	if (turns_to_day == 0)
		StageManager::inst->changeStage("day");
		
	//TODO: Make a variable that changes depending on the number of nights, the higher the night the more it takes.
	//World::inst->consumeHunger(10);
	World::inst->playerDefenseOff();

	zombie_attacking = 0;
	selected_target = 0;
	time_between_turns = TIME_BTW_TURNS;
}

void NightStage::resizeOptions(float width, float height) {

	float size_y = height / 3;
	float size_x = size_y * 1700 / 834;

	night_hud.createQuad(width * 0.18, height * 0.8, size_x, size_y, true);

	World::inst->resizeOptions(width, height);
}