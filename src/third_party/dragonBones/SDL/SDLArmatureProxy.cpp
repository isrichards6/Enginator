/** @file SDLArmatureProxy.cpp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#include "SDLArmatureProxy.h"

#include "SDLSlot.h"
#include "SDLDisplay.h"

DRAGONBONES_NAMESPACE_BEGIN

void SDLArmatureProxy::dbInit(Armature* armature)
{
	_armature = armature;
}

void SDLArmatureProxy::dbClear()
{
	_armature = nullptr;
}

void SDLArmatureProxy::dbUpdate()
{
}

void SDLArmatureProxy::addDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener)
{
	_dispatcher.addDBEventListener(type, listener);
}

void SDLArmatureProxy::removeDBEventListener(const std::string& type, const std::function<void(EventObject*)>& listener)
{
}

void SDLArmatureProxy::dispatchDBEvent(const std::string& type, EventObject* value)
{
	_dispatcher.dispatchDBEvent(type, value);
}

void SDLArmatureProxy::addNode(SDLNode* node)
{
	_nodes.push_back(node);
}

void SDLArmatureProxy::removeNode(SDLNode* node)
{
	auto it = std::find_if(_nodes.begin(), _nodes.end(), [=] (SDLNode* _node) { return node == _node; });

	if (it != _nodes.end())
	{
		_nodes.erase(it);
	}
}

void SDLArmatureProxy::sortNodes()
{
	std::sort(_nodes.begin(), _nodes.end(), [&] (SDLNode* a, SDLNode* b) {
		return (a->getZOffset() < b->getZOffset());
	});
}

void SDLArmatureProxy::dispose(bool disposeProxy)
{
	if (_armature)
	{
		_armature->dispose();
		_armature = nullptr;
	}
}

SDL_FRect SDLArmatureProxy::getBoundingBox() const
{
	auto slots = _armature->getSlots();
	bool isFirst = true;

	SDL_FPoint min;
	SDL_FPoint max;

	for (const auto slot : _armature->getSlots())
	{
		if (!slot->getVisible() || !slot->getDisplay())
		{
			continue;
		}

		auto display = static_cast<SDLDisplay*>(slot->getRawDisplay());
		const auto bounds = display->getBoundingBox();
		if (isFirst)
		{
			isFirst = false;
			min = { bounds.x, bounds.y };
			max = { bounds.x + bounds.w, bounds.y + bounds.h };
		}
		else
		{
			min.x = std::min(min.x, bounds.x);
			min.y = std::min(min.y, bounds.y);
			max.x = std::max(max.x, bounds.x + bounds.w);
			max.y = std::max(max.y, bounds.y + bounds.h);
		}
	}

	return SDL_FRect{ min.x, min.y, max.x - min.x, max.y - min.y };
}

void SDLArmatureProxy::setVisible(bool visible)
{
	for (auto node : _nodes)
	{
		node->setVisible(visible);
	}
}

void SDLArmatureProxy::setColor(const SDL_Color& color)
{
	for (auto node : _nodes)
	{
		node->setColor(color);
	}
}

void SDLArmatureProxy::draw(SDL_Renderer* renderer, float offsetX, float offsetY, float scaleX, float scaleY) const {
	float currentOffsetX = offsetX + _transform.tx;
	float currentOffsetY = offsetY + _transform.ty;

	for (auto node : _nodes) {
		if (!node) continue;
		node->draw(renderer, currentOffsetX, currentOffsetY, scaleX, scaleY);
	}
}

DRAGONBONES_NAMESPACE_END
