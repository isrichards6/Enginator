#include "Actor.h"

luabridge::LuaRef Actor::InstantiateComponent(const std::string& component_name, const std::string& component_type) {
	// component loading
	luabridge::LuaRef base_component = ComponentDB::GetBaseComponentRef(component_type);
	
	luabridge::LuaRef component_instance = luabridge::newTable(ComponentDB::lua_state);
	ComponentDB::EstablishInheritance(component_instance, base_component);

	SetComponentBaseFields(component_instance, component_name, component_type);

	return component_instance;
}

void Actor::LoadComponentData(luabridge::LuaRef& component_instance, const rapidjson::Value& component_data) {
	// load components with specified component_data
	for (auto& property : component_data.GetObject()) {
		std::string property_name = property.name.GetString();

		if (property_name == "type") continue;

		if (property.value.IsString()) {
			component_instance[property_name] = property.value.GetString();
		}
		else if (property.value.IsInt()) {
			component_instance[property_name] = property.value.GetInt();
		}
		else if (property.value.IsNumber()) {
			component_instance[property_name] = property.value.GetFloat();
		}
		else if (property.value.IsBool()) {
			component_instance[property_name] = property.value.GetBool();
		}
	}
}

void Actor::SetComponentBaseFields(luabridge::LuaRef component, const std::string& name, std::string type) {
	component["key"] = name;
	component["type"] = type;
	component["enabled"] = true;
	component["actor"] = this;
}

void Actor::InstantiateComponentTemplates() {
	// template inheritance, need to make copy of template table so actor can uniquely modify
	for (auto& [key, template_component_table] : components) {
		luabridge::LuaRef component_from_template = luabridge::newTable(ComponentDB::lua_state);
		ComponentDB::EstablishInheritance(component_from_template, template_component_table);
		SetComponentBaseFields(component_from_template, key, template_component_table["type"].tostring());
		components.insert_or_assign(key, component_from_template);
	}
}

void Actor::LoadActorData (const rapidjson::Value& data) {
	// load components
	if (data.HasMember("components")) {
		const auto& component_data = data["components"].GetObject();

		for (auto& component : component_data) {
			std::string component_name = component.name.GetString();

			luabridge::LuaRef actor_component_instance(ComponentDB::lua_state);

			auto it = components.find(component_name);
			if (it != components.end()) { // component added by template
				actor_component_instance = it->second;
			}
			else { // new component
				actor_component_instance = InstantiateComponent(component_name, component.value["type"].GetString());
				components.insert_or_assign(component_name, actor_component_instance);

			}

			LoadComponentData(actor_component_instance, component.value);
		}
	}
}

void Actor::Load(const rapidjson::Value& data, int assigned_id) {
	// hw7
	FileUtils::Extract(data, "name", name);
	id = assigned_id;

	InstantiateComponentTemplates();

	LoadActorData(data);
}

std::string Actor::GetName() {
	return name;
}

int Actor::GetID() {
	return id;
}

luabridge::LuaRef Actor::GetComponentByKey(const std::string& key) {
	auto it = components.find(key);
	if (it != components.end()) {
		if (it->second["enabled"].cast<bool>() == true) {
			return it->second;
		}
	}
	return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::GetComponent(const std::string& type_name) {
	for (auto& [key, component] : components) {
		if (component["type"].tostring() == type_name && component["enabled"].cast<bool>() == true) {
			return component;
		}
	}
	return luabridge::LuaRef(ComponentDB::lua_state);
}

luabridge::LuaRef Actor::GetComponents(const std::string& type_name) {
	luabridge::LuaRef found_components = luabridge::newTable(ComponentDB::lua_state);
	int index = 1;

	for (auto& [key, component] : components) {
		if (component["type"].tostring() == type_name && component["enabled"].cast<bool>() == true) {
			found_components[index] = component;
			index++;
		}
	}
	return found_components;
}

luabridge::LuaRef Actor::AddComponent(const std::string& type_name) {
	std::string key = "r" + std::to_string(ComponentDB::runtime_components_counter++);
	luabridge::LuaRef component = InstantiateComponent(key, type_name);
	components.insert_or_assign(key, component);

	if (component["OnStart"].isFunction()) {
		ComponentDB::pending_startup_components.push_back(component);
	}
	if (component["OnUpdate"].isFunction()) {
		ComponentDB::pending_update_components.push_back(component);
	}
	if (component["OnLateUpdate"].isFunction()) {
		ComponentDB::pending_late_update_components.push_back(component);
	}

	return component;
}

void Actor::RemoveComponent(luabridge::LuaRef component_ref) {
	component_ref["enabled"] = false;
	ComponentDB::pending_removal_components.push_back(component_ref);
}

