//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Frame Transition
//
// Copyright(c) 2023-2025 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#include "FrameTransition.hpp"

#include <cmath> // for lround
#include <cstdint> // for uint8_t

namespace
{

inline float linearInterpolation(const float a, const float b, const float alpha)
{
	return (a * (1.f - alpha)) + (b * alpha);
}
inline sf::Color colorFromColorAndAlpha(sf::Color color, const unsigned int alpha)
{
	color.a = static_cast<std::uint8_t>(alpha);
	return color;
}

} // namespace

namespace selbaward
{

FrameTransition::FrameTransition(const sf::Vector2f size)
	: m_ratio{ 0.f }
	, m_size{}
	, m_transitionId{}
	, m_drawAOverB{ true }
	, m_parameter1{}
	, m_parameter2{}
	, m_frameA{}
	, m_frameB{}

	// mutable
	, m_isUpdateRequired{ true }
	, m_vertices()
{
	setTransition(T::TexCrop_A_Start | T::TexCrop_B_End | T::Direction_Right);
}
void FrameTransition::setSize(const sf::Vector2f size)
{
	m_isUpdateRequired = true;
	m_size = size;
}
void FrameTransition::setDrawOrderToAOverB()
{
	m_drawAOverB = true;
}
void FrameTransition::setDrawOrderToBOverA()
{
	m_drawAOverB = false;
}

void FrameTransition::setPercentage(const float percentage)
{
	setRatio(percentage / 100.f);
}
void FrameTransition::setRatio(const float ratio)
{
	m_isUpdateRequired = true;
	m_ratio = ratio;
	if (m_ratio < 0.f)
		m_ratio = 0.f;
	else if (m_ratio > 1.f)
		m_ratio = 1.f;
}
void FrameTransition::setTransition(const T transitionId)
{
	m_isUpdateRequired = true;
	m_transitionId = transitionId;

	switch (m_transitionId & T::Type)
	{
	case T::Type_Zoom:
	case T::Type_TexCrop:
	default:
		m_frameA.numberOfVertices = 6u;
		m_frameB.numberOfVertices = 6u;
	}
}

void FrameTransition::setColors(const sf::Color colorA, const sf::Color colorB)
{
	setColor(FrameId::A, colorA);
	setColor(FrameId::B, colorB);
}
void FrameTransition::setColors(const sf::Color color)
{
	setColors(color, color);
}
void FrameTransition::setColor(const FrameId frameId, const sf::Color color)
{
	priv_getFrame(frameId).color = color;
}
sf::Color FrameTransition::getColor(const FrameId frameId) const
{
	return priv_getFrame(frameId).color;
}

void FrameTransition::setTextures(const sf::Texture& textureA, const sf::Texture& textureB)
{
	setTexture(FrameId::A, textureA);
	setTexture(FrameId::B, textureB);
}
void FrameTransition::setTextures(const sf::Texture& texture)
{
	setTextures(texture, texture);
}
void FrameTransition::setTextures()
{
	setTexture(FrameId::A);
	setTexture(FrameId::B);
}
void FrameTransition::setTexture(const FrameId frameId, const sf::Texture& texture, const bool resetRect)
{
	m_isUpdateRequired = true;
	Frame& frame{ priv_getFrame(frameId) };
	frame.pTexture = &texture;
	if (resetRect)
	{
		frame.textureRect.size = sf::Vector2i(frame.pTexture->getSize());
	}
}
void FrameTransition::setTexture(const FrameId frameId)
{
	m_isUpdateRequired = true;
	priv_getFrame(frameId).pTexture = nullptr;
}
void FrameTransition::setTextureRect(const FrameId frameId, const sf::IntRect& textureRect)
{
	m_isUpdateRequired = true;
	priv_getFrame(frameId).textureRect = textureRect;
}
const sf::Texture& FrameTransition::getTexture(const FrameId frameId) const
{
	return *(priv_getFrame(frameId).pTexture);
}

void FrameTransition::setParameter1(const float parameterValue)
{
	m_parameter1 = parameterValue;
}
void FrameTransition::setParameter2(const float parameterValue)
{
	m_parameter2 = parameterValue;
}
float FrameTransition::getParameter1() const
{
	return m_parameter1;
}
float FrameTransition::getParameter2() const
{
	return m_parameter2;
}
void FrameTransition::resetParameters()
{
	setParameter1();
	setParameter2();
}

sf::FloatRect FrameTransition::getLocalBounds() const
{
	return{ { 0.f, 0.f }, m_size };
}
sf::FloatRect FrameTransition::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}



