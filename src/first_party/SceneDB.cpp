#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "SceneDB.h"
#include "Config.h"
#include "PrintUtils.h"
#include "FileUtils.h"
#include "Actor.h"
#include "TemplateDB.h"
#include <filesystem>
#include <string>
#include <vector>
#include <algorithm>
#include <document.h>

namespace fs = std::filesystem;

void SceneDB::LoadScene(const std::string& scene_name) {
	current_scene = scene_name;

	std::vector<std::shared_ptr<Actor>> saved_actors;
	// cache don't destroy actors
	for (auto actor : Scene_Actors) {
		if (actor->dont_destroy) {
			saved_actors.push_back(actor);
		}
	}

	Scene_Actors.clear();
	actors_by_name.clear();
	ComponentDB::ClearComponentLists();
	pending_addition_actors.clear();
	pending_removal_actors.clear();
	//actor_id_counter = 0; // Avoiding for now just to prevent id collisions

	// restore don't destroy actors
	for (auto actor : saved_actors) {
		Scene_Actors.push_back(actor);
		actors_by_name[actor->GetName()].push_back(actor.get());

		// restore components to queues, skipping startup
		for (auto& [key, component] : actor->components) {
			if (component["OnUpdate"].isFunction()) {
				ComponentDB::update_components.push_back(component);
			}
			if (component["OnLateUpdate"].isFunction()) {
				ComponentDB::late_update_components.push_back(component);
			}
		}
	}

	// check that scene file is valid
	fs::path scene_path = Config::SCENE_DIR / (scene_name + ".scene");
	if (!fs::exists(scene_path)) {
		print("error: scene ", scene_name, " is missing");
		exit(0);
	}

	// prep scene json file
	rapidjson::Document scene_doc;
	FileUtils::ReadJsonFile(scene_path.string(), scene_doc);

	const auto& actors_array = scene_doc["actors"].GetArray();

	Scene_Actors.reserve(actors_array.Size());

	// iterate through actors and load them into actors array
	for (const auto& actor_data : actors_array) {
		std::shared_ptr<Actor> actor = std::make_shared<Actor>();

		if (actor_data.HasMember("template")) {
			*actor = TemplateDB::LoadTemplate(actor_data["template"].GetString());
		}

		actor->Load(actor_data, static_cast<int>(actor_id_counter++));

		QueueLifecycleFunctions(actor.get());
		

		Scene_Actors.push_back(actor);
		actors_by_name[actor->GetName()].push_back(actor.get());
	}
}

void SceneDB::QueueLifecycleFunctions(Actor* actor) {
	for (auto& [key, component] : actor->components) {
		if (component["OnStart"].isFunction()) {
			ComponentDB::startup_components.push_back(component);
		}
		if (component["OnUpdate"].isFunction()) {
			ComponentDB::update_components.push_back(component);
		}
		if (component["OnLateUpdate"].isFunction()) {
			ComponentDB::late_update_components.push_back(component);
		}
	}
}

void SceneDB::QueuePendingLifecycleFunctions(Actor* actor) {
	for (auto& [key, component] : actor->components) {
		if (component["OnStart"].isFunction()) {
			ComponentDB::pending_startup_components.push_back(component);
		}
		if (component["OnUpdate"].isFunction()) {
			ComponentDB::pending_update_components.push_back(component);
		}
		if (component["OnLateUpdate"].isFunction()) {
			ComponentDB::pending_late_update_components.push_back(component);
		}

	}
}

Actor* SceneDB::Find(std::string name) {
	auto it = actors_by_name.find(name);

	if (it != actors_by_name.end() && !it->second.empty()) {
		return it->second.front();
	}
	return nullptr;
}

luabridge::LuaRef SceneDB::FindAll(std::string name) {
	luabridge::LuaRef actors = luabridge::newTable(ComponentDB::lua_state);
	int index = 1;

	auto it = actors_by_name.find(name);
	if (it != actors_by_name.end()) {
		for (auto* actor : it->second) {
			actors[index] = actor;
			index++;
		}
	}
	return actors;
}

Actor* SceneDB::InstantiateActor(const std::string& template_name) {
	std::shared_ptr<Actor> actor = std::make_shared<Actor>();
	*actor = TemplateDB::LoadTemplate(template_name);
	actor->id = actor_id_counter++;
	actor->InstantiateComponentTemplates();
	actors_by_name[actor->GetName()].push_back(actor.get());
	pending_addition_actors.push_back(actor);
	return actor.get();
}

void SceneDB::ProcessPendingActorAdditions() {
	for (auto& actor : pending_addition_actors) {
		Scene_Actors.push_back(actor);
		QueuePendingLifecycleFunctions(actor.get());
	}
	pending_addition_actors.clear();
}

void SceneDB::DestroyActor(Actor* actor) {
	if (actor == nullptr) return;

	// disable components
	for (auto& [key, component] : actor->components) {
		component["enabled"] = false;
	}

	// grab all actors using this name
	auto it = actors_by_name.find(actor->GetName());
	if (it != actors_by_name.end()) {
		auto& actor_list = it->second;
		// move target actor's instance to end
		auto new_end = std::remove(actor_list.begin(), actor_list.end(), actor);
		// cut off target actor's instance
		actor_list.erase(new_end,actor_list.end());

		// remove from actor lookup
		if (actor_list.empty()) {
			actors_by_name.erase(it);
		}
	}

	pending_removal_actors.push_back(actor);
}

void SceneDB::ProcessPendingActorRemovals() {
	for (auto& actor : pending_removal_actors) {
		// remove from scene_actors
		Scene_Actors.erase(
			std::remove_if(Scene_Actors.begin(), Scene_Actors.end(),
				[actor](const std::shared_ptr<Actor>& a) { return a.get() == actor; }),
			Scene_Actors.end()
		);
		// remove from pending_actors
		pending_addition_actors.erase(
			std::remove_if(pending_addition_actors.begin(), pending_addition_actors.end(),
				[actor](const std::shared_ptr<Actor>& a) { return a.get() == actor; }),
			pending_addition_actors.end()
		);
	}
	pending_removal_actors.clear();
}

void SceneDB::ProcessNewScene() {
	if (!pending_scene.empty()) {
		LoadScene(pending_scene);
		pending_scene = "";
	}
}

void SceneDB::DontDestroyOnLoad(Actor* actor) {
	actor->dont_destroy = true;
}