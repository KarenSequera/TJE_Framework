#include "Menu.h"
#include "camera.h"
#include "world.h"
#include "audio.h"
#include "input.h"
#include "our_utils.h"
#include "stageManager.h"

Matrix44 model;

//TODO: do it in terms of the resolution of the screen
Vector2 size = Vector2(350.f, 100.f);

MenuEntity::MenuEntity(Texture* normal, Texture* selected)
{
	normal_texture = normal;
	selected_texture = selected;
	shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
}

void MenuEntity::render(bool selected, Mesh* quad )
{
	shader->enable();

	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", selected ? selected_texture : normal_texture, 0);
	shader->setUniform("u_animated", false);

	quad->render(GL_TRIANGLES);

	shader->disable();
}

NightMenuEntity::NightMenuEntity(Texture* normal, Texture* selected) : MenuEntity(normal, selected) {}

void NightMenuEntity::render(bool selected, int menu_pos)
{
	shader->enable();

	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", selected ? selected_texture : normal_texture, 0);
	shader->setUniform("u_animated", false);

	World::inst->option_quads[menu_pos]->render(GL_TRIANGLES);
	shader->disable();
	
	float x = World::inst->option_uses_pos[menu_pos].x ;
	float y = World::inst->option_uses_pos[menu_pos].y;

	renderUses(x,y);
}

ConsumableMenuEntity::ConsumableMenuEntity(Texture* normal_texture, Texture* selected, consumableType type)
	: NightMenuEntity(normal_texture, selected)
{
	c_type = type;
}

void ConsumableMenuEntity::renderUses(float x, float y)
{
	//TODO: DO IT PROPERLY

	int size = World::inst->window_height * 0.003;
	drawText(x, y, std::to_string(World::inst->getConsumableQuant(c_type)), Vector3(1.f, 1.f, 1.f), size);
}

bool ConsumableMenuEntity::onSelect()
{
	if(World::inst->useConsumable(c_type))
		return false;
	return true;
}


WeaponMenuEntity::WeaponMenuEntity(Texture* normal_texture, Texture* selected, weaponType type)
	: NightMenuEntity(normal_texture, selected)
{
	w_type = type;
}

void WeaponMenuEntity::renderUses(float x, float y)
{
	if (w_type == FISTS)
		return;

	int size = World::inst->window_height * 0.003;
	drawText(x, y, std::to_string(World::inst->getWeaponUses(w_type)), Vector3(1.f, 1.f, 1.f), size);
}


bool WeaponMenuEntity::onSelect()
{
	if(World::inst->unlimited_everything || w_type == FISTS || World::inst->getWeaponUses(w_type))
	{
		Audio::Play("data/audio/menu/select.wav", 1.f, false);
		World::inst->weapon = w_type;
		World::inst->selectWeapon(w_type);
	}
	else {
		Audio::Play("data/audio/error.wav", 1.f, false);
		//TODO -> make a function or something that resets a timer in the world, when it is not 0 a message will be shown.
		//World::inst->errorMessage("You don't have enough of that item!");
	}

	return false;
}

DefensiveMenuEntity::DefensiveMenuEntity(Texture* normal_texture, Texture* selected, defensiveType type)
	: NightMenuEntity(normal_texture, selected)
{
	d_type = type;
}

void DefensiveMenuEntity::renderUses(float x, float y)
{
	if (d_type == ARMS)
		return;
	int size = World::inst->window_height * 0.003 ;
	drawText(x, y, std::to_string(World::inst->getDefItemUses(d_type)), Vector3(1.f, 1.f, 1.f), size);
}

bool DefensiveMenuEntity::onSelect()
{
	if (World::inst->unlimited_everything || d_type == ARMS || World::inst->getDefItemUses(d_type))
	{
		Audio::Play("data/audio/menu/select.wav", 1.f, false);
		World::inst->defend(d_type);
		return true;

	}
	else {
		//TODO -> make a function or something that resets a timer in the world, when it is not 0 a message will be shown.
		Audio::Play("data/audio/error.wav", 1.f, false);
		//World::inst->errorMessage("You don't have enough of that item!");
	}
	return false;
}

