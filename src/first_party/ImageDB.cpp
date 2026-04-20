#include "ImageDB.h"
#include "PrintUtils.h"
#include "Renderer.h"
#include "Config.h"
#include "SDL_image.h"
#include <SDL_render.h>
#include <cmath>
#include "Engine.h"
#include "Helper.h"

SDL_Texture* ImageDB::GetImage(const std::string& image_name)
{
	auto it = Image_Cache.find(image_name);

	// cache hit, return
	if (it != Image_Cache.end()) return it->second;

	// cache miss, cache then return
	auto img_path = Config::IMAGE_DIR / (image_name+ +".png");
	if (!std::filesystem::exists(img_path)) {
		print("error: missing image ", image_name);
		exit(0);
	}
	SDL_Texture* texture = IMG_LoadTexture(Renderer::GetRenderer(), img_path.string().c_str());

	return Image_Cache[image_name] = texture;
}

void ImageDB::RenderAndClearAllImages() {

}

void ImageDB::DrawUI(const std::string& image_name, float x, float y) {
	DrawUIEx(image_name, x, y, IDR.color.r, IDR.color.g, IDR.color.b, IDR.color.a, IDR.sorting_order);
}

void ImageDB::DrawUIEx(const std::string& image_name, float x, float y, float r, float g, float b, float a, float sorting_order) {
	ImageDrawRequest image;

	image.image_name = image_name;
	image.x = std::trunc(x);
	image.y = std::trunc(y);
	image.color.r = static_cast<Uint8>(r);
	image.color.g = static_cast<Uint8>(g);
	image.color.b = static_cast<Uint8>(b);
	image.color.a = static_cast<Uint8>(a);
	image.sorting_order = static_cast<int>(sorting_order);

	UI_Requests.push_back(image);
}

void ImageDB::Draw(const std::string& image_name, float x, float y) {
	DrawEx(image_name, x, y, IDR.rotation_degrees, IDR.scale_x, IDR.scale_y, IDR.pivot_x, IDR.pivot_y, IDR.color.r, IDR.color.g, IDR.color.b, IDR.color.a, IDR.sorting_order);
}

void ImageDB::DrawEx(const std::string& image_name, float x, float y, float rotation_degrees, float scale_x, float scale_y, float pivot_x, float pivot_y, float r, float g, float b, float a, float sorting_order) {
	ImageDrawRequest image;

	image.image_name = image_name;
	image.x = x;
	image.y = y;
	image.rotation_degrees = static_cast<int>(rotation_degrees);
	image.scale_x = scale_x;
	image.scale_y = scale_y;
	image.pivot_x = pivot_x;
	image.pivot_y = pivot_y;
	image.color.r = static_cast<Uint8>(r);
	image.color.g = static_cast<Uint8>(g);
	image.color.b = static_cast<Uint8>(b);
	image.color.a = static_cast<Uint8>(a);
	image.sorting_order = static_cast<int>(sorting_order);

	Scene_Requests.push_back(image);
}

void ImageDB::DrawPixel(float x, float y, float r, float g, float b, float a) {
		ImageDrawRequest pixel;
		pixel.x = std::trunc(x);
		pixel.y = std::trunc(y);
		pixel.color.r = static_cast<Uint8>(r);
		pixel.color.g = static_cast<Uint8>(g);
		pixel.color.b = static_cast<Uint8>(b);
		pixel.color.a = static_cast<Uint8>(a);

		Pixel_Requests.push_back(pixel); 
}

void ImageDB::ProcessSceneRequests() {
	std::stable_sort(Scene_Requests.begin(), Scene_Requests.end(), CompareBySortingOrder);

	// process scene images w/ zoom
	float zoom_factor = Renderer::Zoom_Factor;
	SDL_RenderSetScale(Renderer::GetRenderer(), zoom_factor, zoom_factor);

	for (const auto& image : Scene_Requests) {
		glm::vec2 image_world_pos = glm::vec2(image.x, image.y) - Renderer::GetCameraPosition();

		SDL_Texture* texture = GetImage(image.image_name);
		SDL_FRect texture_rect;
		Helper::SDL_QueryTexture(texture, &texture_rect.w, &texture_rect.h);

		// flip
		int flip_mode = SDL_FLIP_NONE;
		if (image.scale_x < 0) flip_mode |= SDL_FLIP_HORIZONTAL;
		if (image.scale_y < 0) flip_mode |= SDL_FLIP_VERTICAL;

		// apply scale
		texture_rect.w *= glm::abs(image.scale_x);
		texture_rect.h *= glm::abs(image.scale_y);

		// pivot
		SDL_FPoint pivot_point = {
			image.pivot_x * texture_rect.w,
			image.pivot_y * texture_rect.h
		};

		glm::ivec2 camera_dimensions = Renderer::GetCameraDimensions();

		texture_rect.x = image_world_pos.x * Renderer::PIXELS_PER_METER // world unit to pixel
											+ camera_dimensions.x * 0.5f // account for 0,0 being center, not top left
											* (1.0f / Renderer::Zoom_Factor) // undo zoom factor for math
											- pivot_point.x; // apply pivot
		texture_rect.y = image_world_pos.y * Renderer::PIXELS_PER_METER 
											+ camera_dimensions.y * 0.5f 
											* (1.0f / Renderer::Zoom_Factor) 
											- pivot_point.y;
		// tint and alpha
		SDL_SetTextureColorMod(texture, image.color.r, image.color.g, image.color.b);
		SDL_SetTextureAlphaMod(texture, image.color.a);

		// draw
		Helper::SDL_RenderCopyEx(0, "", Renderer::GetRenderer(), texture, NULL,
			&texture_rect, image.rotation_degrees, &pivot_point, static_cast<SDL_RendererFlip>(flip_mode));

		// reset tint and alpha
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}

	// set back to default (UI processed at scale 1)
	SDL_RenderSetScale(Renderer::GetRenderer(), 1.0f, 1.0f);
	Scene_Requests.clear();
}

void ImageDB::ProcessUIRequests() {
	std::stable_sort(UI_Requests.begin(), UI_Requests.end(), CompareBySortingOrder);

	for (const auto& image : UI_Requests) {
		SDL_Texture* texture = GetImage(image.image_name);
		SDL_FRect texture_rect = { image.x, image.y, 0, 0 };
		Helper::SDL_QueryTexture(texture, &texture_rect.w, &texture_rect.h);
		
		// tint and alpha
		SDL_SetTextureColorMod(texture, image.color.r, image.color.g, image.color.b);
		SDL_SetTextureAlphaMod(texture, image.color.a);

		// draw
		Helper::SDL_RenderCopyEx(0, "", Renderer::GetRenderer(), texture,
							NULL, &texture_rect, 0, NULL, SDL_FLIP_NONE);

		// reset tint and alpha
		SDL_SetTextureColorMod(texture, 255, 255, 255);
		SDL_SetTextureAlphaMod(texture, 255);
	}
	UI_Requests.clear();
}

void ImageDB::ProcessPixelRequests() {
	// render on top of everything
	SDL_SetRenderDrawBlendMode(Renderer::GetRenderer(), SDL_BLENDMODE_BLEND);

	for (const auto& request : Pixel_Requests) {
		SDL_SetRenderDrawColor(Renderer::GetRenderer(), request.color.r, request.color.g, request.color.b, request.color.a);
		SDL_RenderDrawPoint(Renderer::GetRenderer(), static_cast<int>(request.x), static_cast<int>(request.y));
	}
	SDL_SetRenderDrawBlendMode(Renderer::GetRenderer(), SDL_BLENDMODE_NONE);
	Pixel_Requests.clear();
}
