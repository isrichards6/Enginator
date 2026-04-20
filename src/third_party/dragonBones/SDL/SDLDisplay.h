/** @file SDLDisplay.h
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @author Patryk (PsychoX) Ludwikowski <psychoxivi@gmail.com>
 ** @license MIT License 
 **/

#pragma once

#include <vector>

#include <dragonBones/DragonBonesHeaders.h>

#include "SDLNode.h"

DRAGONBONES_NAMESPACE_BEGIN

class SDLDisplay : public SDLNode
{
public:
	SDL_Texture* texture = nullptr;

	std::vector<std::vector<int>> verticesInTriagles;

	std::vector<SDL_Vertex>	verticesDisplay;

	SDL_BlendMode blendMode = SDL_BLENDMODE_BLEND;

protected:

public:
	SDLDisplay() = default;
	~SDLDisplay() = default;

public:
	void setColor(const SDL_Color& color) override
	{
		for (auto& vert : verticesDisplay)
		{
			vert.color = color;
		}
	}

	SDL_FRect getBoundingBox() const override
	{
		if (texture == nullptr)
			return SDL_FRect();

		if (verticesDisplay.empty())
			return SDL_FRect();

		SDL_FPoint min = verticesDisplay[0].position;
		SDL_FPoint max = verticesDisplay[0].position;

		for (auto& vert : verticesDisplay)
		{
			min.x = std::min(min.x, vert.position.x);
			min.y = std::min(min.y, vert.position.y);
			max.x = std::max(max.x, vert.position.x);
			max.y = std::max(max.y, vert.position.y);
		}

		SDL_FRect rect;

		SDL_FPoint p1 = TransformPoint(min.x, min.y);
		SDL_FPoint p2 = TransformPoint(max.x, min.y);
		SDL_FPoint p3 = TransformPoint(min.x, max.y);
		SDL_FPoint p4 = TransformPoint(max.x, max.y);

		float newMinX = std::min({ p1.x, p2.x, p3.x, p4.x });
		float newMaxX = std::max({ p1.x, p2.x, p3.x, p4.x });
		float newMinY = std::min({ p1.y, p2.y, p3.y, p4.y });
		float newMaxY = std::max({ p1.y, p2.y, p3.y, p4.y });

		return SDL_FRect{ newMinX, newMinY, newMaxX - newMinX, newMaxY - newMinY };

		return rect;
	}

protected:
	void draw(SDL_Renderer* renderer, float offsetX = 0.0f, float offsetY = 0.0f, float scaleX = 1.0f, float scaleY = 1.0f) const override {
		if (_visible && texture != nullptr && !verticesDisplay.empty()) {
			SDL_SetTextureBlendMode(texture, blendMode);

			std::vector<SDL_Vertex> transformedVerts = verticesDisplay;
			for (auto& vert : transformedVerts) {
				// get local pos
				SDL_FPoint localPos = TransformPoint(vert.position.x, vert.position.y);

				// apply engine scale & flip
				localPos.x *= scaleX;
				localPos.y *= scaleY;

				// add screen pos
				vert.position.x = localPos.x + offsetX;
				vert.position.y = localPos.y + offsetY;
			}

			// normal texture
			if (transformedVerts.size() == 4) {
				const int indices[6] = { 0, 1, 2, 1, 3, 2 };
				SDL_RenderGeometry(renderer, texture, transformedVerts.data(), transformedVerts.size(), indices, 6);
			}
			// complex mesh
			else {
				SDL_RenderGeometry(renderer, texture, transformedVerts.data(), transformedVerts.size(), nullptr, 0);
			}
		}
	}

private:
	/// Applies DragonBones matrix to an X/Y coordinate
	inline SDL_FPoint TransformPoint(float x, float y) const {
		return SDL_FPoint{
			x * _transform.a + y * _transform.c + _transform.tx,
			x * _transform.b + y * _transform.d + _transform.ty
		};
	}
};

DRAGONBONES_NAMESPACE_END
