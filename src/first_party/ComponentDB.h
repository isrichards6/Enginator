#pragma once
#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h" // lua must always be before luabridge
#include <unordered_map>
#include <string>
#include <vector>

class ComponentDB {
public:
	static inline lua_State* lua_state;
	static inline int runtime_components_counter = 0;

	static inline std::vector<luabridge::LuaRef> startup_components;
	static inline std::vector<luabridge::LuaRef> update_components;
	static inline std::vector<luabridge::LuaRef> late_update_components;

	static inline std::vector<luabridge::LuaRef> pending_startup_components;
	static inline std::vector<luabridge::LuaRef> pending_update_components;
	static inline std::vector<luabridge::LuaRef> pending_late_update_components;
	static inline std::vector<luabridge::LuaRef> pending_removal_components;

	static inline std::unordered_map<std::string, luabridge::LuaRef> component_types;

	static void ClearComponentLists() {
		startup_components.clear();
		update_components.clear();
		late_update_components.clear();
		pending_startup_components.clear();
		pending_update_components.clear();
		pending_late_update_components.clear();
		pending_removal_components.clear();
	}


	static void CppLog(const std::string& message);
	static void CppLogError(const std::string& message);
	static void Init();
	static void OnStart();
	static void OnUpdate();
	static void OnLateUpdate();
	static void EstablishInheritance(luabridge::LuaRef& instance_table, luabridge::LuaRef& parent_table);
	static void ProcessPendingComponentAdditions();
	static void ProcessPendingComponentRemovals();
	static luabridge::LuaRef GetBaseComponentRef(const std::string& component_type);
};

