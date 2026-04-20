#pragma once
#include "Actor.h"
#include "glm/glm.hpp"
#include <string>
#include <unordered_map>
#include <vector>

class SceneDB
{
public:
	// hw7
	static void LoadScene(const std::string& scene_name);
	static void QueueLifecycleFunctions(Actor* actor);
	static void QueuePendingLifecycleFunctions(Actor* actor);
	static Actor* Find(std::string name);
	static luabridge::LuaRef FindAll(std::string name);

	static Actor* InstantiateActor(const std::string& template_name);

	static void ProcessPendingActorAdditions();
	static void ProcessPendingActorRemovals();

	static void DestroyActor(Actor* actor);

	static inline std::string current_scene = "";
	static inline std::string pending_scene = "";
	static inline void SetPendingScene(std::string scene_name) { pending_scene = scene_name; }
	static void ProcessNewScene();
	static inline std::string GetScene() { return current_scene; }
	static void DontDestroyOnLoad(Actor* actor);

	static inline std::vector<std::shared_ptr<Actor>> Scene_Actors;
	static inline std::unordered_map<std::string, std::vector<Actor*>> actors_by_name;
	static inline std::vector<std::shared_ptr<Actor>> pending_addition_actors;
	static inline std::vector<Actor*> pending_removal_actors;
	static inline int actor_id_counter = 0;

};
