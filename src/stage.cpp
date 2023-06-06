#include "StageManager.h"
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
	
	time_remaining = DAY_TIME;

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse
};

void DayStage::onEnter()
{
	// TODO: add the shield that has been left off from the night

	camera->lookAt(Vector3(-500.0f, 100.0f, 100.0f), Vector3(0.0f, 0.0f, 0.0f), Vector3(0.0f, 1.0f, 0.0f)); //position the camera and point to 0,0,0
	World::inst->player->position = camera->eye;
	World::inst->spawnerInit();
	time_remaining = DAY_TIME;
}

void DayStage::render() {
	World::inst->player->position = camera->eye;

	for (auto& entity : World::inst->day_entities) {
		entity->render();
	}

	drawText(5, 25, "HP: " + std::to_string(World::inst->player->health), Vector3(1.0f, 0.0f, 0.0f), 2);
	drawText(5, 45, "HUNGER: " + std::to_string(World::inst->player->hunger), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 65, "SHIELD: " + std::to_string(World::inst->player->shield), Vector3(0.75f, 0.75f, 0.75f), 2);
	
	renderConsumableMenu();
	#if DEBUG
	drawText(5, 505, "C: consume, F: getItem, J: hurt, K: get hunger, N: to night"
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
			World::inst->player->velocity.x = (World::inst->player->velocity.x-new_dir.x)*0.5;
			World::inst->player->velocity.z = (World::inst->player->velocity.z-new_dir.z)*0;
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

		else if (Input::wasKeyPressed(SDL_SCANCODE_N))
			StageManager::inst->changeStage("night");
		#endif
	}
}

NightStage::NightStage() : Stage()
{
	cur_night = 0;
	is_player_turn = true;

	selected_target = 0;
	turns_to_day = 0;
}

void NightStage::onEnter() {
	World::inst->generateZombies(cur_night);

	World::inst->player->model_matrix = World::inst->night_models[2];
	World::inst->player->health = 100;
	World::inst->generateZombies(cur_night);
	World* inst = World::inst;
	is_player_turn = true;

	//TODO: adjust formula so that it is enjoyable
	turns_to_day = 10 + (cur_night % 5) * 10;

	cur_night++;
	
	//camera->lookAt(World::inst->night_models[0].getTranslation(), Vector3(419.525, 196.748, 502.831), Vector3(0.0f, 1.0f, 0.0f));
	camera->lookAt(World::inst->night_models[0].getTranslation() ,World::inst->night_models[1].getTranslation(), Vector3(0.0f, 1.0f, 0.0f));

}

void NightStage::render()
{
	for (auto& entity : World::inst->night_entities)
	{
		entity->render();
	}
	renderCrosshair();

	// render what must be rendered always
	drawText(5, 125, "Player Health: " + std::to_string(World::inst->player->health), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 145, "Player Hunger: " + std::to_string(World::inst->player->hunger), Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 165, "Player Shield: " + std::to_string(World::inst->player->shield), Vector3(1.0f, 0.75f, 0.0f), 2);
	
	if (is_player_turn)
	{
		playerTurnRender();
	}
	else
	{
		zombieTurnRender();
	}
}

void NightStage::renderCrosshair()
{
	Matrix44 model = World::inst->wave[selected_target]->getGlobalMatrix();
	Vector3 position = camera->project(model.getTranslation(), Game::instance->window_width, Game::instance->window_width);

	//position.x = (position.x / position.z);// * 0.5 * Game::instance->window_width;
	//position.y = (position.y / position.z);// * 0.5 * Game::instance->window_height;
	
	Mesh quad;
	quad.createQuad(position.x, position.y, 350.f, 100.f, true);

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");

	shader->enable();

	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture",Texture::Get("data/texture.tga"), 0);

	//quad->render(GL_TRIANGLES);
	shader->disable();

}

