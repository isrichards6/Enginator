#include "ComponentDB.h"

#include "Actor.h"
#include "AudioDB.h"
#include "DragonBonesDB.h"
#include "Engine.h"
#include "ImageDB.h"
#include "Input.h"
#include "PrintUtils.h"
#include "Renderer.h"
#include "SceneDB.h"
#include "TemplateDB.h"
#include "TextDB.h"
#include "lua/lua.hpp" 
#include "LuaBridge/LuaBridge.h" // lua must always be before luabridge

void ReportError(const std::string& actor_name, const luabridge::LuaException& e) {
	std::string error_message = e.what();

	/* Normalize file paths across platforms */
	std::replace(error_message.begin(), error_message.end(), '\\', '/');

	/* Display (with color codes) */
	println("\033[31m", actor_name, " : ", error_message, "\033[0m");
}

void ComponentDB::CppLog(const std::string& message) {
	println(message);
}

void ComponentDB::CppLogError(const std::string& message) {
	println(message);
}

void ComponentDB::Init() {
	// lua startup
	lua_state = luaL_newstate();
	luaL_openlibs(lua_state);

	luabridge::getGlobalNamespace(lua_state)
		// classes
		.beginClass<Actor>("Actor")
			.addFunction("GetName", &Actor::GetName)
			.addFunction("GetID", &Actor::GetID)
			.addFunction("GetComponentByKey", &Actor::GetComponentByKey)
			.addFunction("GetComponent", &Actor::GetComponent)
			.addFunction("GetComponents", &Actor::GetComponents)
			.addFunction("AddComponent", &Actor::AddComponent)
			.addFunction("RemoveComponent", &Actor::RemoveComponent)
		.endClass()
		.beginClass<glm::vec2>("vec2")
			.addData("x", &glm::vec2::x)
			.addData("y", &glm::vec2::y)
		.endClass()
		.beginClass<dragonBones::SDLArmatureDisplay>("ArmatureDisplay")
		.endClass()

		//functions
		.beginNamespace("Debug")
			.addFunction("Log", &ComponentDB::CppLog)
			.addFunction("LogError", &ComponentDB::CppLogError)
		.endNamespace()
		.beginNamespace("Actor")
			.addFunction("Find", &SceneDB::Find)
			.addFunction("FindAll", &SceneDB::FindAll)
			.addFunction("Instantiate", &SceneDB::InstantiateActor)
			.addFunction("Destroy",&SceneDB::DestroyActor)
		.endNamespace()
		.beginNamespace("Application")
			.addFunction("Quit", &Engine::Quit)
			.addFunction("Sleep", &Engine::Sleep)
			.addFunction("GetFrame", &Renderer::GetFrame)
			.addFunction("OpenURL", &Engine::OpenURL)
		.endNamespace()
		.beginNamespace("Input")
			.addFunction("GetKey", &Input::GetKey)
			.addFunction("GetKeyDown", &Input::GetKeyDown)
			.addFunction("GetKeyUp", &Input::GetKeyUp)
			.addFunction("GetMousePosition", &Input::GetMousePosition)
			.addFunction("GetMouseButton", &Input::GetMouseButton)
			.addFunction("GetMouseButtonDown", &Input::GetMouseButtonDown)
			.addFunction("GetMouseButtonUp", &Input::GetMouseButtonUp)
			.addFunction("GetMouseScrollDelta", &Input::GetMouseScrollDelta)
			.addFunction("HideCursor", &Input::HideCursor)
			.addFunction("ShowCursor", &Input::ShowCursor)
		.endNamespace()
		.beginNamespace("Text")
			.addFunction("Draw", &TextDB::DrawText)
		.endNamespace()
		.beginNamespace("Audio")
			.addFunction("Play", &AudioDB::PlayChannel)
			.addFunction("Halt", &AudioDB::HaltChannel)
			.addFunction("SetVolume", &AudioDB::SetVolume)
		.endNamespace()
		.beginNamespace("Image")
			.addFunction("DrawUI", &ImageDB::DrawUI)
			.addFunction("DrawUIEx", &ImageDB::DrawUIEx)
			.addFunction("Draw", &ImageDB::Draw)
			.addFunction("DrawEx", &ImageDB::DrawEx)
			.addFunction("DrawPixel", &ImageDB::DrawPixel)
		.endNamespace()
		.beginNamespace("Camera")
			.addFunction("SetPosition", &Renderer::SetCameraPosition)
			.addFunction("GetPositionX", &Renderer::GetCameraPositionX)
			.addFunction("GetPositionY", &Renderer::GetCameraPositionY)
			.addFunction("SetZoom", &Renderer::SetCameraZoom)
			.addFunction("GetZoom", &Renderer::GetCameraZoom)
		.endNamespace()
		.beginNamespace("Scene")
			.addFunction("Load", &SceneDB::SetPendingScene)
			.addFunction("GetCurrent", &SceneDB::GetScene)
			.addFunction("DontDestroy", &SceneDB::DontDestroyOnLoad)
		.endNamespace()
		.beginNamespace("DragonBones")
			.addFunction("LoadData", &DragonBonesDB::LoadData)
			.addFunction("CreateArmature", &DragonBonesDB::CreateArmature)
			.addFunction("PlayAnimation", &DragonBonesDB::PlayAnimation)
			.addFunction("Draw", &DragonBonesDB::Draw)
			.addFunction("DestroyArmature", &DragonBonesDB::DestroyArmature)
		.endNamespace();
}

