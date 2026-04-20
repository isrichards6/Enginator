#include "Renderer.h"
#include "Helper.h"

void Renderer::Init(std::string title, glm::ivec2 resolution, glm::ivec3 window_color)
{
	window = Helper::SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, SDL_WINDOW_SHOWN);
	renderer = Helper::SDL_CreateRenderer(window, -1, SDL_RENDERER_PRESENTVSYNC | SDL_RENDERER_ACCELERATED);
	SDL_SetRenderDrawColor(renderer, window_color.r, window_color.g, window_color.b, SDL_ALPHA_OPAQUE);
}

void Renderer::Clear()
{
	SDL_SetRenderDrawColor(renderer, Window_Color.r, Window_Color.g, Window_Color.b, SDL_ALPHA_OPAQUE);
	SDL_RenderClear(renderer);
}

void Renderer::Present()
{
	Helper::SDL_RenderPresent(renderer);
}

void Renderer::SetCameraPosition(float x, float y) {
	Camera_Pos.x = x;
	Camera_Pos.y = y;
}

float Renderer::GetCameraPositionX() {
	return Camera_Pos.x;
}

float Renderer::GetCameraPositionY() {
	return Camera_Pos.y;
}

void Renderer::SetCameraZoom(float zoom_factor) {
	Zoom_Factor = zoom_factor;
}

float Renderer::GetCameraZoom() {
	return Zoom_Factor;
}

int Renderer::GetFrame() {
	return Helper::GetFrameNumber();
}