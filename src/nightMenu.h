#pragma once
#include "player.h"

class MenuEntity : public EntityMesh
{
public:
	Vector2 position;
	Vector2 size;

	MenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size);

	Texture* selected_texture;

	void render(bool selected);
	virtual void onSelect() {};
};

class ConsumableMenuEntity : public MenuEntity
{
public:
	consumableType c_type;

	ConsumableMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, consumableType type);
	void onSelect();
};

class WeaponMenuEntity : public MenuEntity
{
public:
	weaponType w_type;

	WeaponMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, weaponType type);
	void onSelect();
};

class DefensiveMenuEntity : public MenuEntity
{
public:
	defensiveType d_type;

	DefensiveMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, defensiveType type);
	void onSelect();
};

class GeneralMenuEntity : public MenuEntity
{
public:
	std::string go_to;

	GeneralMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, std::string in_goto);
	void onSelect();
};

class Menu
{
public:
	std::vector<MenuEntity*> options;

	Menu() {};

	void render();
	void onSelect(int selected);
};