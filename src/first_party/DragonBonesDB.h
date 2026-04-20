#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <dragonBones/SDL/SDLFactory.h>
#include <dragonBones/SDL/SDLArmatureDisplay.h>

struct ArmatureDrawRequest {
	dragonBones::SDLArmatureDisplay* armature;
	float x, y;
	float scale_x, scale_y;
};

class DragonBonesDB {



public:
	static void Test();

	static inline dragonBones::SDLFactory* factory = new dragonBones::SDLFactory();
	static inline std::unordered_map<std::string, bool> loaded_assets;

	static inline std::vector<ArmatureDrawRequest> render_requests;

	static void Update(float deltaTime);
	static void ProcessRenderRequests();

	static void LoadData(const std::string& ske_json, const std::string& tex_json, const std::string& tex_png);
	static dragonBones::SDLArmatureDisplay* CreateArmature(const std::string& armatureName);
	static void PlayAnimation(dragonBones::SDLArmatureDisplay* armature, const std::string& anim_name);
	static void Draw(dragonBones::SDLArmatureDisplay* armature, float x, float y, float scale_x = 1.0f, float scale_y = 1.0f);
	static void DestroyArmature(dragonBones::SDLArmatureDisplay* armature);
};

