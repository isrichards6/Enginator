#include "TextDB.h"
#include "Config.h"
#include "Renderer.h"
#include <string>
#include <SDL_pixels.h>
#include "SDL_ttf.h"

#include "Helper.h"

void TextDB::DrawText(const std::string& text_content, float x, float y, const std::string& font_name, float font_size, float r, float g, float b, float a) {
	int size = static_cast<int>(font_size);
	TTF_Font* font;

	auto it_name = Font_Cache.find(font_name);
	if (it_name == Font_Cache.end() || it_name->second.find(size) == it_name->second.end()) { // font not found, add to cache
		font = TTF_OpenFont((Config::FONT_DIR / (font_name + ".ttf")).string().c_str(), size);
		if (!font) return; // TODO: Not sure what we're supposed to do in this case
		Font_Cache[font_name][size] = font;
	} 
	else {
		font = it_name->second[size];
	}

	SDL_Color font_color = { (static_cast<Uint8>(r)), static_cast<Uint8>(g), static_cast<Uint8>(b), static_cast<Uint8>(a) };
	glm::ivec2 font_pos = { static_cast<int>(x), static_cast<int>(y) };

	Text_Requests.emplace_back(Text(text_content, font_pos, font_name, size, font_color));
}

void TextDB::Init(std::string font_name) {
	TTF_Init();
}

void TextDB::RenderText() {
	for(auto& text : Text_Requests) {
		auto [it, inserted] = Text_Cache.insert({ text.text_content, nullptr});

		// if cache miss, cache texture
		if (inserted) {
			SDL_Surface* surface = TTF_RenderText_Solid(Font_Cache[text.font_name][text.font_size], text.text_content.c_str(), text.font_color);
			it->second = SDL_CreateTextureFromSurface(Renderer::GetRenderer(), surface);
			SDL_FreeSurface(surface);
		}
		
		SDL_Texture* texture = it->second;
		float w, h;

		// get texture dimensions
		Helper::SDL_QueryTexture(texture, &w, &h);

		SDL_FRect dest_rect = { static_cast<float>(text.position.x), static_cast<float>(text.position.y), w, h };
		Helper::SDL_RenderCopy(Renderer::GetRenderer(), texture, nullptr, &dest_rect);
	}
	Text_Requests.clear();
}
