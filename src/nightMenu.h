#pragma once
#include "player.h"
#define NUM_GENERAL_OPTIONS 3

class Camera;

class MenuEntity : public EntityMesh
{
public:
	Vector2 position;
	Vector2 size;

	MenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size);

	Texture* selected_texture;

	void render(bool selected);
	virtual bool onSelect() { return false; };
};

class ConsumableMenuEntity : public MenuEntity
{
public:
	consumableType c_type;

	ConsumableMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, consumableType type);
	bool onSelect();
};

class WeaponMenuEntity : public MenuEntity
{
public:
	weaponType w_type;

	WeaponMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, weaponType type);
	bool onSelect();
};

class DefensiveMenuEntity : public MenuEntity
{
public:
	defensiveType d_type;

	DefensiveMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, defensiveType type);
	bool onSelect();
};

class GeneralMenuEntity : public MenuEntity
{
public:
	std::string go_to;

	GeneralMenuEntity(Texture* normal_texture, Texture* selected, Vector2 position, Vector2 size, std::string in_goto);
	bool onSelect();
};

class Menu
{
public:
	std::vector<MenuEntity*> options;

	Menu() {};

	void render(int selected);
	bool onSelect(int selected);
};
//
//class ConsumableMenu : public Menu
//{
//public:
//	void render(int selected);
//};