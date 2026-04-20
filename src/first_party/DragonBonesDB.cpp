#include "DragonBonesDB.h"
#include "Renderer.h"
#include "Config.h"
#include "ComponentDB.h"
#include <SDL_image.h>
#include <glm.hpp>
#include <SDLArmatureProxy.h>

#include "Helper.h"
#include "PrintUtils.h"

void DragonBonesDB::Update(float deltaTime) {
	factory->update(deltaTime);
}

void DragonBonesDB::ProcessRenderRequests() {
	float zoom_factor = Renderer::Zoom_Factor;
	SDL_RenderSetScale(Renderer::GetRenderer(), zoom_factor, zoom_factor);

	glm::ivec2 camera_dimensions = Renderer::GetCameraDimensions();
	glm::vec2 cam_pos = Renderer::GetCameraPosition();

	for (const auto& req : render_requests) {
		if (req.armature) {
			glm::vec2 world_pos = glm::vec2(req.x, req.y) - cam_pos;

			float final_x = world_pos.x * Renderer::PIXELS_PER_METER
				+ camera_dimensions.x * 0.5f * (1.0f / Renderer::Zoom_Factor);

			float final_y = world_pos.y * Renderer::PIXELS_PER_METER
				+ camera_dimensions.y * 0.5f * (1.0f / Renderer::Zoom_Factor);

			req.armature->setPosition({ final_x, final_y });
			req.armature->draw(Renderer::GetRenderer(), req.scale_x, req.scale_y);
		}
	}

	SDL_RenderSetScale(Renderer::GetRenderer(), 1.0f, 1.0f);
	render_requests.clear();
}

void DragonBonesDB::LoadData(const std::string& ske_json, const std::string& tex_json, const std::string& tex_png) {
	if (loaded_assets.find(ske_json) != loaded_assets.end()) return;

	std::string path_png = (Config::RESOURCES_DIR / "dragonbones" / tex_png).string();
	std::string path_ske = (Config::RESOURCES_DIR / "dragonbones" / ske_json).string();
	std::string path_tex = (Config::RESOURCES_DIR / "dragonbones" / tex_json).string();

	SDL_Texture* texture = IMG_LoadTexture(Renderer::GetRenderer(), path_png.c_str());
	if (!texture) {
		print("DragonBones: Failed to load texture ",tex_png);
		return;
	}

	factory->loadDragonBonesData(path_ske);
	factory->loadTextureAtlasData(path_tex, texture);
	loaded_assets[ske_json] = true;
}

dragonBones::SDLArmatureDisplay* DragonBonesDB::CreateArmature(const std::string& armatureName) {
	return new dragonBones::SDLArmatureDisplay(armatureName);
}

void DragonBonesDB::PlayAnimation(dragonBones::SDLArmatureDisplay* armature, const std::string& anim_name) {
	if (!armature || !armature->getAnimation()) return;

	if (armature->getArmature()->getAnimation()->hasAnimation(anim_name)) {
		armature->getAnimation()->play(anim_name);
	}
	else {
		println("DragonBones: Failed to play animation ", anim_name);
	}
}

void DragonBonesDB::Draw(dragonBones::SDLArmatureDisplay* armature, float x, float y, float scale_x, float scale_y) {
	if (!armature) return;
	render_requests.push_back({ armature, x, y, scale_x, scale_y });
}

void DragonBonesDB::DestroyArmature(dragonBones::SDLArmatureDisplay* armature) {
	if (armature) {
		delete armature;
	}
}

void DragonBonesDB::Test() {
	dragonBones::SDLFactory factory;

	SDL_Texture* texture = IMG_LoadTexture(Renderer::GetRenderer(), "resources/dragonbones/DragonBoy_tex.png");
	if (!texture) {
		SDL_Log("Failed to load DragonBoy texture!");
		return;
	}

	factory.loadDragonBonesData("resources/dragonbones/DragonBoy_ske.json");
	factory.loadTextureAtlasData("resources/dragonbones/DragonBoy_tex.json", texture);

	auto armatureDisplay = new dragonBones::SDLArmatureDisplay("Dragon");
	armatureDisplay->getAnimation()->play("walk");
	armatureDisplay->setPosition({ 320.0f, 300.0f });

	bool quit = false;
	Uint64 lastTime = SDL_GetPerformanceCounter();

	while (!quit) {
		Uint64 now = SDL_GetPerformanceCounter();
		float deltaTime = (float)(now - lastTime) / SDL_GetPerformanceFrequency();
		lastTime = now;

		SDL_Event event;
		while (Helper::SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT)
				quit = true;
		}

		factory.update(deltaTime);

		Renderer::Clear();
		armatureDisplay->draw(Renderer::GetRenderer());
		Renderer::Present();
	}
}