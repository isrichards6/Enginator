/** @file SDLArmatureDisplay.cpp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include <core/DragonBones.h>

#include "SDLArmatureDisplay.h"
#include "SDLArmatureProxy.h"
#include <SDL_render.h>
#include <SDL_rect.h>

DRAGONBONES_NAMESPACE_BEGIN

SDLArmatureDisplay::SDLArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName, const std::string& skinName, const std::string& textureAtlasName)
{
	_proxy = SDLFactory::get()->buildArmatureDisplay(armatureName, dragonBonesName, skinName, textureAtlasName);
}

SDLArmatureDisplay::~SDLArmatureDisplay()
{
	_proxy->dispose(true);
	_proxy = nullptr;
}

Armature* SDLArmatureDisplay::getArmature() const
{
	if (_proxy)
		return _proxy->getArmature();

	return nullptr;
}

Animation* SDLArmatureDisplay::getAnimation() const
{
	if (_proxy)
		return _proxy->getAnimation();

	return nullptr;
}

SDLEventDispatcher* SDLArmatureDisplay::getEventDispatcher()
{
	if (_proxy)
		return _proxy->getEventDispatcher();

	return nullptr;
}

SDLArmatureProxy* SDLArmatureDisplay::getArmatureProxy() const
{
	return _proxy;
}

SDL_FRect SDLArmatureDisplay::getBoundingBox()
{
	if (_proxy)
		return _proxy->getBoundingBox();

	return SDL_FRect();
}

void SDLArmatureDisplay::draw(SDL_Renderer* renderer, float scaleX, float scaleY) const {
	if (_proxy) {
		_proxy->draw(renderer, _pos.x, _pos.y, scaleX, scaleY);
	}
}

DRAGONBONES_NAMESPACE_END
