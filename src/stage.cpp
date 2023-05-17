#include "stage.h"
#include "input.h"
#include "camera.h"
#include "our_utils.h"
#include "game.h"

#include <algorithm>
 
float angle = 0;
float mouse_speed = 100.0f;

Stage::Stage() {
	camera = Camera::current;
	mouse_locked = false;
}

DayStage::DayStage() : Stage() {

	mouse_locked = true;
	gamepad_sensitivity = 0.05f;

	consumable_selected = BURGER;

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse
};

float right_analog_x_disp;
float right_analog_y_disp;
float left_analog_x_disp;
float left_analog_y_disp;
float speed;
vec3 move_dir = vec3(0.0);

void DayStage::render() {
	World::inst->player->position = Camera::current->eye;

	for (auto& entity : World::inst->day_entities) {
		entity->render();
	}
	drawText(5, 25, "HP: " + std::to_string(World::inst->player->health), Vector3(1.0f, 0.0f, 0.0f), 2);
	drawText(5, 45, "HUNGER: " + std::to_string(World::inst->player->hunger), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 65, "SHIELD: " + std::to_string(World::inst->player->shield), Vector3(0.75f, 0.75f, 0.75f), 2);
	
	renderConsumableMenu();
	#if DEBUG
	drawText(5, 505, "C: consume, F: getItem, J: hurt, K: get hunger, P: getCons"
		, Vector3(0.0f, 0.5f, 0.75f), 2);
	#endif
}

//	Renders the consumable menu to screen, that is, the menu where the player chooses which item to consume
void DayStage::renderConsumableMenu() {
	drawText(5, 85, consumable_names[consumable_selected] + std::to_string(World::inst->getConsumableQuant(consumable_selected)), Vector3(0.0f, 0.5f, 0.75f), 2);
}

void DayStage::update(float dt) {
	updateMovement(dt);
	updateItemsAndStats();
}

void DayStage::updateMovement(float dt){
	speed = dt * mouse_speed * 550.f; //the speed is defined by the seconds_elapsed so it goes constant

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
			move_dir = move_dir + Vector3(0.0f, 0.0f, 1.0f);
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
		{
			move_dir = move_dir + Vector3(0.0f, 0.0f, -1.0f);
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT))
		{
			move_dir = move_dir + Vector3(1.0f, 0.0f, 0.0f);
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT))
		{
			move_dir = move_dir + Vector3(-1.0f, 0.0f, 0.0f);
			if (move_dir.length() > 0.01)
				move_dir.normalize();
		}

		World::inst->player->velocity = World::inst->player->velocity + move_dir * speed;

		/*#if DEBUG
		printf("%f %f %f\n", World::inst->player->velocity.x, World::inst->player->velocity.y, World::inst->player->velocity.z);
		#endif*/

		camera->moveXZ(World::inst->player->velocity * dt);

		World::inst->player->velocity *= 0.15f;
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
			consumable_selected = consumableType((consumable_selected + 1) % (NUM_CONSUMABLES - NUM_SHILED_ITEMS));
		}
		else if (Input::wasPadPressed(HATState::PAD_LEFT))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHILED_ITEMS)));
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
		else if (Input::wasButtonPressed(Y_BUTTON))
		{
			World::inst->getConsumable(consumable_selected);
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
			consumable_selected = consumableType((consumable_selected + 1) % (NUM_CONSUMABLES - NUM_SHILED_ITEMS));
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_E))
		{
			consumable_selected = consumableType(ourMod((consumable_selected - 1), (NUM_CONSUMABLES - NUM_SHILED_ITEMS)));
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
		else if (Input::wasKeyPressed(SDL_SCANCODE_F))
		{
			Vector3 ray = camera->getRayDirection(
				Input::mouse_position.x, Input::mouse_position.y,
				Game::instance->window_width, Game::instance->window_height);

			if(ray.length())
				ray.normalize();

			printf("%f %f %f\n", ray.x, ray.y, ray.z);
			
			World::inst->getItem(ray);
		}
		#if DEBUG
		else if (Input::wasKeyPressed(SDL_SCANCODE_J))
		{
			World::inst->hurtPlayer(KNIFE);
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_K))
		{
			World::inst->consumeHunger(10);
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_P))
		{
			World::inst->getConsumable(consumable_selected);
		}
		#endif
	}
}