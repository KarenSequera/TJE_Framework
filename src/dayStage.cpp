#include "StageManager.h"
#include "input.h"
#include "camera.h"
#include "our_utils.h"
#include "game.h"

#include <algorithm>

float mouse_speed = 100.0f;
DayStage::DayStage() : Stage() {
	mouse_locked = true;
	gamepad_sensitivity = 0.05f;

	consumable_selected = BURGER;
	
	time_remaining = DAY_TIME + STAGE_TRANSITION_TIME;

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse

	float size_x = (Game::instance->window_width)/2.5;
	float size_y = (size_x * 1000/3000);
	
	float position_x = Game::instance->window_width/2;
	float position_y = (position_x/2 * 1000/3000);

	HUD_quad.createQuad(position_x, position_y, size_x, size_y , true);

	size_x = (Game::instance->window_width)/1.8;
	size_y = (size_x * 1000 / 4000);
	position_x = Game::instance->window_width / 2;
	position_y = (position_x / 1.4 * 1000 / 4000);

	instructions_quad.createQuad(position_x, position_y, size_x, size_y, true);

	post_fx = POST_FX;
	if (post_fx)
		fx_shader = Shader::Get("data/shaders/screen.vs", "data/shaders/postfx.fs");
	else
		fx_shader = nullptr;

	num_slides = TUT_SLIDES_DAY;
	getSlides();
};

void DayStage::onEnter()
{
	channel = Audio::Play("data/audio/day/day.wav", 0.1f, true);
	
	World::inst->clearItems();
	camera->lookAt(Vector3(-50.f, 100.0f, -1600.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)); //position the camera and point to 0,0,0
	World::inst->player->position = camera->eye;
	World::inst->spawnerInit();
	time_remaining = DAY_TIME;

	if (World::inst->triggerTutorial) {
		Audio::PlayDelayed("data/audio/messages/appear.wav", 1.f, 0.6f, 0, 0.f);
		in_tutorial = true;
	}
}

void DayStage::onExit()
{
}

void DayStage::render() {

	camera->lookAt(camera->eye, camera->eye + camera->front, camera->up);
	
	if (post_fx) {
		renderTarget->enable();

		renderSky();

		for (auto& entity : World::inst->day_entities) {
			entity->render();
		}

		renderTarget->disable();
		glDisable(GL_DEPTH_TEST);
		renderTarget->ourToViewport(Vector3(in_tutorial ? 1.f : 0.f, 1.f, 1.f), fx_shader);
		glEnable(GL_DEPTH_TEST);
	}
	else
	{
		renderSky();

		for (auto& entity : World::inst->day_entities) {
			entity->render();
		}
	}

	if (in_tutorial)
		renderTutorial();
	else
	{
		glDisable(GL_DEPTH_TEST);
		renderHUD();
		glEnable(GL_DEPTH_TEST);
	}
	

}

void DayStage::renderSky() 
{
	Matrix44 model;
	model.setTranslation(camera->eye.x, camera->eye.y, camera->eye.z);
	glDisable(GL_DEPTH_TEST);
	Mesh* cubemap = Mesh::Get("data/cubemap/cubemap.ASE");
	Shader* shader = Shader::Get("data/shaders/basic.vs","data/shaders/cubemap.fs");
	shader->enable();
	shader->setUniform("u_model", model);
	shader->setUniform("u_viewprojection", camera->viewprojection_matrix);
	shader->setUniform("u_camera_position", camera->eye);
	shader->setUniform("u_sky_texture", World::inst->cubemap, 0);
	cubemap->render(GL_TRIANGLES);

	shader->disable();
	glEnable(GL_DEPTH_TEST);

}


