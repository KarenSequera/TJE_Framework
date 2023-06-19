#include "StageManager.h"
#include "input.h"
#include "camera.h"
#include "our_utils.h"
#include "game.h"
#include "audio.h"

#include <algorithm>
 
float angle = 0;
float mouse_speed = 100.0f;
HCHANNEL channel;
DayStage::DayStage() : Stage() {

	mouse_locked = true;
	gamepad_sensitivity = 0.05f;

	consumable_selected = BURGER;
	
	time_remaining = DAY_TIME;

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse


	//TODO: ADAPT THIS TO THE NEW ASSETS
	float size_x = (Game::instance->window_width)/2.5;
	float size_y = (size_x * 1000/3000);
	
	float position_x = Game::instance->window_width/2;
	float position_y = (position_x/2 * 1000/3000);

	HUD_quad.createQuad(position_x, position_y, size_x, size_y , true);

};

void DayStage::onEnter()
{
	// TODO: add the shield that has been left off from the night
	//udio::Init();
	//channel = Audio::Play("data/audio/intro.wav", 1.0, 1);
	camera->lookAt(Vector3(-1000.0f, 100.0f, 100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)); //position the camera and point to 0,0,0
	World::inst->player->position = camera->eye;
	World::inst->spawnerInit();
	time_remaining = DAY_TIME;


	
}

void DayStage::onExit()
{
	//bool a = Audio::Stop(channel);
	World::inst->clearItems();
	
}

void DayStage::render() {
	camera->lookAt(camera->eye, camera->eye + camera->front, camera->up);
	renderSky();
	for (auto& entity : World::inst->day_entities) {
		entity->render();
	}

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));

	glDisable(GL_DEPTH_TEST);
	renderHUD(shader);
	glEnable(GL_DEPTH_TEST);

	//drawText(5, 25, "HP: " + std::to_string(World::inst->player->health), Vector3(1.0f, 0.0f, 0.0f), 2);
	//drawText(5, 45, "HUNGER: " + std::to_string(World::inst->player->hunger), Vector3(1.0f, 0.75f, 0.0f), 2);
	//drawText(5, 65, "SHIELD: " + std::to_string(World::inst->player->shield), Vector3(0.75f, 0.75f, 0.75f), 2);
	//
	//renderConsumableMenu();
	#if DEBUG
	drawText(5, 400, "C: consume, F: getItem, J: hurt, K: get hunger, N: to night"
		, Vector3(0.0f, 0.5f, 0.75f), 2);
	#endif

}

