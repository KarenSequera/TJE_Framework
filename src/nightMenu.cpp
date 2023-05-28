#include "nightMenu.h"
#include "camera.h"
#include "world.h"

Matrix44 model;

//TODO: do it in terms of the resolution of the screen
Vector2 size = Vector2(350.f, 100.f);

MenuEntity::MenuEntity(Texture* normal, Texture* selected)
{
	normal_texture = normal;
	selected_texture = selected;
	shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
}

void MenuEntity::render(bool selected, int menu_pos)
{
	shader->enable();

	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", selected ? selected_texture : normal_texture, 0);

	World::inst->option_quads[menu_pos]->render(GL_TRIANGLES);
	shader->disable();
}

ConsumableMenuEntity::ConsumableMenuEntity(Texture* normal_texture, Texture* selected, consumableType type)
	: MenuEntity(normal_texture, selected)
{
	c_type = type;
}

bool ConsumableMenuEntity::onSelect()
{
	if(World::inst->useConsumable(c_type))
		return false;
	return true;
}


WeaponMenuEntity::WeaponMenuEntity(Texture* normal_texture, Texture* selected, weaponType type)
	: MenuEntity(normal_texture, selected)
{
	w_type = type;
}

bool WeaponMenuEntity::onSelect()
{
	if(World::inst->unlimited_everything || w_type == FISTS || World::inst->getWeaponUses(w_type))
	{
		World::inst->weapon = w_type;
		World::inst->ready_to_attack = true;
	}
	else {
		//TODO -> make a function or something that resets a timer in the world, when it is not 0 a message will be shown.
		//World::inst->errorMessage("You don't have enough of that item!");
	}

	return false;
}

DefensiveMenuEntity::DefensiveMenuEntity(Texture* normal_texture, Texture* selected, defensiveType type)
	: MenuEntity(normal_texture, selected)
{
	d_type = type;
}

bool DefensiveMenuEntity::onSelect()
{
	if (World::inst->unlimited_everything || World::inst->getDefItemUses(d_type))
	{
		//TODO:
		World::inst->defend(d_type);
	}
	else {
		//TODO -> make a function or something that resets a timer in the world, when it is not 0 a message will be shown.
		//World::inst->errorMessage("You don't have enough of that item!");
	}
	return true;
}

GeneralMenuEntity::GeneralMenuEntity(Texture* normal_texture, Texture* selected, std::string in_goto)
	: MenuEntity(normal_texture, selected)
{
	go_to = in_goto;
}

bool GeneralMenuEntity::onSelect()
{
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
	MenuEntity* option;
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