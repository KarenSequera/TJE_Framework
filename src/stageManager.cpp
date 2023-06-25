#include "stageManager.h"
#include "our_utils.h"
#include "world.h"
#include "camera.h"
#include "audio.h"

StageManager* StageManager::inst = NULL;

StageManager::StageManager(float window_width, float window_height) {
	inst = this;

	transition_time = 0.f;

	cur_stage = nullptr;

	stages["day"] = new DayStage();
	stages["night"] = new NightStage();
	stages["game over"] = new GameOverStage();
	stages["intro stage"] = new IntroStage();

	transition_sounds["day"] = "data/audio/day/to_day.wav";
	transition_sounds["night"] = "data/audio/night/to_night.wav";

	cur_stage = stages["intro stage"];
	cur_stage->onEnter();
}

void StageManager::render() {
	cur_stage->render();

	if (transition_time > 0.f)
		renderStageTransition();
}

void StageManager::renderStageTransition() {
	Shader* shader = Shader::Get("data/shaders/quad.vs", "data/shaders/flat_color.fs");
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	shader->enable();
	shader->setUniform("u_viewprojection", World::inst->camera2D->viewprojection_matrix);
	shader->setUniform("u_color", Vector3(0.f));
	
	float alpha = 0.f;
	float middle = STAGE_TRANSITION_TIME / 2.f;

	if (transition_time > middle) {
		alpha = 1.f - (transition_time - middle) / middle;
	}
	else {
		alpha = transition_time / middle;
	}

	shader->setUniform("u_color", Vector3(0.0f));
	shader->setUniform("u_alpha", alpha);

	World::inst->fullscreen_quad.render(GL_TRIANGLES);

	shader->disable();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void StageManager::changeStage(std::string go_to)
{
	Audio::Play(transition_sounds[go_to].c_str(), 0.35, false);
	cur_stage->onExit();
	transition_time = STAGE_TRANSITION_TIME;
	next = go_to;
}

void StageManager::update(float dt) {
	if (transition_time > 0.f)
	{
		transition_time -= dt;

		float diff = transition_time - STAGE_TRANSITION_TIME / 2.f;
		if(!next.empty() && diff <= 0.05){
			cur_stage->stopMusic();
			cur_stage = stages[next];
			cur_stage->onEnter();
			next.clear();
		}
		cur_stage->update(dt, true);
	}
	else {
		cur_stage->update(dt);
	}
}

void StageManager::resize(float width, float height) {

	World::inst->resizeOptions(width, height);

	for (auto& stage : stages)
		stage.second->resizeOptions(width, height);
}