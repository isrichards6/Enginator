#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include "PrintUtils.h"
#include "FileUtils.h"
#include "Config.h"
#include <string>
#include <filesystem>
#include <cstdlib>
#include <document.h>

#include "Renderer.h"


namespace fs = std::filesystem;

void Config::Load() {
	if (!fs::is_directory(RESOURCES_DIR)) {
		print("error: ", RESOURCES_DIR.generic_string(), "/ missing");
		exit(0);
	}

	LoadGameConfig();
	LoadRenderingConfig();
}

void Config::LoadGameConfig() {
	auto config_path = RESOURCES_DIR / CONFIG_FILE;

	if (!fs::exists(config_path)) {
		print("error: ", CONFIG_FILE.generic_string(), " missing");
		exit(0);
	}

	rapidjson::Document game_doc;
	FileUtils::ReadJsonFile(config_path.string(), game_doc);

	/*FileUtils::Extract(game_doc, "game_start_message", Game_Start_Message);
	FileUtils::Extract(game_doc, "game_over_bad_message", Game_Over_Bad_Message);
	FileUtils::Extract(game_doc, "game_over_good_message", Game_Over_Good_Message);*/
	FileUtils::Extract(game_doc, "initial_scene", Initial_Scene);
	FileUtils::Extract(game_doc, "game_title", Game_Title);

	// hw5
	/*FileUtils::Extract(game_doc, "intro_image", Intro_Images);
	FileUtils::Extract(game_doc, "intro_text", Intro_Text);*/
	FileUtils::Extract(game_doc, "font", Game_Font_Name);
	/*FileUtils::Extract(game_doc, "intro_bgm", Intro_Audio);
	FileUtils::Extract(game_doc, "gameplay_audio", Gameplay_Audio);
	FileUtils::Extract(game_doc, "hp_image", HP_Image);
	FileUtils::Extract(game_doc, "game_over_bad_image", Game_Over_Bad_Image);
	FileUtils::Extract(game_doc, "game_over_bad_audio", Game_Over_Bad_Audio);
	FileUtils::Extract(game_doc, "game_over_good_image", Game_Over_Good_Image);
	FileUtils::Extract(game_doc, "game_over_good_audio", Game_Over_Good_Audio);*/

	// hw6
	/*FileUtils::Extract(game_doc, "player_movement_speed", Player_Movement_Speed);
	FileUtils::Extract(game_doc, "score_sfx", Score_SFX);*/

	if (Initial_Scene.empty()) {
		print("error: initial_scene unspecified");
		exit(0);
	}

	/*if (!Intro_Text.empty() && Game_Font_Name.empty()) {
		print("error: text render failed. No font configured");
		exit(0);
	}*/

	if (!Game_Font_Name.empty()) {
		auto font_path = FONT_DIR / (Game_Font_Name + ".ttf");
		if (!fs::exists(font_path)) {
			print("error: font ", Game_Font_Name, " missing");
			exit(0);
		}
	}


}

void Config::LoadRenderingConfig() {
	auto rendering_path = RESOURCES_DIR / RENDERING_FILE;
	rapidjson::Document rendering_doc;
	if (fs::exists(rendering_path)) {
		FileUtils::ReadJsonFile(rendering_path.string(), rendering_doc);
	}

	if (rendering_doc.IsObject()) {
		FileUtils::Extract(rendering_doc, "x_resolution", Renderer::Resolution.x);
		FileUtils::Extract(rendering_doc, "y_resolution", Renderer::Resolution.y);
		FileUtils::Extract(rendering_doc, "clear_color_r", Renderer::Window_Color.r);
		FileUtils::Extract(rendering_doc, "clear_color_g", Renderer::Window_Color.g);
		FileUtils::Extract(rendering_doc, "clear_color_b", Renderer::Window_Color.b);

		//// hw5
		//FileUtils::Extract(rendering_doc, "cam_offset_x", Cam_Offset.x);
		//FileUtils::Extract(rendering_doc, "cam_offset_y", Cam_Offset.y);

		// hw6
		FileUtils::Extract(rendering_doc, "zoom_factor", Renderer::Zoom_Factor);
		FileUtils::Extract(rendering_doc, "cam_ease_factor", Renderer::Cam_Ease_Factor);
		/*FileUtils::Extract(rendering_doc, "x_scale_actor_flipping_on_movement", X_Scale_Actor_Flipping_On_Movement);*/
	}
}