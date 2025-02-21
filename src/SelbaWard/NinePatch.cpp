//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// NinePatch
//
// Copyright(c) 2015-2025 M.J.Silk
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

#include "NinePatch.hpp"

#include <SFML/Graphics/Image.hpp>

namespace
{

const sf::Vector2f trimAmount{ 1.f, 1.f };

void extractScalePositionsAndContentAreaFromTexture(const sf::Texture* const pTexture, const sf::IntRect textureRectangle, sf::Vector2f& topLeft, sf::Vector2f& bottomRight, sf::Vector2f& contentTopLeft, sf::Vector2f& contentBottomRight)
{
	sf::Image image{ pTexture->copyToImage() };

	// scale positions
	topLeft = { 0.f, 0.f };
	bottomRight = { textureRectangle.size.x - 2.f, textureRectangle.size.y - 2.f };
	bool foundStart{ false }, foundEnd{ false };
	for (unsigned int x{ 1u }; x < static_cast<unsigned int>(textureRectangle.size.x); ++x)
	{
		if (!foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + x, textureRectangle.position.y)) == sf::Color::Black)
			{
				foundStart = true;
				topLeft.x = x - 1.f;
			}
			else
				continue;
		}
		if (foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + x, textureRectangle.position.y)) == sf::Color::Black)
				bottomRight.x = x - 1.f;
			else
				break;
		}
	}
	foundStart = false;
	foundEnd = false;
	for (unsigned int y{ 1u }; y < static_cast<unsigned int>(textureRectangle.size.y); ++y)
	{
		if (!foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x, textureRectangle.position.y + y)) == sf::Color::Black)
			{
				foundStart = true;
				topLeft.y = y - 1.f;
			}
			else
				continue;
		}
		if (foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x, textureRectangle.position.y + y)) == sf::Color::Black)
				bottomRight.y = y - 1.f;
			else
				break;
		}
	}

	// content area
	contentTopLeft = { 0.f, 0.f };
	contentBottomRight = { textureRectangle.size.x - 2.f, textureRectangle.size.y - 2.f };
	foundStart = false;
	foundEnd = false;
	const sf::Vector2u textureBottomRightPixel(textureRectangle.size.x - 1u, textureRectangle.size.y - 1u);
	for (unsigned int x{ 1u }; x < static_cast<unsigned int>(textureRectangle.size.x); ++x)
	{
		if (!foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + x, textureRectangle.position.y + textureBottomRightPixel.y)) == sf::Color::Black)
			{
				foundStart = true;
				contentTopLeft.x = x - 1.f;
			}
			else
				continue;
		}
		if (foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + x, textureRectangle.position.y + textureBottomRightPixel.y)) == sf::Color::Black)
				contentBottomRight.x = x - 1.f;
			else
				break;
		}
	}
	foundStart = false;
	foundEnd = false;
	for (unsigned int y{ 1u }; y < static_cast<unsigned int>(textureRectangle.size.y); ++y)
	{
		if (!foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + textureBottomRightPixel.x, textureRectangle.position.y + y)) == sf::Color::Black)
			{
				foundStart = true;
				contentTopLeft.y = y - 1.f;
			}
			else
				continue;
		}
		if (foundStart)
		{
			if (image.getPixel(sf::Vector2u(textureRectangle.position.x + textureBottomRightPixel.x, textureRectangle.position.y + y)) == sf::Color::Black)
				contentBottomRight.y = y - 1.f;
			else
				break;
		}
	}
}

} // namespace

