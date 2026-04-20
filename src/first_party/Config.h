#pragma once
#include <string>
#include <filesystem>
#include <glm.hpp>
#include <vector>

class Config
{
private:
	static void LoadRenderingConfig();
	static void LoadGameConfig();


public:
	static inline const std::filesystem::path RESOURCES_DIR = "resources";
	static inline const std::filesystem::path CONFIG_FILE = "game.config";
	static inline const std::filesystem::path RENDERING_FILE = "rendering.config";
	static inline const std::filesystem::path SCENE_DIR = RESOURCES_DIR / "scenes";
	static inline const std::filesystem::path TEMPLATE_DIR = RESOURCES_DIR / "actor_templates";
	static inline const std::filesystem::path IMAGE_DIR = RESOURCES_DIR / "images";
	static inline const std::filesystem::path FONT_DIR = RESOURCES_DIR / "fonts";
	static inline const std::filesystem::path AUDIO_DIR = RESOURCES_DIR / "audio";
	static inline const std::filesystem::path SCRIPTS_DIR = RESOURCES_DIR / "component_types";

	static void Load();

	// hw4
	static inline std::string Initial_Scene = "";
	static inline std::string Game_Title = "";

	// hw5
	static inline std::string Game_Font_Name;

	// hw6
};
