#pragma once
#include <string>
#include "Actor.h"
#include <unordered_map>

class TemplateDB
{
public:
	static std::unordered_map<std::string, Actor> templates;

	static Actor LoadTemplate(const std::string& name);
};

