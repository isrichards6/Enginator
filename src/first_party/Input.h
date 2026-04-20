#pragma once

#include <string>

#include "SDL2/SDL.h"
#include <unordered_map>
#include <vec2.hpp>
#include <vector>

enum INPUT_STATE { INPUT_STATE_UP, INPUT_STATE_JUST_BECAME_DOWN, INPUT_STATE_DOWN, INPUT_STATE_JUST_BECAME_UP };

class Input {
public:
	static void Init(); // Call before main loop begins.
	static void ProcessEvent(const SDL_Event& e); // Call every frame at start of event loop.
	static void LateUpdate(); // Call at frame end.

	static bool GetKey(const std::string& keystring);
	static bool GetKeyDown(const std::string& keystring);
	static bool GetKeyUp(const std::string& keystring);

	static glm::vec2 GetMousePosition();

	static bool GetMouseButton(int button);
	static bool GetMouseButtonDown(int button);
	static bool GetMouseButtonUp(int button);
	static float GetMouseScrollDelta();

	static void HideCursor();
	static void ShowCursor();

	static INPUT_STATE GetState(SDL_Scancode keycode);

	static inline std::unordered_map<SDL_Scancode, INPUT_STATE> keyboard_states;
	static inline std::vector<SDL_Scancode> just_became_key_down;
	static inline std::vector<SDL_Scancode> just_became_key_up;

	static inline std::unordered_map<int, INPUT_STATE> mouse_states;
	static inline std::vector<int> just_became_mouse_down;
	static inline std::vector<int> just_became_mouse_up;

	static inline glm::vec2 mouse_pos = {0.0f, 0.0f};
	static inline float mouse_scroll_delta = 0.0f;
};