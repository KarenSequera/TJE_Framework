#include "nightMenu.h"

MenuEntity::MenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size)
{
	mesh = new Mesh();
	mesh->createQuad(position.x, position.y, size.x, size.y, true);

	texture = normal_texture;
	selected_texture = selected;
}

void MenuEntity::render(bool selected)
{

}

ConsumableMenuEntity::ConsumableMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, consumableType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	c_type = type;
}

void ConsumableMenuEntity::onSelect()
{

}



WeaponMenuEntity::WeaponMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, weaponType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	w_type = type;
}

void WeaponMenuEntity::onSelect()
{

}



DefensiveMenuEntity::DefensiveMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, defensiveType type)
	: MenuEntity(normal_texture, selected, position, size)
{
	d_type = type;
}

void DefensiveMenuEntity::onSelect()
{

}




GeneralMenuEntity::GeneralMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, std::string in_goto)
	: MenuEntity(normal_texture, selected, position, size)
{
	go_to = in_goto;
}

void GeneralMenuEntity::onSelect()
{

}


void Menu::onSelect(int selected)
{
	options.at(selected)->onSelect();
}