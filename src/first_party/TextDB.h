#pragma once
#include <string>
#include <SDL_pixels.h>
#include "SDL_ttf.h"
#include <unordered_map>
#include <vec2.hpp>
#include <vec4.hpp>
#include <vector>
#include "Config.h"
#include "Colors.h"

struct Text {
	std::string text_content = "";
	glm::ivec2 position = {0, 0};
	std::string font_name = "";
	int font_size = 0;
	SDL_Color font_color = Colors::WHITE;

	Text(const std::string& text_content, glm::ivec2 position, const std::string& font_name, int font_size, SDL_Color font_color)
		: text_content(text_content), position(position), font_name(font_name), font_size(font_size), font_color(font_color) {}
};

class TextDB {
public:
	static void DrawText(const std::string& text_content, float x, float y, const std::string& font_name,
		float font_size, float r, float g, float b, float a);
	static void Init(std::string font_name);
	static void RenderText();
	static inline std::unordered_map<std::string, SDL_Texture*> Text_Cache;
	static inline std::vector<Text> Text_Requests;
	static inline std::unordered_map<std::string, std::unordered_map<int, TTF_Font* >> Font_Cache;
};

