/** @file SDLFactory.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#pragma once

#include <string>
#include <vector>

#include <dragonBones/DragonBonesHeaders.h>

#include <SDL_render.h>

#include "SDLEventDispatcher.h"

DRAGONBONES_NAMESPACE_BEGIN

class SDLArmatureProxy;
class SDLTextureData;

class SDLFactory : public BaseFactory
{
protected:
	static DragonBones* _dragonBonesInstance;
	static SDLFactory* _factory;

	std::unique_ptr<SDLEventDispatcher> _soundEventDispatcher;

public:
	SDLFactory();
	~SDLFactory();

public:
	DragonBonesData* loadDragonBonesData(const std::string& filePath, const std::string& name = "");
	TextureAtlasData* loadTextureAtlasData(const std::string& filePath, SDL_Texture* atlasTexture, const std::string& name = "", float scale = 1.0f);
	SDLArmatureProxy* buildArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName = "", const std::string& skinName = "", const std::string& textureAtlasName = "") const;
	SDL_Texture* getTextureDisplay(const std::string& textureName, const std::string& dragonBonesName = "") const;

	void addSoundEventListener(const std::function<void(EventObject*)>& listener)
	{
		_soundEventDispatcher->addDBEventListener(EventObject::SOUND_EVENT, listener);
	}

	void update(float lastUpdate);

	static SDLFactory* get() { return _factory; }

protected:
	TextureAtlasData* _buildTextureAtlasData(TextureAtlasData* textureAtlasData, void* textureAtlas) const override;
	Armature* _buildArmature(const BuildArmaturePackage& dataPackage) const override;
	Slot* _buildSlot(const BuildArmaturePackage& dataPackage, const SlotData* slotData, Armature* armature) const override;
};

DRAGONBONES_NAMESPACE_END
