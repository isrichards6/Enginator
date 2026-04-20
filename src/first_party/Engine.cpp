#include "Engine.h"
#include "PrintUtils.h"
#include "Config.h"
#include "SceneDB.h"
#include "Renderer.h"
#include "Helper.h"
#include "ImageDB.h"
#include <SDL_events.h>
#include <algorithm>
#include <glm.hpp>
#include <string>
#include <cassert>
#include <thread>

#include "AudioDB.h"
#include "AudioHelper.h"
#include "ComponentDB.h"
#include "DragonBonesDB.h"
#include "Input.h"
#include "SpineDB.h"
#include "TextDB.h"

void Engine::GameLoop() {
	StartUp();

	while (game_running) {
		SceneDB::ProcessNewScene();

		SceneDB::ProcessPendingActorAdditions();
		ComponentDB::ProcessPendingComponentAdditions();

		ComponentDB::OnStart();

		Update();
		ComponentDB::OnUpdate();
		ComponentDB::OnLateUpdate();
		
		Render();
		
		Input::LateUpdate();
		
		ComponentDB::ProcessPendingComponentRemovals();
		SceneDB::ProcessPendingActorRemovals();
	}
}

void Engine::StartUp() {
	Config::Load();
	ComponentDB::Init();
	SceneDB::LoadScene(Config::Initial_Scene);
	Renderer::Init(Config::Game_Title, Renderer::Resolution, Renderer::Window_Color);
	TextDB::Init(Config::Game_Font_Name);
	AudioDB::Init();
	Input::Init();
}

void Engine::Update() {
	SDL_Event event;

	while (Helper::SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT) {
			game_running = false;
		}
		Input::ProcessEvent(event);
	}
	DragonBonesDB::Update(1.0f / 60.0f);
}

void Engine::Render() {
	Renderer::Clear();
	ImageDB::ProcessSceneRequests();
	ImageDB::ProcessUIRequests();
	TextDB::RenderText();
	ImageDB::ProcessPixelRequests();
	DragonBonesDB::ProcessRenderRequests();
	Renderer::Present();
}

void Engine::Quit() {
	exit(0);
}

void Engine::Sleep(int milliseconds) {
	std::this_thread::sleep_for(std::chrono::milliseconds(milliseconds));
}

void Engine::OpenURL(std::string url) {
	std::string command;
	// Windows : start <url> (chosen via _WIN32 preprocessor variable)
#ifdef _WIN32
	command = "start " + url;
	// OSX : open <url> (chosen via __APPLE__ preprocessor variable)
#elif __APPLE__
	command = "open " + url;
	// Linux : xdg-open <url>
#else
	command = "xdg-open " + url;
#endif
	std::system(command.c_str());
}