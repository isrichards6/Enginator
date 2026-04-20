/** @file SDLFactory.cpp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include "SDLFactory.h"

#include <fstream>
#include <sstream>
#include <memory>

#include "SDLSlot.h"
#include "SDLTextureData.h"
#include "SDLTextureAtlasData.h"
#include "SDLArmatureProxy.h"
#include "SDLDisplay.h"
#include "SDLEventDispatcher.h"

DRAGONBONES_NAMESPACE_BEGIN

DragonBones* SDLFactory::_dragonBonesInstance;
SDLFactory* SDLFactory::_factory;

SDLFactory::SDLFactory()
{
	_factory = this;

	if (_dragonBonesInstance == nullptr)
	{
		_soundEventDispatcher = std::make_unique<SDLEventDispatcher>();

		_dragonBonesInstance = new DragonBones(_soundEventDispatcher.get());
	}

	_dragonBones = _dragonBonesInstance;
}

SDLFactory::~SDLFactory()
{
	clear();

	if (_dragonBonesInstance)
	{
		delete _dragonBonesInstance;
	}
}

DragonBonesData* SDLFactory::loadDragonBonesData(const std::string& filePath, const std::string& name)
{
	if (!name.empty())
	{
		const auto existedData = getDragonBonesData(name);

		if (existedData)
			return existedData;
	}

	std::stringstream data;

	std::ifstream json(filePath);

	if (json.bad())
		return nullptr;

	data << json.rdbuf();

	if (data.str().empty())
		return nullptr;

	return parseDragonBonesData(data.str().c_str(), name, 1.0f);
}

TextureAtlasData* SDLFactory::loadTextureAtlasData(const std::string& filePath, SDL_Texture* atlasTexture, const std::string& name, float scale)
{
	std::stringstream data;

	std::ifstream json(filePath);

	if (json.bad())
		return nullptr;

	data << json.rdbuf();

	if (data.str().empty())
		return nullptr;

	return static_cast<SDLTextureAtlasData*>(BaseFactory::parseTextureAtlasData(data.str().c_str(), atlasTexture, name, scale));
}

SDLArmatureProxy* SDLFactory::buildArmatureDisplay(const std::string& armatureName, const std::string& dragonBonesName, const std::string& skinName, const std::string& textureAtlasName) const
{
	const auto armature = buildArmature(armatureName, dragonBonesName, skinName, textureAtlasName);

	if (armature != nullptr)
	{
		_dragonBones->getClock()->add(armature);

		return static_cast<SDLArmatureProxy*>(armature->getDisplay());
	}

	return nullptr;
}

SDL_Texture* SDLFactory::getTextureDisplay(const std::string& textureName, const std::string& dragonBonesName) const
{
	const auto textureData = static_cast<SDLTextureData*>(_getTextureData(dragonBonesName, textureName));
	if (textureData != nullptr && textureData->texture != nullptr)
	{
		return textureData->texture;
	}

	return nullptr;
}

void SDLFactory::update(float lastUpdate)
{
	_dragonBonesInstance->advanceTime(lastUpdate);
}

TextureAtlasData* SDLFactory::_buildTextureAtlasData(TextureAtlasData* textureAtlasData, void* textureAtlas) const
{
	auto textureAtlasData_ = static_cast<SDLTextureAtlasData*>(textureAtlasData);

	if (textureAtlasData != nullptr)
	{
		if (textureAtlas != nullptr)
		{
			textureAtlasData_->setRenderTexture(static_cast<SDL_Texture*>(textureAtlas));
		}
		else
		{
			DRAGONBONES_ASSERT(false, "No atlas texture");
		}
	}
	else
	{
		textureAtlasData_ = BaseObject::borrowObject<SDLTextureAtlasData>();
	}

	return textureAtlasData_;
}

Armature* SDLFactory::_buildArmature(const BuildArmaturePackage& dataPackage) const
{
	const auto armature = BaseObject::borrowObject<Armature>();
	const auto armatureDisplay = new SDLArmatureProxy();

	armature->init(dataPackage.armature, armatureDisplay, armatureDisplay, _dragonBones);

	return armature;
}

Slot* SDLFactory::_buildSlot(const BuildArmaturePackage& dataPackage, const SlotData* slotData, Armature* armature) const
{
	auto slot = BaseObject::borrowObject<SDLSlot>();
	auto wrapperDisplay = new SDLDisplay();

	slot->init(slotData, armature, wrapperDisplay, wrapperDisplay);

	return slot;
}

DRAGONBONES_NAMESPACE_END