void DayStage::renderHUD()
{

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", Texture::Get("data/hudDay/hud.tga"), 0);
	shader->setUniform("u_animated", false);

	// rendering the icons 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	
	HUD_quad.render(GL_TRIANGLES);
	shader->setUniform("u_texture", Texture::Get("data/hudDay/hud2.tga"), 0);
	shader->setUniform("u_animated", false);
	instructions_quad.render(GL_TRIANGLES);

	// Rendering Health Bar
	Vector3 position = Vector3(Game::instance->window_width / 2.3, Game::instance->window_width / 8.3, 0);
	float width = Game::instance->window_width /7.5;
	float height = (width * 15 / 90);
	
	renderBar(position,
		(float)(World::inst->player->health) / MAX_HEALTH , shader, width, height,
		Texture::Get("data/NightTextures/redTexture.tga"), Texture::Get("data/NightTextures/greenTexture.tga"));

	// Rendering Hunger bar
	position = Vector3(Game::instance->window_width / 1.58, Game::instance->window_width / 8.3, 0);
	
	renderBar(position, (float)(World::inst->player->hunger)/MAX_HUNGER, shader, width, height,
		Texture::Get("data/NightTextures/grayTexture.tga"), Texture::Get("data/NightTextures/orangeTexture.tga"));
	shader->disable();


	//Rendering selected option
	Shader* shader_selected = Shader::Get("data/shaders/quad.vs", "data/shaders/hud.fs");
	shader_selected->enable();
	shader_selected->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader_selected->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader_selected->setUniform("u_texture", Texture::Get("data/hudDay/hud_selected.tga"), 0);
	shader_selected->setUniform("u_ratio", 1.f/6.f);
	shader_selected->setUniform("u_selected", (consumable_selected));

	HUD_quad.render(GL_TRIANGLES);
	shader_selected->disable();

	float x = Game::instance->window_width / 2.87;
	float y = Game::instance->window_height - (Game::instance->window_width / 8 * 1000 / 3000);

	float offset = Game::instance->window_width /15;

	for (int i = 0; i < 6; i++) {
		drawText(x + offset * i, y,
			std::to_string(World::inst->getConsumableQuant(consumableType(i))), Vector3(1.0f, 1.0f, 1.0f), Game::instance->window_height * 0.003);

	}
	
	glDisable(GL_BLEND);

}


void DayStage::update(float dt, bool transitioning) {
	if (!transitioning) {
		if (in_tutorial) {
			updateTutorial();
		}
		else {
			if (Input::wasButtonPressed(Y_BUTTON) || Input::wasKeyPressed(SDL_SCANCODE_P)) {
				Audio::Play("data/audio/messages/appear.wav", 1.f, false);
				World::inst->frozen = !World::inst->frozen;
			}
			
			if (!World::inst->frozen) {
				if (shouldTrigger(time_remaining, dt))
					StageManager::inst->changeStage("night");
				updateMovement(dt);
				updateItemsAndStats();
			}
		}
	}
}

float right_analog_x_disp;
float right_analog_y_disp;
float left_analog_x_disp;
float left_analog_y_disp;
float speed = 500.f;
Vector3 move_dir = Vector3(0.0);
std::vector<sCollisionData> collisions;

int collided = 0;

void DayStage::updateMovement(float dt){
	collisions.clear();

	//We check if the gamepad is connected:
	if (Input::gamepads[0].connected) {

		// MOVEMENT
		// The orientation of the camara is conttrolled with the right joystick
		right_analog_x_disp = -1.f * Input::gamepads[0].axis[RIGHT_ANALOG_X];
		right_analog_y_disp = -1.f * Input::gamepads[0].axis[RIGHT_ANALOG_Y];

		left_analog_x_disp = Input::gamepads[0].axis[LEFT_ANALOG_X];
		left_analog_y_disp = Input::gamepads[0].axis[LEFT_ANALOG_Y];

		// We check in case the gamepad has slight drift. 
		if (std::abs(right_analog_x_disp) > DRIFT_THRESHOLD || std::abs(right_analog_y_disp) > DRIFT_THRESHOLD) {
			camera->ourRotate(right_analog_x_disp * gamepad_sensitivity, right_analog_y_disp * gamepad_sensitivity);
		}

		//The player moves with the left joystick, so when the left joystick is moved, we need to move the camera. 
		if (std::abs(left_analog_x_disp) > DRIFT_THRESHOLD) {
			int sign = left_analog_x_disp > 0.f ? -1.f : 1.f;
			move_dir = move_dir + sign * camera->v_right;
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (std::abs(left_analog_y_disp) > DRIFT_THRESHOLD) {
			int sign = left_analog_y_disp > 0.f ? -1.f : 1.f;
			move_dir = move_dir + sign * camera->front;
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}
	}
	else {
		//mouse input to rotate the cam
		camera->ourRotate(Input::mouse_delta.x * 0.005f, Input::mouse_delta.y * 0.005f);

		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))
		{
			move_dir = move_dir + camera->front;//Vector3(0.0f, 0.0f, 1.0f);
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
		{
			move_dir = move_dir - camera->front;
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT))
		{
			move_dir = move_dir + camera->v_right;
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT))
		{
			move_dir = move_dir - camera->v_right;
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		//to navigate with the mouse fixed in the middle
		if (mouse_locked)
			Input::centerMouse();
	}

	World::inst->player->velocity = World::inst->player->velocity + move_dir * speed;

	if (World::inst->checkPlayerCollisions(World::inst->player->position + World::inst->player->velocity * dt, &collisions) == 1)
	{
		Vector3 new_dir;
		Vector3 velocity;

		sCollisionData collision = collisions[0];

		velocity = World::inst->player->velocity;

		new_dir = World::inst->player->velocity.dot(collision.colNormal);

		new_dir = new_dir * collision.colNormal;
		World::inst->player->velocity.x = 0;
		World::inst->player->velocity.z = 0;
	}
	World::inst->player->velocity.y = 0.f;

	World::inst->player->position = World::inst->player->position + World::inst->player->velocity * dt;
	World::inst->player->model_matrix.setTranslation(World::inst->player->position.x, World::inst->player->position.y, World::inst->player->position.z);

	World::inst->player->velocity *= 0.15f;
	camera->eye = World::inst->player->position;

	move_dir *= 0.15f;
	
}

