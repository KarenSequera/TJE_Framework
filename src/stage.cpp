#include "StageManager.h"
#include "input.h"
#include "camera.h"
#include "our_utils.h"

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
	#if DEBUG
	time_remaining = 5.f;
	#else
	time_remaining = DAY_TIME;
	#endif

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse
};

void DayStage::onEnter()
{
	Camera::current->lookAt(Vector3(0.0f, 135.0f, 100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)); //position the camera and point to 0,0,0
	World::inst->player->position = camera->eye;
	World::inst->spawnerInit();
	#if DEBUG
	time_remaining = 5.f;
	#else
	time_remaining = DAY_TIME;
	#endif
}

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
			//printf("%f %f %f\n", collision.colNormal.x, collision.colNormal.y, collision.colNormal.z);

			new_dir = new_dir * collision.colNormal;
			World::inst->player->velocity.x -= new_dir.x;
			World::inst->player->velocity.z -= new_dir.z;
		}
		World::inst->player->velocity.y = 0.f;

		World::inst->player->position = World::inst->player->position + World::inst->player->velocity * dt;
		camera->eye = World::inst->player->position;


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

		else if (Input::wasKeyPressed(SDL_SCANCODE_P))
			World::inst->getConsumable(consumable_selected);

		else if (Input::wasKeyPressed(SDL_SCANCODE_N))
			StageManager::inst->changeStage("night");
		#endif
	}
}

NightStage::NightStage() : Stage()
{
	cur_night = 0;
	cur_turn = 0;
	is_player_turn = true;

	selected_target = 0;
	turns_to_day = 0;
}

void NightStage::resetParams()
{
	selected_target = 0;
}

void NightStage::onEnter() {
	World::inst->generateZombies(cur_night);
	World::inst->player->health = 100;

	is_player_turn = true;

	//TODO: set num_turns properly (in terms of the number of nights)
	turns_to_day = 5;
	cur_turn = 0;

	cur_night++;

	resetParams();
}

void NightStage::render()
{
	// render what must be rendered always
	drawText(5, 125, "Player Health: " + std::to_string(World::inst->player->health), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 145, "Player Hunger: " + std::to_string(World::inst->player->hunger), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 165, "Option: " + std::to_string(World::inst->selected_option), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 185, "start: " + std::to_string(World::inst->cur_menu->start_visible), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 205, "end: " + std::to_string(World::inst->cur_menu->end_visible), Vector3(1.0f, 0.75f, 0.0f), 2);


	if (is_player_turn)
	{
		playerTurnRender();
	}
	else
	{
		zombieTurnRender();
	}
}

void NightStage::playerTurnRender() {
	//TODO
	drawText(5, 65, "Player's turn ", Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 85, "Selected target: " + std::to_string(selected_target), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 105, "Health target: " + std::to_string(World::inst->wave[selected_target]->info.health), Vector3(1.0f, 0.75f, 0.0f), 2);

	// Render menus -> prep options
	glClear(GL_DEPTH_BUFFER_BIT);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	//menu render
	World::inst->cur_menu->render(World::inst->selected_option);

	// Render menus -> prep options
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
}

void NightStage::zombieTurnRender() {
	//TODO
}


void NightStage::update(float dt)
{
	/*#if DEBUG
	if (Input::wasKeyPressed(SDL_SCANCODE_D))
	{
		finished = true;
	}
	#endif*/
	

	if (is_player_turn)
	{
		playerTurnUpdate();
	}
	else
	{
		zombieTurnUpdate();
	}
}

void NightStage::playerTurnUpdate() 
{
	// if null pointer, the user has chosen to attack
	if (World::inst->cur_menu == nullptr)
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT))
			selected_target = ourMod(selected_target - 1, World::inst->zombies_alive);

		else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
			selected_target = ourMod(selected_target + 1, World::inst->zombies_alive);

		//TODO
		//int weakness = World::zombie_attacked(weapon, World::inst->wave[selected_target]);
		
		else if (Input::wasKeyPressed(SDL_SCANCODE_Q)) {
			int result = World::inst->hurtZombie(selected_target);
			
			if (World::inst->zombies_alive <= 0) {
				StageManager::inst->changeStage("day");
			}

			// if the attack is not super effective then we move onto the zombie's turn
			if(result != 2)
				is_player_turn = false;
						
			// otherwise we give the player another action
			// TODO: message of super efective, perhaps -> with a bool and a specific UI element for it, for example
			World::inst->changeMenu("general");
		}
	}

	if (Input::wasKeyPressed(SDL_SCANCODE_W) || Input::wasKeyPressed(SDL_SCANCODE_UP))
		World::inst->changeOption(-1);

	else if (Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasKeyPressed(SDL_SCANCODE_DOWN))
		World::inst->changeOption(1);

	else if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
		if (World::inst->selectOption()) {
			is_player_turn = false;
		}
	}
		
	else if (Input::wasKeyPressed(SDL_SCANCODE_Z))
		World::inst->changeMenu("general");

	#if DEBUG	
	else if (Input::wasKeyPressed(SDL_SCANCODE_J))
		World::inst->hurtPlayer(KNIFE);

	else if (Input::wasKeyPressed(SDL_SCANCODE_K))
		World::inst->consumeHunger(10);
	#endif
};

void NightStage::zombieTurnUpdate() 
{
	std::cout << " || ";
	std::cout << "zombie turn";
	std::cout << " || ";

	int num_zombies = World::inst->wave.size();

	for (int i = 0; i < num_zombies; i++)
	{

		//In the turn of the zombies 
		// The zombies only hurt the player

		weaponType weapon = World::inst->wave[i]->info.weapon;

		std::cout << " || ";
		std::cout << "zombie hurt player with ";
		std::cout << weapon;
		std::cout << " || ";

		World::inst->hurtPlayer(weapon);

		if (!World::inst->isPlayerAlive()) 
		{
			std::cout << " || ";
			std::cout << "game over";
			std::cout << " || \n";

			StageManager::inst->changeStage("game over");
			return;
		}
		
	}
	is_player_turn = true;
	newTurn();
	return;
}

void NightStage::newTurn() 
{
	cur_turn++;

	if (cur_turn >= turns_to_day)
		StageManager::inst->changeStage("day");

	resetParams();
}

void GameOverStage::render() {
	drawText(5, 25, "oops, you died!", Vector3(1.0f, 0.0f, 0.0f), 2);
}
