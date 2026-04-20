#pragma once
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include <common.hpp>
#include <document.h>
#include <map>
#include <string>
#include <vec2.hpp>
#include <optional>
#include <unordered_set>

#include "ComponentDB.h"
#include "Config.h"
#include "FileUtils.h"
#include "Helper.h"

#include "lua/lua.hpp"
#include "LuaBridge/LuaBridge.h" // lua must always be before luabridge



struct Actor
{
	// hw7
	std::string name = "";
	int id = -1;
	std::map<std::string, luabridge::LuaRef> components;

	std::string GetName();
	int GetID();

	luabridge::LuaRef InstantiateComponent(const std::string& component_name, const std::string& component_type);
	void LoadComponentData(luabridge::LuaRef& component_instance, const rapidjson::Value& component_data);
	void SetComponentBaseFields(luabridge::LuaRef component, const std::string& name, std::string type);
	void InstantiateComponentTemplates();
	void LoadActorData(const rapidjson::Value& data);
	void Load(const rapidjson::Value& data, int assigned_id);

	luabridge::LuaRef GetComponentByKey(const std::string& key);
	luabridge::LuaRef GetComponent(const std::string& type_name);
	luabridge::LuaRef GetComponents(const std::string& type_name);

	luabridge::LuaRef AddComponent(const std::string& type_name);
	void RemoveComponent(luabridge::LuaRef component_ref);

	static bool CompareByID(const Actor* a, const Actor* b) {
		return a->id < b->id;
	}

	bool dont_destroy = false;
};
