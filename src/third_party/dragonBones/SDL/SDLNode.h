/** @file SDLNode.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @license MIT License
 **/

#pragma once

#include <dragonBones/DragonBonesHeaders.h>
#include <SDL_render.h>

DRAGONBONES_NAMESPACE_BEGIN

class SDLNode
{
protected:
	dragonBones::Matrix _transform;

	bool _visible = true;

	int _zOffset = 0;

public:
	SDLNode() = default;
	virtual ~SDLNode() = default;

	void setZOffset(int zOffset) { _zOffset = zOffset; }
	int getZOffset() const { return _zOffset; }

	void setMatrix(const Matrix& matrix, const SDL_FPoint& offset, float scaleX, float scaleY)
	{
		_transform.a = matrix.a * scaleX;
		_transform.c = matrix.c * scaleY;
		_transform.tx = offset.x;
		_transform.b = matrix.b * scaleX;
		_transform.d = matrix.d * scaleY;
		_transform.ty = offset.y;
	}

	virtual void setVisible(bool visible) { _visible = visible; }

	virtual void setColor(const SDL_Color& color) = 0;

	virtual SDL_FRect getBoundingBox() const = 0;

	virtual void draw(SDL_Renderer* renderer, float offsetX = 0.0f, float offsetY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f) const = 0;
};

DRAGONBONES_NAMESPACE_END
