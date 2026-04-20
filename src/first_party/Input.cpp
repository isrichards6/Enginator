#include "Input.h"

#include <string>

#include "keycode_to_scancode.h"

void Input::Init() {
	keyboard_states.clear();
	just_became_key_down.clear();
	just_became_key_up.clear();

	mouse_states.clear();
	just_became_mouse_down.clear();
	just_became_mouse_up.clear();
}

void Input::ProcessEvent(const SDL_Event& e) {
	if (e.type == SDL_KEYDOWN) {
		SDL_Scancode key = e.key.keysym.scancode;
		keyboard_states[key] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_key_down.push_back(key);
	} 
	else if (e.type == SDL_KEYUP) {
		SDL_Scancode key = e.key.keysym.scancode;
		keyboard_states[key] = INPUT_STATE_JUST_BECAME_UP;
		just_became_key_up.push_back(key);
	} 
	else if (e.type == SDL_MOUSEMOTION) {
		mouse_pos.x = static_cast<float>(e.motion.x);
		mouse_pos.y = static_cast<float>(e.motion.y);
	}
	else if (e.type == SDL_MOUSEBUTTONDOWN) {
		mouse_states[e.button.button] = INPUT_STATE_JUST_BECAME_DOWN;
		just_became_mouse_down.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		mouse_states[e.button.button] = INPUT_STATE_JUST_BECAME_UP;
		just_became_mouse_up.push_back(e.button.button);
	}
	else if (e.type == SDL_MOUSEWHEEL) {
		mouse_scroll_delta += e.wheel.preciseY;
	}

}

void Input::LateUpdate() {
	for (SDL_Scancode key : just_became_key_down) {
		keyboard_states[key] = INPUT_STATE_DOWN;
	}

	for (SDL_Scancode key : just_became_key_up) {
		keyboard_states[key] = INPUT_STATE_UP;
	}

	just_became_key_down.clear();
	just_became_key_up.clear();

	for (int key : just_became_mouse_down) {
		mouse_states[key] = INPUT_STATE_DOWN;
	}

	for (int key : just_became_mouse_up) {
		mouse_states[key] = INPUT_STATE_UP;
	}

	just_became_mouse_down.clear();
	just_became_mouse_up.clear();

	mouse_scroll_delta = 0.0f;
}

INPUT_STATE Input::GetState(SDL_Scancode keycode) {
	auto it = keyboard_states.find(keycode);
	if (it == keyboard_states.end()) return INPUT_STATE_UP;
	return it->second;
}

bool Input::GetKey(const std::string& keystring) {
	SDL_Scancode keycode;
	if (!FindScancode(keystring, keycode)) return false;
	INPUT_STATE state = GetState(keycode);
	return state == INPUT_STATE_DOWN || state == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyDown(const std::string& keystring) {
	SDL_Scancode keycode;
	if (!FindScancode(keystring, keycode)) return false;
	return GetState(keycode) == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetKeyUp(const std::string& keystring) {
	SDL_Scancode keycode;
	if (!FindScancode(keystring, keycode)) return false;
	return GetState(keycode) == INPUT_STATE_JUST_BECAME_UP;
}

glm::vec2 Input::GetMousePosition() {
	return mouse_pos;
}

bool Input::GetMouseButton(int button) {
	auto it = mouse_states.find(button);
	if (it == mouse_states.end()) return false;
	return it->second == INPUT_STATE_DOWN || it->second == INPUT_STATE_JUST_BECAME_DOWN;
}

bool Input::GetMouseButtonUp(int button) {
	auto it = mouse_states.find(button);
	if (it == mouse_states.end()) return false;
	return it->second == INPUT_STATE_JUST_BECAME_UP;
}

bool Input::GetMouseButtonDown(int button) {
	auto it = mouse_states.find(button);
	if (it == mouse_states.end()) return false;
	return it->second == INPUT_STATE_JUST_BECAME_DOWN;
}

float Input::GetMouseScrollDelta() {
	return mouse_scroll_delta;
}

void Input::HideCursor() {
	SDL_ShowCursor(SDL_DISABLE);
}

void Input::ShowCursor() {
	SDL_ShowCursor(SDL_ENABLE);
}