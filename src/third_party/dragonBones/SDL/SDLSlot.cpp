/** @file SDLSlot.cpp
 ** @author Piotr Krupa (piotrkrupa06@gmail.com)
 ** @author Patryk (PsychoX) Ludwikowski <psychoxivi@gmail.com>
 ** @license MIT License
 **/

#include "SDLSlot.h"

#include "SDLArmatureProxy.h"
#include "SDLDisplay.h"
#include "SDLTextureAtlasData.h"
#include "SDLTextureData.h"

DRAGONBONES_NAMESPACE_BEGIN

void SDLSlot::_updateVisible()
{
	_renderDisplay->setVisible(_parent->getVisible());
}

void SDLSlot::_updateBlendMode()
{
	if (_renderDisplay)
	{
		auto display = static_cast<SDLDisplay*>(_renderDisplay);

		switch (_blendMode)
		{
			case BlendMode::Normal:
				display->blendMode = SDL_BLENDMODE_BLEND;
				break;
			case BlendMode::Add:
				display->blendMode = SDL_BLENDMODE_ADD;
				break;
			case BlendMode::Multiply:
				display->blendMode = SDL_BLENDMODE_MOD;
				break;
			default:
				display->blendMode = SDL_BLENDMODE_BLEND;
				break;
		}
	}
	else if (_childArmature)
	{
		for (const auto slot : _childArmature->getSlots())
		{
			slot->_blendMode = _blendMode;
			slot->_updateBlendMode();
		}
	}
}

void SDLSlot::_updateColor()
{
	if (_renderDisplay)
	{
		SDL_Color color;

		color.a = static_cast<uint8_t>(_colorTransform.alphaMultiplier * 255.f);
		color.r = static_cast<uint8_t>(_colorTransform.redMultiplier * 255.f);
		color.g = static_cast<uint8_t>(_colorTransform.greenMultiplier * 255.f);
		color.b = static_cast<uint8_t>(_colorTransform.blueMultiplier * 255.f);

		_renderDisplay->setColor(color);
	}
}

void SDLSlot::_initDisplay(void* value, bool isRetain)
{
}

void SDLSlot::_disposeDisplay(void* value, bool isRelease)
{
	if (!isRelease && value)
	{
		delete static_cast<SDLNode*>(value);
	}
}

void SDLSlot::_onUpdateDisplay()
{
	_renderDisplay = static_cast<SDLNode*>(_display != nullptr ? _display : _rawDisplay);
	_renderDisplay->setZOffset(_slotData->zOrder);
}

void SDLSlot::_addDisplay()
{
	auto arm = static_cast<SDLArmatureProxy*>(_armature->getDisplay());
	arm->addNode(_renderDisplay);
}

void SDLSlot::_replaceDisplay(void* value, bool isArmatureDisplay)
{
	auto prevDisplay = static_cast<SDLNode*>(value);

	auto arm = static_cast<SDLArmatureProxy*>(_armature->getDisplay());

	_renderDisplay->setZOffset(prevDisplay->getZOffset());

	arm->removeNode(prevDisplay);
	arm->addNode(_renderDisplay);

	arm->sortNodes();

	_textureScale = 1.f;

}

void SDLSlot::_removeDisplay()
{
	auto arm = static_cast<SDLArmatureProxy*>(_armature->getDisplay());
	arm->removeNode(static_cast<SDLNode*>(_renderDisplay));
}

void SDLSlot::_updateZOrder()
{
	_renderDisplay->setZOffset(_slotData->zOrder);
	auto arm = static_cast<SDLArmatureProxy*>(_armature->getDisplay());
	arm->sortNodes();
}

