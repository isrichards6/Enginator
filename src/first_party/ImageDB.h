#pragma once
#include <string>
#include <unordered_map>
#include <SDL_render.h>
#include <vec3.hpp>
#include <vector>
#include "Colors.h"


struct ImageDrawRequest {
	std::string image_name = "";
	float x = 0.0f;
	float y = 0.0f;
	int rotation_degrees = 0;
	float scale_x = 1.0f;
	float scale_y = 1.0f;
	float pivot_x = 0.5f;
	float pivot_y = 0.5f;
	SDL_Color color = Colors::WHITE;
	int sorting_order = 0;

	ImageDrawRequest() = default;
};

static bool CompareBySortingOrder(const ImageDrawRequest& a, const ImageDrawRequest& b) {
	return a.sorting_order < b.sorting_order;
}

class ImageDB
{
public:
	static SDL_Texture* GetImage(const std::string& image_name);
	static void RenderAndClearAllImages();

	static inline std::unordered_map<std::string, SDL_Texture*> Image_Cache;

	static void DrawUI(const std::string& image_name, float x, float y);
	static void DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order);
	static void Draw(const std::string& image_name, float x, float y);
	static void DrawEx(const std::string& image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order);
	static void DrawPixel(float x, float y, float r, float g, float b, float a);

	static inline std::vector<ImageDrawRequest> Scene_Requests;
	static inline std::vector<ImageDrawRequest> UI_Requests;
	static inline std::vector<ImageDrawRequest> Pixel_Requests;

	static void ProcessSceneRequests();
	static void ProcessUIRequests();
	static void ProcessPixelRequests();

	static const inline ImageDrawRequest IDR;
};