GeneralMenuEntity::GeneralMenuEntity(Texture* normal_texture, Texture* selected, std::string in_goto)
	: NightMenuEntity(normal_texture, selected)
{
	go_to = in_goto;
}

bool GeneralMenuEntity::onSelect()
{
	Audio::Play("data/audio/menu/select.wav", 1.f, false);
	World::inst->changeMenu(go_to);
	return false;
}

Menu::Menu()
{
	start_visible = 0;
	end_visible = 2;
}

void Menu::render(int selected)
{
	NightMenuEntity* option;
	int pos = 0;

	for (int i = start_visible; i <= end_visible; i++)
	{
		option = options[i];
		option->render(i == selected, pos);
		pos++;
	}
}

bool Menu::onSelect(int selected)
{
	return options[selected]->onSelect();
}


PauseMenu::PauseMenu()
{
	options[0] = new MenuEntity(
			Texture::Get("data/quad_textures/menus/pause/resume.tga"),
			Texture::Get("data/quad_textures/menus/pause/resume_selected.tga")
		);

	options[1] = new MenuEntity(
		Texture::Get("data/quad_textures/menus/pause/restart.tga"),
		Texture::Get("data/quad_textures/menus/pause/restart_selected.tga")
		);

	options[2] = new MenuEntity(
		Texture::Get("data/quad_textures/menus/pause/exit.tga"),
		Texture::Get("data/quad_textures/menus/pause/exit_selected.tga")
		);

	selected = 0;
}

void PauseMenu::render()
{
	// draw overlay and game paused message
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
	shader->enable();
	shader->setUniform("u_animated", false);
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/tutorial/overlay.tga"), 0);
	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	shader->setUniform("u_texture", Texture::Get("data/quad_textures/menus/pause/game_paused.tga"), 0);
	option_quads[0].render(GL_TRIANGLES);

	shader->disable();
	glEnable(GL_DEPTH_TEST);

	for (int i = 0; i < OPTIONS_PAUSE_MENU; i++)
		options[i]->render(selected == i, &option_quads[i + 1]);
	glDisable(GL_BLEND);
}

void PauseMenu::update()
{
	if (Input::gamepads[0].connected) {
		if (Input::gamepads[0].didDirectionChanged(FLICK_UP))
			changeOption(-1, selected, OPTIONS_PAUSE_MENU);

		else if (Input::gamepads[0].didDirectionChanged(FLICK_DOWN))
			changeOption(1, selected, OPTIONS_PAUSE_MENU);

		else if (Input::wasButtonPressed(A_BUTTON))
			selectOption();

	}
	else {
		if (Input::wasKeyPressed(SDL_SCANCODE_W) || Input::wasKeyPressed(SDL_SCANCODE_UP))
			changeOption(-1, selected, OPTIONS_PAUSE_MENU);

		else if (Input::wasKeyPressed(SDL_SCANCODE_S) || Input::wasKeyPressed(SDL_SCANCODE_DOWN))
			changeOption(1, selected, OPTIONS_PAUSE_MENU);
		else if (Input::wasKeyPressed(SDL_SCANCODE_C))
			selectOption();
	}
}

void PauseMenu::selectOption()
{
	Audio::Play("data/audio/menu/select.wav", 1.f, false);
	switch (selected) {
		case RESUME:
			World::inst->frozen = false;
			selected = 0;
			break;
		case RESTART:
			World::inst->resetWorld();
			World::inst->triggerTutorial = false;
			StageManager::inst->changeStage("day");
			break;
		case EXIT:
			exit(-1);
	}
}

void PauseMenu::resize(float width, float height)
{
	float size_y = height / 8.f;
	float size_x = size_y * 350.f / 100.f;

	float offset = 1.25 * size_y;
	float start = height - 2.f * size_y;

	for (int i = 0; i < OPTIONS_PAUSE_MENU + 1; i++)
		option_quads[i].createQuad(width / 2.f, start - offset * i, size_x, size_y, true);
}