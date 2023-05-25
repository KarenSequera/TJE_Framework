#include "nightMenu.h"
#include "camera.h"
#include "world.h"
#include "game.h"

Matrix44 model;


MenuEntity::MenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size)
{
	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, true);

	texture = normal_texture;
	selected_texture = selected;
	shader = Shader::Get("data/shaders/quad.vs", "data/shaders/texture.fs");
}

void MenuEntity::render(bool selected)
{
	shader->enable();

	shader->setUniform("u_viewprojection", Game::instance->camera2D->viewprojection_matrix);
	shader->setUniform("u_model", model);
	shader->setUniform("u_color", vec4(1.0, 1.0, 1.0, 1.0));
	shader->setUniform("u_texture", selected ? selected_texture : texture, 0);

	mesh->render(GL_TRIANGLES);
	shader->disable();
}

ConsumableMenuEntity::ConsumableMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, consumableType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	c_type = type;
}

bool ConsumableMenuEntity::onSelect()
{
	World::inst->useConsumable(c_type);
	return true;
}



WeaponMenuEntity::WeaponMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, weaponType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	w_type = type;
}

bool WeaponMenuEntity::onSelect()
{
	World::inst->weapon = w_type;
	World::inst->cur_menu = nullptr;
	return true;
}

DefensiveMenuEntity::DefensiveMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, defensiveType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	d_type = type;
}

bool DefensiveMenuEntity::onSelect()
{
	//TODO
	return true;
}

GeneralMenuEntity::GeneralMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, std::string in_goto)
	: MenuEntity(normal_texture, selected, position, size)
{
	go_to = in_goto;
}

bool GeneralMenuEntity::onSelect()
{
	World::inst->changeMenu(go_to);
	return false;
}

void Menu::render(int selected)
{
	MenuEntity* option;
	for (int i = 0; i < options.size(); i++)
	{
		option = options[i];
		option->render(i == selected);
	}
}

bool Menu::onSelect(int selected)
{
	return options[selected]->onSelect();
}