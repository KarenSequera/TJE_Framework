#include "Menu.h"
#include "camera.h"
#include "world.h"
#include "audio.h"

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


