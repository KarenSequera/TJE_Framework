#include "StageManager.h"
#include "input.h"
#include "camera.h"

#include <algorithm>

Stage::Stage() {
	frozen = false;
	camera = Camera::current;
	post_fx = false;
	renderTarget = nullptr;
	mouse_locked = false;

	if (!renderTarget) {
		renderTarget = new RenderToTexture();
		renderTarget->create(RENDER_TARGET_RES, RENDER_TARGET_RES);
	}
}


void Stage::renderBar(Vector3 position, float hp_ratio, Shader* shader, float width, float height, Texture* texture_background, Texture* texture_foreground)
{
	Mesh quad1;
	Mesh quad2;

	//Creation of the first quad, the background one.
	quad1.createQuad(position.x, position.y, width, height, true);

	//Creation of the second quad. This one contains the life information. 
	float foreBarWidth = width * hp_ratio;
	float offset = (width - foreBarWidth) * 0.5f;
	quad2.createQuad(position.x - offset, position.y, foreBarWidth, height, true);

	shader->setUniform("u_texture", texture_background, 0);
	quad1.render(GL_TRIANGLES);

	shader->setUniform("u_texture", texture_foreground, 0);
	quad2.render(GL_TRIANGLES);

}

void Stage::stopMusic() {
	Audio::Stop(channel);
}