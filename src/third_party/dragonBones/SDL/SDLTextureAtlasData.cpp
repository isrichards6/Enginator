/** @file SDLTextureAtlasData.cpp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include "SDLTextureAtlasData.h"
#include "SDLTextureData.h"

#include <memory>
#include <SDL_render.h>

DRAGONBONES_NAMESPACE_BEGIN
	SDLTextureAtlasData::SDLTextureAtlasData()
{
	_onClear();
}

SDLTextureAtlasData::~SDLTextureAtlasData()
{
	_onClear();
}

void SDLTextureAtlasData::setRenderTexture(SDL_Texture* value)
{
	if (_renderTexture == value)
	{
		return;
	}

	_renderTexture = value;

	if (_renderTexture != nullptr)
	{
		for (const auto& pair : textures)
		{
			const auto textureData = static_cast<SDLTextureData*>(pair.second);

			if (textureData->texture == nullptr)
			{
				dragonBones::Rectangle region;
				region.x = textureData->region.x;
				region.y = textureData->region.y;
				region.width = textureData->rotated ? textureData->region.height : textureData->region.width;
				region.height = textureData->rotated ? textureData->region.width : textureData->region.height;

				textureData->texture = _renderTexture;
				textureData->region = std::move(region);
			}
		}
	}
}

TextureData* SDLTextureAtlasData::createTexture() const
{
	return BaseObject::borrowObject<SDLTextureData>();
}

DRAGONBONES_NAMESPACE_END