namespace selbaward
{

NinePatch::NinePatch()
	: m_vertices(22u, sf::Vertex{})
	, m_texture{ nullptr }
	, m_trimmedSize{ 0.f, 0.f }
	, m_size{ 0.f, 0.f }
	, m_scaleTopLeft{ 0.f, 0.f }
	, m_scaleBottomRight{ 0.f, 0.f }
	, m_contentTopLeft{ 0.f, 0.f }
	, m_contentBottomRight{ 0.f, 0.f }
	, m_textureRectangle{ { 0, 0 }, { 3, 3 } }
{
}

void NinePatch::setTexture(const sf::Texture& texture, const bool resetSize, const bool resetRect)
{
	m_texture = &texture;
	if (resetRect)
		m_textureRectangle = { { 0, 0 }, sf::Vector2i(m_texture->getSize()) };
	m_trimmedSize = sf::Vector2f{ static_cast<float>(m_textureRectangle.size.x), static_cast<float>(m_textureRectangle.size.y) } - trimAmount * 2.f;
	if (resetSize)
		m_size = m_trimmedSize;
	extractScalePositionsAndContentAreaFromTexture(m_texture, m_textureRectangle, m_scaleTopLeft, m_scaleBottomRight, m_contentTopLeft, m_contentBottomRight);
	priv_updateVertices();
}

void NinePatch::setSize(sf::Vector2f size)
{
	m_size = size;
	const sf::Vector2f minimumSize{ m_scaleTopLeft + (m_trimmedSize - m_scaleBottomRight) };
	if (m_size.x < minimumSize.x)
		m_size.x = minimumSize.x;
	if (m_size.y < minimumSize.y)
		m_size.y = minimumSize.y;
	priv_updateVerticesPositions();
}

void NinePatch::resetSize()
{
	setSize(m_trimmedSize);
}

void NinePatch::setTextureRect(const sf::IntRect textureRectangle, const bool resetSize)
{
	m_textureRectangle = textureRectangle;
	m_trimmedSize = sf::Vector2f{ static_cast<float>(m_textureRectangle.size.x), static_cast<float>(m_textureRectangle.size.y) } - trimAmount * 2.f;
	if (resetSize)
		m_size = m_trimmedSize;
	if (m_texture != nullptr)
	{
		extractScalePositionsAndContentAreaFromTexture(m_texture, m_textureRectangle, m_scaleTopLeft, m_scaleBottomRight, m_contentTopLeft, m_contentBottomRight);
		priv_updateVertices();
	}
}

void NinePatch::setColor(const sf::Color& color)
{
	for (auto& vertex : m_vertices)
		vertex.color = color;
}

sf::Color NinePatch::getColor() const
{
	return m_vertices[0].color;
}

sf::FloatRect NinePatch::getLocalBounds() const
{
	return{ { 0.f, 0.f }, m_size };
}

sf::FloatRect NinePatch::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::FloatRect NinePatch::getLocalContentArea() const
{
	const sf::Vector2f topLeft{ priv_getResultingPositionOfTextureCoord(m_contentTopLeft) };
	return{ topLeft, priv_getResultingPositionOfTextureCoord(m_contentBottomRight) - topLeft + sf::Vector2f{ 1.f, 1.f } };
}

sf::FloatRect NinePatch::getGlobalContentArea() const
{
	return getTransform().transformRect(getLocalContentArea());
}

bool NinePatch::isPointInsideTransformedContentArea(const sf::Vector2f point) const
{
	return getLocalContentArea().contains(getInverseTransform().transformPoint(point));
}



// PRIVATE

void NinePatch::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = m_texture;
	states.transform *= getTransform();
	target.draw(m_vertices.data(), 22u, sf::PrimitiveType::TriangleStrip, states);
}

void NinePatch::priv_updateVertices()
{
	priv_updateVerticesPositions();
	priv_updateVerticesTexCoords();
}

void NinePatch::priv_updateVerticesPositions()
{
	const sf::Vector2f newBottomRightScaled{ m_size - (m_trimmedSize - m_scaleBottomRight) };

	const float x0{ 0.f };
	const float x1{ m_scaleTopLeft.x };
	const float x2{ newBottomRightScaled.x };
	const float x3{ m_size.x };
	const float y0{ 0.f };
	const float y1{ m_scaleTopLeft.y };
	const float y2{ newBottomRightScaled.y };
	const float y3{ m_size.y };

	m_vertices[0u].position = { x0, y0 };
	m_vertices[1u].position = { x0, y1 };
	m_vertices[2u].position = { x1, y0 };
	m_vertices[3u].position = { x1, y1 };
	m_vertices[4u].position = { x2, y0 };
	m_vertices[5u].position = { x2, y1 };
	m_vertices[6u].position = { x3, y0 };
	m_vertices[7u].position = { x3, y1 };
	m_vertices[8u].position = { x3, y2 };
	m_vertices[9u].position = { x2, y1 };
	m_vertices[10u].position = { x2, y2 };
	m_vertices[11u].position = { x1, y1 };
	m_vertices[12u].position = { x1, y2 };
	m_vertices[13u].position = { x0, y1 };
	m_vertices[14u].position = { x0, y2 };
	m_vertices[15u].position = { x0, y3 };
	m_vertices[16u].position = { x1, y2 };
	m_vertices[17u].position = { x1, y3 };
	m_vertices[18u].position = { x2, y2 };
	m_vertices[19u].position = { x2, y3 };
	m_vertices[20u].position = { x3, y2 };
	m_vertices[21u].position = { x3, y3 };
}

