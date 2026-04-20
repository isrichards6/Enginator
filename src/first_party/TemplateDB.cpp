#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING

#include "TemplateDB.h"
#include <string>
#include <filesystem>
#include "Config.h"
#include "PrintUtils.h"
#include "rapidjson/document.h"
#include "FileUtils.h"
#include "Actor.h"

namespace fs = std::filesystem;

std::unordered_map<std::string, Actor> TemplateDB::templates;

Actor TemplateDB::LoadTemplate(const std::string& name) {
	// check if template already loaded, return the actor info if so
	if (templates.count(name) != 0) {
		return templates[name];
	}
	// if template not loaded, check if it exists, if not error & exit
	fs::path template_path = Config::TEMPLATE_DIR / (name + ".template");
	if (!fs::exists(template_path)) {
		print("error: template ", name, " is missing");
		exit(0);
	}

	rapidjson::Document template_doc;
	FileUtils::ReadJsonFile(template_path.string(), template_doc);

	Actor template_actor;
	template_actor.Load(template_doc, -1);

	templates[name] = template_actor;

	return template_actor;
}