void SDLSlot::_updateFrame()
{
	const auto currentVerticesData = (_deformVertices != nullptr && _display == _meshDisplay) ? _deformVertices->verticesData : nullptr;
	auto currentTextureData = static_cast<SDLTextureData*>(_textureData);

	auto display = static_cast<SDLDisplay*>(_renderDisplay);

	if (_displayIndex >= 0 && _display != nullptr && currentTextureData != nullptr)
	{
		if (currentTextureData->texture != nullptr)
		{
			if (currentVerticesData != nullptr) // Mesh
			{
				int texW, texH;
				SDL_QueryTexture(currentTextureData->texture, nullptr, nullptr, &texW, &texH);
				const auto data = currentVerticesData->data;
				const auto intArray = data->intArray;
				const auto floatArray = data->floatArray;
				const unsigned vertexCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
				const unsigned triangleCount = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshTriangleCount];
				int vertexOffset = intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshFloatOffset];

				if (vertexOffset < 0)
				{
					vertexOffset += 65536;
				}

				const unsigned uvOffset = vertexOffset + vertexCount * 2;

				const auto& region = currentTextureData->region;

				std::vector<SDL_Vertex> vertices(vertexCount);

				std::vector<std::vector<int>> verticesInTriagles;

				std::vector<uint16_t> vertexIndices(triangleCount * 3);

				for (std::size_t i = 0, l = vertexCount * 2; i < l; i += 2)
				{
					const auto iH = i / 2;

					const auto x = floatArray[vertexOffset + i];
					const auto y = floatArray[vertexOffset + i + 1];
					auto u = floatArray[uvOffset + i];
					auto v = floatArray[uvOffset + i + 1];

					SDL_Vertex vertexData;
					vertexData.position = { x, y };

					// normalize uvs
					if (currentTextureData->rotated) {
						vertexData.tex_coord.x = (region.x + (1.0f - v) * region.width) / static_cast<float>(texW);
						vertexData.tex_coord.y = (region.y + u * region.height) / static_cast<float>(texH);
					}
					else {
						vertexData.tex_coord.x = (region.x + u * region.width) / static_cast<float>(texW);
						vertexData.tex_coord.y = (region.y + v * region.height) / static_cast<float>(texH);
					}
					vertexData.color = SDL_Color{ 255, 255, 255, 255 };
					
					vertices[iH] = vertexData;
				}

				for (std::size_t i = 0; i < triangleCount * 3; ++i)
				{
					vertexIndices.push_back(intArray[currentVerticesData->offset + (unsigned)BinaryOffset::MeshVertexIndices + i]);
				}

				std::vector<SDL_Vertex> verticesDisplay;

				verticesInTriagles.resize(vertices.size());

				// sorting
				for (unsigned int i = 0; i < vertexIndices.size(); i++)
				{
					verticesInTriagles[vertexIndices[i]].push_back(i);
					verticesDisplay.push_back(vertices[vertexIndices[i]]);
				}

				_textureScale = 1.f;

				display->texture = currentTextureData->texture;
				display->verticesDisplay = std::move(verticesDisplay);
				display->verticesInTriagles = std::move(verticesInTriagles);

				const auto isSkinned = currentVerticesData->weight != nullptr;
				if (isSkinned)
				{
					_identityTransform();
				}
			}
			else // Normal texture
			{
				const auto scale = currentTextureData->parent->scale * _armature->_armatureData->scale;
				const auto height = (currentTextureData->rotated ? currentTextureData->region.width : currentTextureData->region.height) * scale;
				_textureScale = scale; 

				int texW, texH;
				SDL_QueryTexture(currentTextureData->texture, nullptr, nullptr, &texW, &texH);

				auto texRect =currentTextureData->region;

				display->texture = currentTextureData->texture;

				display->verticesDisplay.resize(4);

				display->verticesDisplay[0].tex_coord = SDL_FPoint{ texRect.x / texW, texRect.y / texH };
				display->verticesDisplay[1].tex_coord = SDL_FPoint{ texRect.x / texW, (texRect.y + texRect.height) / texH };
				display->verticesDisplay[2].tex_coord = SDL_FPoint{ (texRect.x + texRect.width) / texW, texRect.y / texH };
				display->verticesDisplay[3].tex_coord = SDL_FPoint{ (texRect.x + texRect.width) / texW, (texRect.y + texRect.height) / texH };

				float boundsWidth = static_cast<float>(std::abs(texRect.width));
				float boundsheight = static_cast<float>(std::abs(texRect.height));

				display->verticesDisplay[0].position = SDL_FPoint{0.f, 0.f};
				display->verticesDisplay[1].position = SDL_FPoint{0.f, boundsheight};
				display->verticesDisplay[2].position = SDL_FPoint{ boundsWidth, 0.f};
				display->verticesDisplay[3].position = SDL_FPoint{boundsWidth, boundsheight};

				display->setColor({255, 255, 255, 255});
			}

			_visibleDirty = true;
			_blendModeDirty = true;
			_colorDirty = true;

			return;
		}
	}

	_renderDisplay->setVisible(false);
}