void NinePatch::priv_updateVerticesTexCoords()
{
	const sf::Vector2f textureBottomRight{ m_trimmedSize };

	const float x0{ 0.f };
	const float x1{ m_scaleTopLeft.x };
	const float x2{ m_scaleBottomRight.x };
	const float x3{ textureBottomRight.x };
	const float y0{ 0.f };
	const float y1{ m_scaleTopLeft.y };
	const float y2{ m_scaleBottomRight.y };
	const float y3{ textureBottomRight.y };

	m_vertices[0u].texCoords = { x0, y0 };
	m_vertices[1u].texCoords = { x0, y1 };
	m_vertices[2u].texCoords = { x1, y0 };
	m_vertices[3u].texCoords = { x1, y1 };
	m_vertices[4u].texCoords = { x2, y0 };
	m_vertices[5u].texCoords = { x2, y1 };
	m_vertices[6u].texCoords = { x3, y0 };
	m_vertices[7u].texCoords = { x3, y1 };
	m_vertices[8u].texCoords = { x3, y2 };
	m_vertices[9u].texCoords = { x2, y1 };
	m_vertices[10u].texCoords = { x2, y2 };
	m_vertices[11u].texCoords = { x1, y1 };
	m_vertices[12u].texCoords = { x1, y2 };
	m_vertices[13u].texCoords = { x0, y1 };
	m_vertices[14u].texCoords = { x0, y2 };
	m_vertices[15u].texCoords = { x0, y3 };
	m_vertices[16u].texCoords = { x1, y2 };
	m_vertices[17u].texCoords = { x1, y3 };
	m_vertices[18u].texCoords = { x2, y2 };
	m_vertices[19u].texCoords = { x2, y3 };
	m_vertices[20u].texCoords = { x3, y2 };
	m_vertices[21u].texCoords = { x3, y3 };

	// offset trim and texture rectangle
	const sf::Vector2f textureRectangleOffset{ static_cast<float>(m_textureRectangle.position.x), static_cast<float>(m_textureRectangle.position.y) };
	for (auto& vertex : m_vertices)
		vertex.texCoords += textureRectangleOffset + trimAmount;
}

sf::Vector2f NinePatch::priv_getResultingPositionOfTextureCoord(sf::Vector2f textureCoord) const
{
	sf::Vector2f result;

	const sf::Vector2f newBottomRightScaled{ m_size - (m_trimmedSize - m_scaleBottomRight) };
	const sf::Vector2f scaleSize{ m_scaleBottomRight - m_scaleTopLeft };
	const sf::Vector2f newScaleSize{ newBottomRightScaled - m_scaleTopLeft };

	if (textureCoord.x <= m_scaleTopLeft.x)
		result.x = textureCoord.x;
	else if (textureCoord.x >= m_scaleBottomRight.x)
		result.x = newBottomRightScaled.x + (textureCoord.x - m_scaleBottomRight.x); // brackets are to help clarity
	else
		result.x = ((textureCoord.x - m_scaleTopLeft.x) / scaleSize.x) * newScaleSize.x + m_scaleTopLeft.x;
	if (textureCoord.y <= m_scaleTopLeft.y)
		result.y = textureCoord.y;
	else if (textureCoord.y >= m_scaleBottomRight.y)
		result.y = newBottomRightScaled.y + (textureCoord.y - m_scaleBottomRight.y); // brackets are to help clarity
	else
		result.y = ((textureCoord.y - m_scaleTopLeft.y) / (scaleSize.y)) * newScaleSize.y + m_scaleTopLeft.y;

	return result;
}

} // namespace selbaward