// PRIVATE
void FrameTransition::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();

	if (m_isUpdateRequired)
		priv_update();

	if (m_frameA.pTexture == m_frameB.pTexture)
	{
		states.texture = m_frameA.pTexture;
		if (m_drawAOverB)
			target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Triangles, states);
		else
		{
			target.draw(m_vertices.data() + m_frameB.numberOfVertices, m_frameA.numberOfVertices, sf::PrimitiveType::Triangles, states);
			target.draw(m_vertices.data(), m_frameB.numberOfVertices, sf::PrimitiveType::Triangles, states);
		}
	}
	else
	{
		if (m_drawAOverB)
		{
			states.texture = m_frameB.pTexture;
			target.draw(m_vertices.data(), m_frameB.numberOfVertices, sf::PrimitiveType::Triangles, states);
			states.texture = m_frameA.pTexture;
			target.draw(m_vertices.data() + m_frameB.numberOfVertices, m_frameA.numberOfVertices, sf::PrimitiveType::Triangles, states);
		}
		else
		{
			states.texture = m_frameA.pTexture;
			target.draw(m_vertices.data() + m_frameB.numberOfVertices, m_frameA.numberOfVertices, sf::PrimitiveType::Triangles, states);
			states.texture = m_frameB.pTexture;
			target.draw(m_vertices.data(), m_frameB.numberOfVertices, sf::PrimitiveType::Triangles, states);
		}
	}
}
void FrameTransition::priv_update() const
{
	m_isUpdateRequired = false;
	m_vertices.resize(m_frameA.numberOfVertices + m_frameB.numberOfVertices);



	enum class Type
	{
		TexCrop,
		Zoom,
	} type;

	if ((m_transitionId & T::Type) == T::Type_Zoom)
		type = Type::Zoom;
	else
		type = Type::TexCrop;

	switch (type)
	{
	case Type::Zoom:
		priv_updateFromZoom();
		break;
	case Type::TexCrop:
		priv_updateFromTexCrop();
		break;
	}



	sf::Color colorA{};
	sf::Color colorB{};

	switch (m_transitionId & T::Fade_A)
	{
	case T::Fade_A_Step:
		colorA = (m_ratio > 0.5f) ? colorFromColorAndAlpha(m_frameA.color, 0u) : m_frameA.color;
		break;
	case T::Fade_A_Linear:
		colorA = colorFromColorAndAlpha(m_frameA.color, std::lround(m_frameA.color.a * (1.f - m_ratio)));
		break;
	case T::Fade_A_Off:
		colorA = colorFromColorAndAlpha(m_frameA.color, 0u);
		break;
	case T::Fade_A_None:
		colorA = m_frameA.color;
		break;
	}
	switch (m_transitionId & T::Fade_B)
	{
	case T::Fade_B_Step:
		colorB = (m_ratio > 0.5f) ? m_frameB.color : colorFromColorAndAlpha(m_frameB.color, 0u);
		break;
	case T::Fade_B_Linear:
		colorB = colorFromColorAndAlpha(m_frameB.color, std::lround(m_frameA.color.a * m_ratio));
		break;
	case T::Fade_B_Off:
		colorB = colorFromColorAndAlpha(m_frameB.color, 0u);
		break;
	case T::Fade_B_None:
		colorB = m_frameB.color;
		break;
	}

	for (std::size_t i{ 0u }; i < m_frameB.numberOfVertices; ++i)
		m_vertices[i].color = colorB;
	for (std::size_t i{ 0u }; i < m_frameA.numberOfVertices; ++i)
		m_vertices[m_frameB.numberOfVertices + i].color = colorA;
}
void FrameTransition::priv_updateFromTexCrop() const
{
	float ratio{ m_ratio };
	float inverseRatio{ 1.f - m_ratio };
	bool swapDirection{ false };



	enum class Direction
	{
		Right,
		Left,
		Down,
		Up,
	} direction;
	if ((m_transitionId & T::Direction_Up) == T::Direction_Up)
		direction = Direction::Up;
	else if ((m_transitionId & T::Direction_Left) != T::None)
		direction = Direction::Left;
	else if ((m_transitionId & T::Direction_Down) != T::None)
		direction = Direction::Down;
	else
		direction = Direction::Right;

	switch (direction)
	{
	case Direction::Left:
	case Direction::Up:
		swapDirection = true;
		break;
	}



	// front is left/up (depending on horizontal/vertical direction)
	// back is right/down (depending on horizontal/vertical direction)
	// note that this is different from "start" and "end" which refers to different edges depending on direction (e.g. start is right edge when direction is left)
	enum class TexCrop
	{
		None,
		Front,
		Back,
		Both,
		Shuffle,
	};
	TexCrop texCropA{ TexCrop::None };
	TexCrop texCropB{ TexCrop::None };

	T transitionTexCropA{ m_transitionId & T::TexCrop_A };
	T transitionTexCropB{ m_transitionId & T::TexCrop_B };

	if ((transitionTexCropA & T::TexCrop_A_Shuffle) != T::None)
		texCropA = TexCrop::Shuffle;
	else if ((m_transitionId & T::TexCrop_A_Both) == T::TexCrop_A_Both)
		texCropA = TexCrop::Both;
	else if ((m_transitionId & T::TexCrop_A_Start) != T::None)
		texCropA = TexCrop::Front;
	else if ((m_transitionId & T::TexCrop_A_End) != T::None)
		texCropA = TexCrop::Back;
	else
		texCropA = TexCrop::None;

	if ((transitionTexCropB & T::TexCrop_B_Shuffle) != T::None)
		texCropB = TexCrop::Shuffle;
	else if ((m_transitionId & T::TexCrop_B_Both) == T::TexCrop_B_Both)
		texCropB = TexCrop::Both;
	else if ((m_transitionId & T::TexCrop_B_Start) != T::None)
		texCropB = TexCrop::Front;
	else if ((m_transitionId & T::TexCrop_B_End) != T::None)
		texCropB = TexCrop::Back;
	else
		texCropB = TexCrop::None;


	if (swapDirection)
	{
		// since front and back are currently equal to start and end, flip them if we are facing in opposite direction
		if (texCropA == TexCrop::Front)
			texCropA = TexCrop::Back;
		else if (texCropA == TexCrop::Back)
			texCropA = TexCrop::Front;
		if (texCropB == TexCrop::Back)
			texCropB = TexCrop::Front;
		else if (texCropB == TexCrop::Front)
			texCropB = TexCrop::Back;
	}



	sf::FloatRect texRectA(m_frameA.textureRect);
	sf::FloatRect texRectB(m_frameB.textureRect);
	float scaleSizeA{};
	float scaleSizeB{};
	switch (direction)
	{
	case Direction::Left:
	case Direction::Right:
		scaleSizeA = texRectA.size.x;
		scaleSizeB = texRectB.size.x;
		break;
	case Direction::Up:
	case Direction::Down:
		scaleSizeA = texRectA.size.y;
		scaleSizeB = texRectB.size.y;
		break;
	}
	


	float offsetPosA{};
	float offsetPosB{};
	switch (texCropA)
	{
	case TexCrop::None:
		break;
	case TexCrop::Back:
		offsetPosA = 0.f;
		break;
	case TexCrop::Front:
		offsetPosA = scaleSizeA * ratio;
		break;
	case TexCrop::Both:
		offsetPosA = scaleSizeA * ratio * 0.5f;
		break;
	case TexCrop::Shuffle:
		if (swapDirection)
			offsetPosA = scaleSizeA * ((ratio > 0.5f) ? inverseRatio : ratio);
		else
			offsetPosA = (ratio < 0.5f) ? 0.f : scaleSizeA * ((ratio * 2.f) - 1.f);
		break;
	}
	switch (texCropB)
	{
	case TexCrop::None:
		break;
	case TexCrop::Back:
		offsetPosB = 0.f;
		break;
	case TexCrop::Front:
		offsetPosB = scaleSizeB * inverseRatio;
		break;
	case TexCrop::Both:
		offsetPosB = scaleSizeB * inverseRatio * 0.5f;
		break;
	case TexCrop::Shuffle:
		if (swapDirection)
			offsetPosB = (ratio > 0.5f) ? 0.f : scaleSizeB * (1.f - (ratio * 2.f));
		else
			offsetPosB = scaleSizeB * ((ratio > 0.5f) ? inverseRatio : ratio);
		break;
	}

	scaleSizeA *= inverseRatio;
	scaleSizeB *= ratio;
	switch (direction)
	{
	case Direction::Left:
	case Direction::Right:
		texRectA.position.x = offsetPosA;
		texRectA.size.x = scaleSizeA;
		texRectB.position.x = offsetPosB;
		texRectB.size.x = scaleSizeB;
		break;
	case Direction::Down:
	case Direction::Up:
		texRectA.position.y = offsetPosA;
		texRectA.size.y = scaleSizeA;
		texRectB.position.y = offsetPosB;
		texRectB.size.y = scaleSizeB;
		break;
	}





	if (texCropA == TexCrop::None)
		texRectA = sf::FloatRect(m_frameA.textureRect);
	if (texCropB == TexCrop::None)
		texRectB = sf::FloatRect(m_frameB.textureRect);



	Quad quadA;
	Quad quadB;

	Quad* qa{ &quadA };
	Quad* qb{ &quadB };

	if (swapDirection)
	{
		std::swap(ratio, inverseRatio);
		std::swap(qa, qb);
	}

	switch (direction)
	{
	case Direction::Right:
	case Direction::Left:
		qa->topLeft.position = { m_size.x * ratio, 0.f };
		qa->bottomLeft.position = { m_size.x * ratio, m_size.y };
		qa->bottomRight.position = m_size;
		qa->topRight.position = { m_size.x, 0.f };

		qb->topLeft.position = { 0.f, 0.f };
		qb->bottomLeft.position = { 0.f, m_size.y };
		qb->bottomRight.position = { m_size.x * ratio, m_size.y };
		qb->topRight.position = { m_size.x * ratio, 0.f };
		break;
	case Direction::Down:
	case Direction::Up:
		qa->topLeft.position = { 0.f, m_size.y * ratio };
		qa->bottomLeft.position = { 0.f, m_size.y };
		qa->bottomRight.position = m_size;
		qa->topRight.position = { m_size.x, m_size.y * ratio };

		qb->topLeft.position = { 0.f, 0.f };
		qb->bottomLeft.position = { 0.f, m_size.y * ratio };
		qb->bottomRight.position = { m_size.x, m_size.y * ratio };
		qb->topRight.position = { m_size.x, 0.f };
		break;
	}



	priv_setQuadTextureCoordsFromRect(quadA, texRectA);
	priv_setQuadTextureCoordsFromRect(quadB, texRectB);

	priv_addQuad(0u, quadB);
	priv_addQuad(m_frameB.numberOfVertices, quadA);
}
void FrameTransition::priv_updateFromZoom() const
{
	const float ratio{ m_ratio };

	enum class ZoomType
	{
		Crop,
		Scale,
	};
	const ZoomType zoomTypeA{ ((m_transitionId & T::ZoomType_A) == T::ZoomType_A_Scale) ? ZoomType::Scale : ZoomType::Crop };
	const ZoomType zoomTypeB{ ((m_transitionId & T::ZoomType_B) == T::ZoomType_B_Scale) ? ZoomType::Scale : ZoomType::Crop };

	const float zoomScaleA{ m_parameter1 };
	const float zoomScaleB{ m_parameter2 };

	// we use divide by this reciprical (later) instead of the standard multiplication so that texture cropping is as if it was "un-scaled"
	// this allows cropping and scaling to be used together and they match movement
	const float zoomScaleARecip{ 1.f / ((zoomScaleA > 0.f) ? zoomScaleA : 0.001f) };
	const float zoomScaleBRecip{ 1.f / ((zoomScaleB > 0.f) ? zoomScaleB : 0.001f) };

	float zoomSizeScaleA{ zoomTypeA == ZoomType::Scale ? zoomScaleA : zoomScaleARecip };
	float zoomSizeScaleB{ zoomTypeB == ZoomType::Scale ? zoomScaleB : zoomScaleBRecip };

	enum class ZoomMovementType
	{
		None,
		In,
		Out,
		InOut,
		OutIn,
	};
	ZoomMovementType zoomMovementTypeA{};
	ZoomMovementType zoomMovementTypeB{};
	switch (m_transitionId & T::Zoom_A)
	{
	case T::Zoom_A_OutIn:
		zoomMovementTypeA = (zoomTypeA == ZoomType::Scale) ? ZoomMovementType::InOut : ZoomMovementType::OutIn;
		break;
	case T::Zoom_A_InOut:
		zoomMovementTypeA = (zoomTypeA == ZoomType::Scale) ? ZoomMovementType::OutIn : ZoomMovementType::InOut;
		break;
	case T::Zoom_A_Out:
		zoomMovementTypeA = (zoomTypeA == ZoomType::Scale) ? ZoomMovementType::In : ZoomMovementType::Out;
		break;
	case T::Zoom_A_In:
		zoomMovementTypeA = (zoomTypeA == ZoomType::Scale) ? ZoomMovementType::Out : ZoomMovementType::In;
		break;
	case T::Zoom_A_None:
	default:
		zoomMovementTypeA = ZoomMovementType::None;
	}
	switch (m_transitionId & T::Zoom_B)
	{
	case T::Zoom_B_OutIn:
		zoomMovementTypeB = (zoomTypeB == ZoomType::Scale) ? ZoomMovementType::InOut : ZoomMovementType::OutIn;
		break;
	case T::Zoom_B_InOut:
		zoomMovementTypeB = (zoomTypeB == ZoomType::Scale) ? ZoomMovementType::OutIn : ZoomMovementType::InOut;
		break;
	case T::Zoom_B_Out:
		zoomMovementTypeB = (zoomTypeB == ZoomType::Scale) ? ZoomMovementType::In : ZoomMovementType::Out;
		break;
	case T::Zoom_B_In:
		zoomMovementTypeB = (zoomTypeB == ZoomType::Scale) ? ZoomMovementType::Out : ZoomMovementType::In;
		break;
	case T::Zoom_B_None:
	default:
		zoomMovementTypeB = ZoomMovementType::None;
	}

	float multiplierA{};
	float multiplierB{};
	switch (zoomMovementTypeA)
	{
	case ZoomMovementType::OutIn:
		multiplierA = (ratio > 0.5f) ? linearInterpolation(1.f, zoomSizeScaleA, (ratio * 2.f) - 1.f) : linearInterpolation(zoomSizeScaleA, 1.f, ratio * 2.f);
		break;
	case ZoomMovementType::InOut:
		multiplierA = (ratio > 0.5f) ? linearInterpolation(zoomSizeScaleA, 1.f, (ratio * 2.f) - 1.f) : linearInterpolation(1.f, zoomSizeScaleA, ratio * 2.f);
		break;
	case ZoomMovementType::Out:
		multiplierA = linearInterpolation(zoomSizeScaleA, 1.f, ratio);
		break;
	case ZoomMovementType::In:
		multiplierA = linearInterpolation(1.f, zoomSizeScaleA, ratio);
		break;
	case ZoomMovementType::None:
	default:
		multiplierA = 1.f;
	}
	switch (zoomMovementTypeB)
	{
	case ZoomMovementType::OutIn:
		multiplierB = (ratio > 0.5f) ? linearInterpolation(1.f, zoomSizeScaleB, (ratio * 2.f) - 1.f) : linearInterpolation(zoomSizeScaleB, 1.f, ratio * 2.f);
		break;
	case ZoomMovementType::InOut:
		multiplierB = (ratio > 0.5f) ? linearInterpolation(zoomSizeScaleB, 1.f, (ratio * 2.f) - 1.f) : linearInterpolation(1.f, zoomSizeScaleB, ratio * 2.f);
		break;
	case ZoomMovementType::Out:
		multiplierB = linearInterpolation(zoomSizeScaleB, 1.f, ratio);
		break;
	case ZoomMovementType::In:
		multiplierB = linearInterpolation(1.f, zoomSizeScaleB, ratio);
		break;
	case ZoomMovementType::None:
	default:
		multiplierB = 1.f;
	}



	sf::FloatRect texRectA(m_frameA.textureRect);
	sf::FloatRect texRectB(m_frameB.textureRect);

	sf::Vector2f quadSizeA{ m_size };
	sf::Vector2f quadSizeB{ m_size };
	sf::Vector2f texSizeA{ texRectA.size };
	sf::Vector2f texSizeB{ texRectB.size };
	switch (zoomTypeA)
	{
	case ZoomType::Scale:
		quadSizeA *= multiplierA;
		break;
	case ZoomType::Crop:
		texSizeA /= multiplierA;
		break;
	}
	switch (zoomTypeB)
	{
	case ZoomType::Scale:
		quadSizeB *= multiplierB;
		break;
	case ZoomType::Crop:
		texSizeB /= multiplierB;
		break;
	}



	// top lefts calculated to place the resized rectangles in the centre of their originals
	sf::Vector2f texTopLeftA{ (texRectA.size - texSizeA) * 0.5f };
	sf::Vector2f texTopLeftB{ (texRectB.size - texSizeB) * 0.5f };
	sf::Vector2f posTopLeftA{ (m_size - quadSizeA) * 0.5f };
	sf::Vector2f posTopLeftB{ (m_size - quadSizeB) * 0.5f };

	// resized texture rectangles
	texRectA.position = texTopLeftA;
	texRectA.size = texSizeA;

	texRectB.position = texTopLeftB;
	texRectB.size = texSizeB;

	// resized quad rectangles
	sf::FloatRect posRectA{};
	sf::FloatRect posRectB{};

	posRectA.position = posTopLeftA;
	posRectA.size = quadSizeA;

	posRectB.position = posTopLeftB;
	posRectB.size = quadSizeB;



	Quad quadA{};
	Quad quadB{};

	priv_setQuadPositionsFromRect(quadA, posRectA);
	priv_setQuadPositionsFromRect(quadB, posRectB);

	priv_setQuadTextureCoordsFromRect(quadA, texRectA);
	priv_setQuadTextureCoordsFromRect(quadB, texRectB);

	priv_addQuad(0u, quadB);
	priv_addQuad(m_frameB.numberOfVertices, quadA);
}
void FrameTransition::priv_addQuad(const std::size_t startVertex, const sf::Vertex topLeft, const sf::Vertex bottomLeft, const sf::Vertex bottomRight, const sf::Vertex topRight) const
{
	m_vertices[startVertex + 0u] = topLeft;
	m_vertices[startVertex + 1u] = bottomLeft;
	m_vertices[startVertex + 2u] = bottomRight;
	m_vertices[startVertex + 3u] = topLeft;
	m_vertices[startVertex + 4u] = bottomRight;
	m_vertices[startVertex + 5u] = topRight;
}
void FrameTransition::priv_addQuad(const std::size_t startVertex, const Quad& quad) const
{
	priv_addQuad(startVertex, quad.topLeft, quad.bottomLeft, quad.bottomRight, quad.topRight);
}
void FrameTransition::priv_setQuadPositionsFromRect(Quad& quad, sf::FloatRect rect) const
{
	const sf::Vector2f topLeft(rect.position);
	const sf::Vector2f bottomRight{ sf::Vector2f(rect.size) + topLeft };
	quad.topLeft.position = topLeft;
	quad.bottomRight.position = bottomRight;
	quad.bottomLeft.position = { topLeft.x, bottomRight.y };
	quad.topRight.position = { bottomRight.x, topLeft.y };
}
void FrameTransition::priv_setQuadTextureCoordsFromRect(Quad& quad, sf::FloatRect rect) const
{
	const sf::Vector2f topLeft(rect.position);
	const sf::Vector2f bottomRight{ sf::Vector2f(rect.size) + topLeft };
	quad.topLeft.texCoords = topLeft;
	quad.bottomRight.texCoords = bottomRight;
	quad.bottomLeft.texCoords = { topLeft.x, bottomRight.y };
	quad.topRight.texCoords = { bottomRight.x, topLeft.y };
}
const FrameTransition::Frame& FrameTransition::priv_getFrame(const FrameId frameId) const
{
	return (frameId == FrameId::A) ? m_frameA : m_frameB;
}
FrameTransition::Frame& FrameTransition::priv_getFrame(const FrameId frameId)
{
	return (frameId == FrameId::A) ? m_frameA : m_frameB;
}

} // namespace selbward
