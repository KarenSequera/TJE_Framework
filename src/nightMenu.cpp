#include "nightMenu.h"
#include "camera.h"
#include "world.h"
#include "game.h"

Matrix44 model;

//TODO: do it in terms of the resolution of the screen
Vector2 positions[3] = { Vector2(1000.f, 450.f), Vector2(1000.f, 300.f) , Vector2(1000.f, 150.f) };
Vector2 size = Vector2(350.f, 100.f);

MenuEntity::MenuEntity(Texture* normal_texture, Texture* selected)
{
	texture = normal_texture;
	selected_texture = selected;
	shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
}

void MenuEntity::render(bool selected, int menu_pos)
{
	mesh = new Mesh();
	mesh->createQuad(positions[menu_pos].x, positions[menu_pos].y, size.x, size.y, true);

	shader->enable();

	shader->setUniform("u_viewprojection", Game::instance->camera2D->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", selected ? selected_texture : texture, 0);

	mesh->render(GL_TRIANGLES);
	shader->disable();
}

ConsumableMenuEntity::ConsumableMenuEntity(Texture* normal_texture, Texture* selected, consumableType type)
	: MenuEntity(normal_texture, selected)
{
	c_type = type;
}

bool ConsumableMenuEntity::onSelect()
{
	World::inst->useConsumable(c_type);
	return true;
}


WeaponMenuEntity::WeaponMenuEntity(Texture* normal_texture, Texture* selected, weaponType type)
	: MenuEntity(normal_texture, selected)
{
	w_type = type;
}

bool WeaponMenuEntity::onSelect()
{
	World::inst->weapon = w_type;
	World::inst->cur_menu = nullptr;
	return true;
}

DefensiveMenuEntity::DefensiveMenuEntity(Texture* normal_texture, Texture* selected, defensiveType type)
	: MenuEntity(normal_texture, selected)
{
	d_type = type;
}

bool DefensiveMenuEntity::onSelect()
{
	//TODO
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