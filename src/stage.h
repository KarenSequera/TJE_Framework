#pragma once
#include <vector>
#include "world.h"
#include "audio.h"
#include "rendertotexture.h"

#define RENDER_TARGET_RES 1024
#define POST_FX true

class Camera;
class Stage {
public:

	Camera* camera;
	HCHANNEL channel;
	
	bool frozen;
	bool mouse_locked;
	bool post_fx;
	RenderToTexture* renderTarget;
	Shader* fx_shader;

	//ctor
	Stage();
	//~Stage();

	// Functions that will be overwritten
	virtual void onEnter() {};
	virtual void onExit() {};
	virtual void render() {};
	virtual void update(float dt, bool transitioning = false) {};
	virtual void resizeOptions(float width, float height) {};
	void stopMusic();
	void renderBar(Vector3 position, float hp_ratio, Shader* shader, float width,
		float height, Texture* texture_background, Texture* texture_foreground);


};
