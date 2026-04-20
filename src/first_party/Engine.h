#pragma once
#define _SILENCE_CXX17_ITERATOR_BASE_CLASS_DEPRECATION_WARNING
#include "Actor.h"
#include "Config.h"
#include <SDL_events.h>
#include <glm.hpp>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>

class Engine
{
public:
    // hw7
    void GameLoop();
    void StartUp();
    void Update();
    void ProcessNewScene();
    void Render();

    static void Quit();
    static void Sleep(int milliseconds);
    static int GetFrame();
    static void OpenURL(std::string url);
    
    bool game_running = true;
};