void RunLifecycle(std::vector<luabridge::LuaRef>& components, const char* function_name, bool clear_after_run) {
	for (auto& component : components) {
		if (!component["enabled"].cast<bool>()) continue;

		luabridge::LuaRef lifecycle_function = component[function_name];

		if (!lifecycle_function.isNil() && lifecycle_function.isFunction()) {
			try {
				lifecycle_function(component);
			}
			catch (const luabridge::LuaException& e) {
				std::string actor_name = component["actor"].cast<Actor*>()->name;
				ReportError(actor_name, e);
			}
		}
	}
	if (clear_after_run) components.clear();
}

void ComponentDB::OnStart() {
	RunLifecycle(startup_components, "OnStart", true);
}

void ComponentDB::OnUpdate() {
	RunLifecycle(update_components, "OnUpdate", false);
}

void ComponentDB::OnLateUpdate() {
	RunLifecycle(late_update_components, "OnLateUpdate", false);
}

void ComponentDB::EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table) {
	/* We must create a metatable to establish inheritance in lua. */
	luabridge::LuaRef new_metatable = luabridge::newTable(lua_state);
	new_metatable["__index"] = parent_table;

	/* We must use the raw lua C-API (lua stack) to perform a "setmetatable" operation. */
	instance_table.push(lua_state);
	new_metatable.push(lua_state);
	lua_setmetatable(lua_state, -2);
	lua_pop(lua_state, -1);
}

void ComponentDB::ProcessPendingComponentAdditions() {
	for (auto& component : pending_startup_components) {
		if (component["enabled"].cast<bool>() == true) startup_components.push_back(component);
	}
	for (auto& component : pending_update_components) {
		if (component["enabled"].cast<bool>() == true) update_components.push_back(component);
	}
	for (auto& component : pending_late_update_components) {
		if (component["enabled"].cast<bool>() == true) late_update_components.push_back(component);
	}

	pending_startup_components.clear();
	pending_update_components.clear();
	pending_late_update_components.clear();
}

void ComponentDB::ProcessPendingComponentRemovals() {
	for (auto& component : pending_removal_components) {
		Actor* actor = component["actor"].cast<Actor*>();
		std::string component_key = component["key"].cast<std::string>();
		actor->components.erase(component_key);

		// TODO: Lazy removal for now, might want to clean this up later
		// for (size_t i = update_components.size(); i-- > 0; ) {
		// 	if (update_components[i] == component) {
		// 		update_components.erase(update_components.begin() + i);
		// 	}
		// }
		// for (size_t i = late_update_components.size(); i-- > 0; ) {
		// 	if (late_update_components[i] == component) {
		// 		late_update_components.erase(late_update_components.begin() + i);
		// 	}
		// }
	}
	pending_removal_components.clear();
}

luabridge::LuaRef ComponentDB::GetBaseComponentRef(const std::string& component_type) {
	// cache hit
	auto it = component_types.find(component_type);;
	if (it != component_types.end()) {
		return it->second;
	}

	// cache miss, load file
	std::filesystem::path component_path = Config::SCRIPTS_DIR / (component_type + ".lua");

	if (!std::filesystem::exists(component_path)) {
		print("error: failed to locate component ", component_type);
		exit(0);
	}

	if (luaL_dofile(ComponentDB::lua_state, component_path.string().c_str()) != LUA_OK) {
		print("problem with lua file ", component_type);
		exit(0);
	}

	// add component base to cache
	luabridge::LuaRef base_component = luabridge::getGlobal(ComponentDB::lua_state, component_type.c_str());

	component_types.emplace(component_type, base_component);

	return base_component;
}