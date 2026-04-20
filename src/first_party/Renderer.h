#pragma once
#include <glm.hpp>
#include <string>
#include <SDL_video.h>
#include <SDL_render.h>

class Renderer
{
private:
	static inline SDL_Window* window = nullptr;
	static inline SDL_Renderer* renderer = nullptr;

public:
	static const int PIXELS_PER_METER = 100;
	static inline glm::vec2 Camera_Pos = { 0.0f, 0.0f };
	static inline glm::ivec2 Resolution = { 640, 360 };
	static inline glm::ivec3 Window_Color = { 255, 255, 255 };
	static inline float Zoom_Factor = 1.0f;
	static inline float Cam_Ease_Factor = 1.0f;

	static inline SDL_Renderer* GetRenderer() { return renderer; }
	static inline glm::vec2 GetCameraPosition() { return Camera_Pos; }
	static inline glm::ivec2 GetCameraDimensions() { return Resolution; }

	static void Init(std::string title, glm::ivec2 resolution, glm::ivec3 window_color);
	static void Clear();
	static void Present();

	static void SetCameraPosition(float x, float y);
	static float GetCameraPositionX();
	static float GetCameraPositionY();
	static void SetCameraZoom(float zoom_factor);
	static float GetCameraZoom();
	static int GetFrame();
};