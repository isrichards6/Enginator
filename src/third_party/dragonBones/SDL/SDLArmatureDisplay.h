#pragma once

/** @file SDLArmatureDisplay.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include <dragonBones/DragonBonesHeaders.h>
#include <SDL_render.h>

DRAGONBONES_NAMESPACE_BEGIN

class SDLArmatureProxy;
class SDLEventDispatcher;

class SDLArmatureDisplay
{
private:
	SDLArmatureProxy* _proxy = nullptr;

	SDL_FPoint _pos;

public:
	SDLArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName = "", const std::string& skinName = "", const std::string& textureAtlasName = "");
	~SDLArmatureDisplay();

	void setPosition(const SDL_FPoint& pos) { _pos = pos; }
	const SDL_FPoint& getPosition() { return _pos; }

	Armature* getArmature() const;
	Animation* getAnimation() const;

	SDLEventDispatcher* getEventDispatcher();

	SDLArmatureProxy* getArmatureProxy() const;

	SDL_FRect getBoundingBox();

	void draw(SDL_Renderer* renderer, float scaleX = 1.0f, float scaleY = 1.0f) const;
};

DRAGONBONES_NAMESPACE_END