void DayStage::updateItemsAndStats() {
	if (Input::gamepads[0].connected)
	{
		// MENU
		if (Input::wasButtonPressed(RB_BUTTON))
		{
			consumable_selected = consumableType((consumable_selected + 1) % (NUM_CONSUMABLES - NUM_SHIELD_ITEMS));
		}
		else if (Input::wasButtonPressed(LB_BUTTON))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHIELD_ITEMS)));
		}
		else if (Input::wasButtonPressed(A_BUTTON))
		{
			int res = World::inst->useConsumable(consumable_selected);
			switch (res) {
			case 1:
				//TODO: ERROR MSG - NO CONSUMABLES OF THAT TYPE
				printf("NO CONSUMABLES OF THAT TYPE\n");
				break;

			case 2:
				//TODO: ERROR MSG - STAT ALREADY CAPPED
				printf("STAT ALREADY CAPPED\n");
				break;

			default:
				break;
			}
		}
	}
	else
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_Q))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHIELD_ITEMS)));
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_E))
		{
			consumable_selected = consumableType(ourMod((consumable_selected + 1), (NUM_CONSUMABLES - NUM_SHIELD_ITEMS)));
		}

		// use consumable
		else if (Input::wasKeyPressed(SDL_SCANCODE_C))
		{
			int res = World::inst->useConsumable(consumable_selected);
			switch (res) {
			case 1:
				//TODO: ERROR MSG - NO CONSUMABLES OF THAT TYPE
#if DEBUG
				printf("NO CONSUMABLES OF THAT TYPE\n");
#endif
				break;

			case 2:
				//TODO: ERROR MSG - STAT ALREADY CAPPED
#if DEBUG
				printf("STAT ALREADY CAPPED\n");
#endif
				break;

			default:
				break;
			}
		}
	}
#if DEBUG
	if (Input::wasKeyPressed(SDL_SCANCODE_J))
		World::inst->hurtPlayer(KNIFE);

	else if (Input::wasKeyPressed(SDL_SCANCODE_K))
		World::inst->consumeHunger(10);

	else if (Input::wasKeyPressed(SDL_SCANCODE_N))
		StageManager::inst->changeStage("night");
	else if (Input::wasKeyPressed(SDL_SCANCODE_L))
		printf("%f %f %f\n", World::inst->player->position.x, World::inst->player->position.y, World::inst->player->position.z);
#endif
}

void DayStage::getSlides() {
	slides.resize(TUT_SLIDES_DAY);

	for (int i = 0; i < TUT_SLIDES_DAY; i++)
		slides[i] = Texture::Get(("data/quad_textures/tutorial/day" + std::to_string(i + 1) + ".tga").c_str());
}

void DayStage::resizeOptions(float width, float height) {

	float size_x = width / 2.5;
	float size_y = (size_x * 1000 / 3000);


	float position_x = width / 2;
	float position_y = (position_x / 2 * 1000 / 3000);

	HUD_quad.createQuad(position_x, position_y, size_x, size_y, true);

	size_x = (Game::instance->window_width) / 1.8;
	size_y = (size_x * 1000 / 4000);
	position_x = width / 2;
	position_y = (position_x / 1.4 * 1000 / 4000);

	instructions_quad.createQuad(position_x, position_y, size_x, size_y, true);
};