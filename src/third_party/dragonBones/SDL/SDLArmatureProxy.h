#pragma once

/** @file SDLArmatureProxy.hp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include <vector>

#include <dragonBones/DragonBonesHeaders.h>

#include "SDLNode.h"
#include "SDLFactory.h"
#include "SDLEventDispatcher.h"

DRAGONBONES_NAMESPACE_BEGIN

class SDLDisplay;
class SDLNode;

class SDLArmatureProxy : public SDLNode, public IArmatureProxy
{
	friend SDLFactory;

protected:
	Armature* _armature = nullptr;
	SDLEventDispatcher _dispatcher;

	SDL_FPoint _position;

	std::vector<SDLNode*> _nodes;

protected:
	SDLArmatureProxy() = default;
	~SDLArmatureProxy() = default;

public:
	bool hasDBEventListener(const std::string& type) const override { return true; }
	void addDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener) override;
	void removeDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener) override;
	void dispatchDBEvent(const std::string& type, EventObject* value) override;

	SDLEventDispatcher* getEventDispatcher() { return &_dispatcher; }

	void addNode(SDLNode* node);
	void removeNode(SDLNode* node);
	void sortNodes();

	void dbInit(Armature* armature) override;
	void dbClear() override;
	void dbUpdate() override;

	void dispose(bool disposeProxy) override;

	Armature* getArmature() const override { return _armature; }
	Animation* getAnimation() const override { return _armature->getAnimation(); }

	void setVisible(bool visible) override;
	void setColor(const SDL_Color& color) override;

	SDL_FRect getBoundingBox() const override;

	void draw(SDL_Renderer* renderer, float offsetX = 0.0f, float offsetY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f) const override;

};

DRAGONBONES_NAMESPACE_END