void NightStage::debugZombies()
{
	if (World::inst->ready_to_attack) {
		for (int i = 0; i < World::inst->zombies_alive; i++)
		{
			drawText(50 + i * 250, 200, "Z health: " + std::to_string(World::inst->wave[i]->info.health), (selected_target == i) ? Vector3(1.0f, 1.0f, 1.0f) : Vector3(1.0f, 0.75f, 0.0f), 2);
			drawText(50 + i * 250, 230, "Invulnerable to: " + std::to_string(World::inst->wave[i]->info.invulnerable_to), Vector3(1.0f, 0.75f, 0.0f), 2);
			drawText(50 + i * 250, 260, "Weak to: " + std::to_string(World::inst->wave[i]->info.weakness), Vector3(1.0f, 0.75f, 0.0f), 2);

		}
	}
	else
	{
		for (int i = 0; i < World::inst->zombies_alive; i++)
		{
			drawText(50 + i * 250, 200, "Z health: " + std::to_string(World::inst->wave[i]->info.health), Vector3(1.0f, 0.75f, 0.0f), 2);
			drawText(50 + i * 250, 230, "Invulnerable to: " + std::to_string(World::inst->wave[i]->info.invulnerable_to), Vector3(1.0f, 0.75f, 0.0f), 2);
			drawText(50 + i * 250, 260, "Weak to: " + std::to_string(World::inst->wave[i]->info.weakness), Vector3(1.0f, 0.75f, 0.0f), 2);
		}
	}

	drawText(50, 300, "WEAPONS: -1 nothing, 0 fists, 1 bat, 2 knife, 3 gun", Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(50, 330, "W & S: navigate, C: confirm, Z: go back", Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(50, 360, "U:unlimited, N:to day", Vector3(1.0f, 0.75f, 0.0f), 2);
	
}

void NightStage::playerTurnRender() {
	drawText(5, 65, "Player's turn ", Vector3(1.0f, 0.75f, 0.0f), 2);
	drawText(5, 85, "Unlimited everything: " + std::to_string(World::inst->unlimited_everything), Vector3(1.0f, 0.75f, 0.0f), 2);

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

}


void NightStage::update(float dt)
{
	
#if DEBUG
	if (Input::wasKeyPressed(SDL_SCANCODE_N))
		StageManager::inst->changeStage("day");

	else if (Input::wasKeyPressed(SDL_SCANCODE_U))
		World::inst->unlimited_everything = !World::inst->unlimited_everything;
#endif

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
	if (World::inst->ready_to_attack)
	{
		if (Input::wasKeyPressed(SDL_SCANCODE_A) || Input::wasKeyPressed(SDL_SCANCODE_LEFT))
			selected_target = ourMod(selected_target - 1, World::inst->zombies_alive);

		else if (Input::wasKeyPressed(SDL_SCANCODE_D) || Input::wasKeyPressed(SDL_SCANCODE_RIGHT))
			selected_target = ourMod(selected_target + 1, World::inst->zombies_alive);

		else if (Input::wasKeyPressed(SDL_SCANCODE_C)) {
			int result = World::inst->hurtZombie(selected_target);
			
			if (World::inst->zombies_alive <= 0) {
				StageManager::inst->changeStage("day");
			}

			// if the attack is not super effective then we move onto the zombie's turn
			if(result != 2)
				is_player_turn = false;
						
			// otherwise we give the player another action
			// TODO: message of super efective, perhaps -> with a bool and a specific UI element for it, for example
			World::inst->ready_to_attack = false;
			World::inst->changeMenu("general");
			selected_target = 0;
		}
		else if (Input::wasKeyPressed(SDL_SCANCODE_Z))
			World::inst->ready_to_attack = false;
	}
	else
	{
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
	}

	#if DEBUG	
	if (Input::wasKeyPressed(SDL_SCANCODE_J))
		World::inst->hurtPlayer(KNIFE);

	else if (Input::wasKeyPressed(SDL_SCANCODE_K))
		World::inst->consumeHunger(10);
	#endif
};

void NightStage::zombieTurnUpdate() 
{
	int num_zombies = World::inst->wave.size();

	for (int i = 0; i < num_zombies; i++)
	{

		//In the turn of the zombies 
		// The zombies only hurt the player

		weaponType weapon = World::inst->wave[i]->info.weapon;

		World::inst->hurtPlayer(weapon);

		if (!World::inst->isPlayerAlive()) 
		{
			StageManager::inst->changeStage("game over");
			return;
		}
		
	}
	is_player_turn = true;
	World::inst->changeMenu("general");
	newTurn();
	return;
}

void NightStage::newTurn() 
{
	turns_to_day--;

	if (turns_to_day == 0)
		StageManager::inst->changeStage("day");

	//TODO: Make a variable that changes depending on the number of nights, the higher the night the more it takes.
	World::inst->consumeHunger(10);

	selected_target = 0;
}

void GameOverStage::render() {
	drawText(5, 25, "oops, you died!", Vector3(1.0f, 0.0f, 0.0f), 2);
}
