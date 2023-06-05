#pragma once
#include "player.h"
#define NUM_GENERAL_OPTIONS 3
#define NUM_OPTIONS 3

class Camera;


class MenuEntity
{
public:

	MenuEntity(Texture* normal, Texture* selected);

	Shader* shader;
	Texture* normal_texture;
	Texture* selected_texture;

	void render(bool selected, int menu_pos);
	virtual void renderUses(float x, float y) {};
	
	//Returns whether the option selected triggers a change of turn from player to zombie
	virtual bool onSelect() { return false; };
};

class ConsumableMenuEntity : public MenuEntity
{
public:
	consumableType c_type;

	ConsumableMenuEntity(Texture* normal_texture, Texture* selected, consumableType type);
	void renderUses(float x, float y);
	bool onSelect();
};

class WeaponMenuEntity : public MenuEntity
{
public:
	weaponType w_type;

	WeaponMenuEntity(Texture* normal_texture, Texture* selected, weaponType type);
	void renderUses(float x, float y);
	bool onSelect();
};

class DefensiveMenuEntity : public MenuEntity
{
public:
	defensiveType d_type;

	DefensiveMenuEntity(Texture* normal_texture, Texture* selected, defensiveType type);
	void renderUses(float x, float y);
	bool onSelect();
};

class GeneralMenuEntity : public MenuEntity
{
public:
	std::string go_to;

	GeneralMenuEntity(Texture* normal_texture, Texture* selected, std::string in_goto);
	bool onSelect();
};

class Menu
{
public:
	std::vector<MenuEntity*> options;
	int start_visible;
	int end_visible;

	Menu();

	void render(int selected);
	bool onSelect(int selected);
};
