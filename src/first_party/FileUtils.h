#pragma once
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#define _SILENCE_CXX17_DEPRECATION_WARNINGS

#include <optional>
#include "rapidjson/document.h"
#include <string>
#include <vector>
#include "PrintUtils.h"

class FileUtils
{
public:
	static void ReadJsonFile(const std::string& path, rapidjson::Document& out_document);

	/// parses c++ data type value from json key. returns whether key exists.
	template <typename T>
	static bool Extract(const rapidjson::Value& source, const char* key, T& target) {
		auto itr = source.FindMember(key);
		if (itr != source.MemberEnd()) {
			const auto& val = itr->value;
			
			if constexpr (std::is_same_v<T, std::string>) {
				if (val.IsString()) target = val.GetString();
				else print("WARNING: JSON key '", key, "' expected string\n");
			}
			else if constexpr (std::is_same_v<T, int>) {
				if (val.IsNumber()) target = val.GetInt();
				else print("WARNING: JSON key '", key, "' expected number\n");
			}
			else if constexpr (std::is_same_v<T, bool>) {
				if (val.IsBool()) target = val.GetBool();
				else print("WARNING: JSON key '", key, "' expected bool\n");
			}
			else if constexpr (std::is_same_v<T, char>) {
				if (val.IsString()) target = val.GetString()[0];
				else print("WARNING: JSON key '", key, "' expected string\n");
			}
			else if constexpr (std::is_same_v<T, std::vector<std::string>>) {
				if (val.IsArray()) {
					for (const auto& element :val.GetArray()) {
						if (element.IsString()) {
							target.push_back(element.GetString());
						}
					}
				}
				else print("WARNING: JSON key '", key, "' expected array\n");
			}
			else if constexpr (std::is_same_v<T, float>) {
				if (val.IsNumber()) target = val.GetFloat();
				else print("WARNING: JSON key '", key, "' expected number\n");
			}
			else if constexpr (std::is_same_v<T, float>) {
				if (val.IsNumber()) target = val.GetFloat();
				else print("WARNING: JSON key '", key, "' expected number\n");
			}
			return true;
		}
		return false;
	}

	template <typename T>
	static void Extract(const rapidjson::Value& source, const char* key, std::optional<T>& target) {
		T temp_val;
		if (Extract(source, key, temp_val)) {
			target = temp_val;
		}
	}
};

