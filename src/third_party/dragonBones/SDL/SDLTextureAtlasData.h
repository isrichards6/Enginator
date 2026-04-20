/** @file SDLTextureAtlasData.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#pragma once

#include <dragonBones/DragonBonesHeaders.h>
#include <SDL_render.h>

DRAGONBONES_NAMESPACE_BEGIN

class SDLTextureAtlasData : public TextureAtlasData
{
	BIND_CLASS_TYPE_B(SDLTextureAtlasData);

private:
	SDL_Texture* _renderTexture;

public:
	SDLTextureAtlasData();
	~SDLTextureAtlasData();

	virtual TextureData* createTexture() const override;

	SDL_Texture* getRenderTexture() const { return _renderTexture; }

	void setRenderTexture(SDL_Texture* value);
};

DRAGONBONES_NAMESPACE_END