//	Renders the consumable menu to screen, that is, the menu where the player chooses which item to consume
void DayStage::renderConsumableMenu() 
{
	drawText(5, 85, consumable_names[consumable_selected] + std::to_string(World::inst->getConsumableQuant(consumable_selected)), Vector3(0.0f, 0.5f, 0.75f), 2);
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


void DayStage::renderHUD(Shader* shader)
{
	// rendering the icons 
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
	shader->setUniform("u_texture", Texture::Get("data/hudDay/hud.tga"), 0);
	HUD_quad.render(GL_TRIANGLES);
	

	// Rendering the quantity of each consumable

	/*
	drawText( Game::instance->window_width*0.315, Game::instance->window_height - Game::instance->window_height*0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(3))), Vector3(1.0f, 1.0f, 1.0f), 2);

	drawText(Game::instance->window_width * 0.4, Game::instance->window_height - Game::instance->window_height * 0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(4))), Vector3(1.0f, 1.0f, 1.0f), 2);

	drawText(Game::instance->window_width * 0.48, Game::instance->window_height - Game::instance->window_height * 0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(5))), Vector3(1.0f, 1.0f, 1.0f), 2);

	drawText(Game::instance->window_width * 0.5555, Game::instance->window_height - Game::instance->window_height * 0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(0))), Vector3(1.0f, 1.0f, 1.0f), 2);

	drawText(Game::instance->window_width * 0.64, Game::instance->window_height - Game::instance->window_height * 0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(1))), Vector3(1.0f, 1.0f, 1.0f), 2);

	drawText(Game::instance->window_width * 0.72, Game::instance->window_height - Game::instance->window_height * 0.07,
		std::to_string(World::inst->getConsumableQuant(consumableType(2))), Vector3(1.0f, 1.0f, 1.0f), 2);*/

	// Rendering Health Bar
	Vector3 position = Vector3(Game::instance->window_width / 2.3, Game::instance->window_width / 8.3, 0);
	float width = Game::instance->window_width /7.5;
	float height = (width * 15 / 90);

	int total_health = MAX_HEALTH;
	int actual_health = (World::inst->player->health);
	float ratio = (float)actual_health / total_health;
	renderHealthBar(position, ratio, shader, width, height);

	// Rendering Hunger bar
	position = Vector3(Game::instance->window_width / 1.58, Game::instance->window_width / 8.3, 0);
	renderHungerBar(position, 0.5, shader, width, height);

	glDisable(GL_BLEND);

}


void DayStage::update(float dt) {
	time_remaining -= dt;
	if (time_remaining <= 0.f) 
	{
		StageManager::inst->changeStage("night");
		return;
	}
	updateMovement(dt);
	updateItemsAndStats();
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

	//example
	angle += (float)dt * 10.0f;

	/*Vector3 prueba = camera->getLocalVector(Vector3(0.0f, 0.0f, 1.0f));
	printf("%f %f %f\n", prueba.x, prueba.y, prueba.z);*/

	//We check if the gamepad is connected:
	if (Input::gamepads[0].connected) {

		// MOVEMENT
		// The orientation of the camara is conttrolled with the right joystick
		right_analog_x_disp = Input::gamepads[0].axis[RIGHT_ANALOG_X];
		right_analog_y_disp = Input::gamepads[0].axis[RIGHT_ANALOG_Y];

		left_analog_x_disp = Input::gamepads[0].axis[LEFT_ANALOG_X];
		left_analog_y_disp = Input::gamepads[0].axis[LEFT_ANALOG_Y];

		// We check in case the gamepad has slight drift. 
		if (std::abs(right_analog_x_disp) > DRIFT_THRESHOLD || std::abs(right_analog_y_disp) > DRIFT_THRESHOLD) {
			camera->ourRotate(right_analog_x_disp * gamepad_sensitivity, right_analog_y_disp * gamepad_sensitivity);
		}

		//The player moves with the left joystick, so when the left joystick is moved, we need to move the camera. 
		if (std::abs(left_analog_x_disp) > DRIFT_THRESHOLD)
			camera->moveXZ(Vector3(left_analog_x_disp, 0.f, 0.f) * -3.0f * speed);

		if (std::abs(left_analog_y_disp) > DRIFT_THRESHOLD) {
			camera->moveXZ(Vector3(0.f, 0.f, left_analog_y_disp) * -3.0f * speed);
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

		World::inst->player->velocity = World::inst->player->velocity + move_dir * speed;

		/*#if DEBUG
		printf("%f %f %f\n", World::inst->player->velocity.x, World::inst->player->velocity.y, World::inst->player->velocity.z);
		#endif*/


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
		//to navigate with the mouse fixed in the middle
		if (mouse_locked)
			Input::centerMouse();
	}
	
}

void DayStage::updateItemsAndStats() {
	if (Input::gamepads[0].connected)
	{
		// MENU
		if (Input::wasPadPressed(HATState::PAD_RIGHT))
		{
			consumable_selected = consumableType((consumable_selected + 1) % (NUM_CONSUMABLES - NUM_SHIELD_ITEMS));
		}
		else if (Input::wasPadPressed(HATState::PAD_LEFT))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHIELD_ITEMS)));
		}
		else if (Input::wasButtonPressed(X_BUTTON))
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
		#if DEBUG
		else if (Input::wasPadPressed(PAD_UP))
		{
			World::inst->hurtPlayer(KNIFE);
		}
		else if (Input::wasPadPressed(PAD_DOWN))
		{
			World::inst->consumeHunger(10);
		}
		else if (Input::wasButtonPressed(RIGHT_ANALOG_BUTTON))
		{
			World::inst->getConsumable(VEST);
		}
		#endif
	}
	else
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_Q))
		{
			consumable_selected = consumableType((consumable_selected + 1) % (NUM_CONSUMABLES - NUM_SHIELD_ITEMS));
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_E))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHIELD_ITEMS)));
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
#if DEBUG
		else if (Input::wasKeyPressed(SDL_SCANCODE_J))
			World::inst->hurtPlayer(KNIFE);

		else if (Input::wasKeyPressed(SDL_SCANCODE_K))
			World::inst->consumeHunger(10);

		else if (Input::wasKeyPressed(SDL_SCANCODE_N))
			StageManager::inst->changeStage("night");
		#endif
	}
}

void DayStage::resizeOptions(int width, int height) {

	float size_x = width / 2.5;
	float size_y = (size_x * 1000 / 3000);


	float position_x = width / 2;
	float position_y = (position_x / 2 * 1000 / 3000);

	HUD_quad.createQuad(position_x, position_y, size_x, size_y, true);
};