#include "stage.h"
#include "input.h"
#include "camera.h"
#include "world.h"

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

	//hide the cursor
	SDL_ShowCursor(false); //hide or show the mouse+

};

float right_analog_x_disp;
float right_analog_y_disp;
float left_analog_x_disp;
float left_analog_y_disp;
float speed;

void DayStage::render() {
	for (auto& entity : World::world_instance->day_entities) {
		entity->render();
	}
}

void DayStage::update(float dt) {
	speed = dt * mouse_speed; //the speed is defined by the seconds_elapsed so it goes constant

	//example
	angle += (float)dt * 10.0f;

	/*Vector3 prueba = camera->getLocalVector(Vector3(0.0f, 0.0f, 1.0f));
	printf("%f %f %f\n", prueba.x, prueba.y, prueba.z);*/

	//We check if the gamepad is connected:
	if (Input::gamepads[0].connected) {
		//The orientation of the camara is conttrolled with the right joystick
		right_analog_x_disp = Input::gamepads[0].axis[RIGHT_ANALOG_X];
		right_analog_y_disp = Input::gamepads[0].axis[RIGHT_ANALOG_Y];

		left_analog_x_disp = Input::gamepads[0].axis[LEFT_ANALOG_X];
		left_analog_y_disp = Input::gamepads[0].axis[LEFT_ANALOG_Y];


		//We check in case the gamepad has slight drift. 
		if (std::abs(right_analog_x_disp) > DRIFT_THRESHOLD || std::abs(right_analog_y_disp) > DRIFT_THRESHOLD) {
			camera->ourRotate(-right_analog_x_disp * gamepad_sensitivity, -right_analog_y_disp * gamepad_sensitivity);
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

		//async input to move the camera around
		if (Input::isKeyPressed(SDL_SCANCODE_LSHIFT)) speed *= 10; //move faster with left shift

		if (Input::isKeyPressed(SDL_SCANCODE_W) || Input::isKeyPressed(SDL_SCANCODE_UP))
			camera->moveXZ(Vector3(0.0f, 0.0f, 1.0f) * speed);

		if (Input::isKeyPressed(SDL_SCANCODE_S) || Input::isKeyPressed(SDL_SCANCODE_DOWN))
			camera->moveXZ(Vector3(0.0f, 0.0f, -1.0f) * speed);

		if (Input::isKeyPressed(SDL_SCANCODE_A) || Input::isKeyPressed(SDL_SCANCODE_LEFT))
			camera->moveXZ(Vector3(1.0f, 0.0f, 0.0f) * speed);

		if (Input::isKeyPressed(SDL_SCANCODE_D) || Input::isKeyPressed(SDL_SCANCODE_RIGHT))
			camera->moveXZ(Vector3(-1.0f, 0.0f, 0.0f) * speed);

		//to navigate with the mouse fixed in the middle
		if (mouse_locked)
			Input::centerMouse();
	}

}