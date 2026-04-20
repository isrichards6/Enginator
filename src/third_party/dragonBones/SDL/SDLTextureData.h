/** @file SDLTextureData.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#pragma once

#include <memory>

#include <dragonBones/DragonBonesHeaders.h>

#include <SDL_render.h>

DRAGONBONES_NAMESPACE_BEGIN

class SDLTextureData : public TextureData
{
	BIND_CLASS_TYPE_B(SDLTextureData);

public:
	SDL_Texture*			texture;

public:
	SDLTextureData()
	{
		_onClear();
	}

	virtual ~SDLTextureData()
	{
		_onClear();
	}

	void _onClear() override
	{
		texture = nullptr;

		TextureData::_onClear();
	}

	void setTexture(SDL_Texture *value)
	{
		int width, height;
		SDL_QueryTexture(value, nullptr, nullptr, &width, &height);

		region.x = 0.f;
		region.y = 0.f;
		region.width = static_cast<float>(width);
		region.height = static_cast<float>(height);

		texture = value;
	}
};

DRAGONBONES_NAMESPACE_END