void SDLSlot::_updateMesh()
{
	const auto scale = _armature->_armatureData->scale;
	const auto& deformVertices = _deformVertices->vertices;
	const auto& bones = _deformVertices->bones;
	const auto verticesData = _deformVertices->verticesData;
	const auto weightData = verticesData->weight;

	const auto hasFFD = !deformVertices.empty();
	const auto meshDisplay = static_cast<SDLDisplay*>(_renderDisplay);


	if (weightData != nullptr)
	{
		const auto data = verticesData->data;
		const auto intArray = data->intArray;
		const auto floatArray = data->floatArray;
		const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
		int weightFloatOffset = intArray[weightData->offset + (unsigned)BinaryOffset::WeigthFloatOffset];

		if (weightFloatOffset < 0)
		{
			weightFloatOffset += 65536;
		}

		for (
			std::size_t i = 0, iD = 0, iB = weightData->offset + (unsigned)BinaryOffset::WeigthBoneIndices + bones.size(), iV = (std::size_t)weightFloatOffset, iF = 0;
			i < vertexCount;
			++i
			)
		{
			const auto boneCount = (std::size_t)intArray[iB++];
			auto xG = 0.0f, yG = 0.0f;
			for (std::size_t j = 0; j < boneCount; ++j)
			{
				const auto boneIndex = (unsigned)intArray[iB++];
				const auto bone = bones[boneIndex];
				if (bone != nullptr)
				{
					const auto& matrix = bone->globalTransformMatrix;
					const auto weight = floatArray[iV++];
					auto xL = floatArray[iV++] * scale;
					auto yL = floatArray[iV++] * scale;

					if (hasFFD)
					{
						xL += deformVertices[iF++];
						yL += deformVertices[iF++];
					}

					xG += (matrix.a * xL + matrix.c * yL + matrix.tx) * weight;
					yG += (matrix.b * xL + matrix.d * yL + matrix.ty) * weight;
				}
			}

			auto& vertsDisplay = meshDisplay->verticesDisplay;

			for (auto vert : meshDisplay->verticesInTriagles[i])
			{
				auto& vertexPosition = vertsDisplay[vert].position;
				vertexPosition = { xG, yG };
			}
		}
	}
	else if (hasFFD)
	{
		const auto data = verticesData->data;
		const auto intArray = data->intArray;
		const auto floatArray = data->floatArray;
		const auto vertexCount = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshVertexCount];
		int vertexOffset = (std::size_t)intArray[verticesData->offset + (unsigned)BinaryOffset::MeshFloatOffset];

		if (vertexOffset < 0)
		{
			vertexOffset += 65536;
		}

		for (std::size_t i = 0, l = vertexCount * 2; i < l; i += 2)
		{
			const auto iH = i / 2;
			const auto xG = floatArray[vertexOffset + i] * scale + deformVertices[i];
			const auto yG = floatArray[vertexOffset + i + 1] * scale + deformVertices[i + 1];

			auto& vertsDisplay = meshDisplay->verticesDisplay;

			for (auto vert : meshDisplay->verticesInTriagles[iH])
			{
				auto& vertexPosition = vertsDisplay[vert].position;
				vertexPosition = { xG, yG };
			}
		}
	}
}

void SDLSlot::_identityTransform()
{
	_renderDisplay->setMatrix(Matrix(), SDL_FPoint(), _textureScale, _textureScale);
}

void SDLSlot::_updateTransform()
{
	SDL_FPoint pos{
		globalTransformMatrix.tx,
		globalTransformMatrix.ty
	};

	if (_renderDisplay == _rawDisplay || _renderDisplay == _meshDisplay)
	{
		pos.x -= globalTransformMatrix.a * _pivotX + globalTransformMatrix.c * _pivotY;
		pos.y -= globalTransformMatrix.b * _pivotX + globalTransformMatrix.d * _pivotY;
	}
	else
	{
		pos.x -= globalTransformMatrix.a - globalTransformMatrix.c;
		pos.y -= globalTransformMatrix.b - globalTransformMatrix.d;
	}

	_renderDisplay->setMatrix(globalTransformMatrix, pos, _textureScale, _textureScale);
}

void SDLSlot::_onClear()
{
	Slot::_onClear();

	_textureScale = 1.0f;
	_renderDisplay = nullptr;
}

DRAGONBONES_NAMESPACE_END
