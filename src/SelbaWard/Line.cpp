//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Line
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

#include "Line.hpp"

#include <algorithm>
#include <cmath>

namespace
{

const float thicknessEpsilon{ 0.001f };
const float pi{ 3.141592653f };

} // namespace



namespace selbaward
{

Line::Line()
	: m_vertices(sf::PrimitiveType::Lines, 2u)
	, m_quad(sf::PrimitiveType::TriangleStrip, 4u)
	, m_thickness{ 0.f }
	, m_texture{ nullptr }
	, m_textureRect()
{
}

Line::Line(const sf::Vector2f startPosition, const sf::Vector2f endPosition)
	: Line()
{
	setPoints(startPosition, endPosition);
}

void Line::setPoint(const unsigned int index, const sf::Vector2f position)
{
	if (index > 1)
		return;
	m_vertices[index].position = position;

	if (isThick())
		updateQuad();
}

void Line::setPoints(const sf::Vector2f startPosition, const sf::Vector2f endPosition)
{
	m_vertices[0].position = startPosition;
	m_vertices[1].position = endPosition;

	if (isThick())
		updateQuad();
}

sf::Vector2f Line::getPoint(const unsigned int index) const
{
	if (index > 1)
		return{ 0.f, 0.f };
	return m_vertices[index].position;
}

sf::FloatRect Line::getLocalBounds() const
{
	sf::FloatRect box;
	if (isThick())
	{
		float minX, maxX, minY, maxY;
		minX = maxX = m_quad[0].position.x;
		minY = maxY = m_quad[0].position.y;
		for (unsigned int v{ 1u }; v < 4u; ++v)
		{
			minX = std::min(minX, m_quad[v].position.x);
			maxX = std::max(maxX, m_quad[v].position.x);
			minY = std::min(minY, m_quad[v].position.y);
			maxY = std::max(maxY, m_quad[v].position.y);
		}
		box.position.x = minX;
		box.position.y = minY;
		box.size.x = maxX - minX;
		box.size.y = maxY - minY;
	}
	else
	{
		box.position.x = std::min(m_vertices[0].position.x, m_vertices[1].position.x);
		box.position.y = std::min(m_vertices[0].position.y, m_vertices[1].position.y);
		box.size.x = std::max(m_vertices[0].position.x, m_vertices[1].position.x) - box.position.x;
		box.size.y = std::max(m_vertices[0].position.y, m_vertices[1].position.y) - box.position.y;
	}
	return box;
}

sf::FloatRect Line::getGlobalBounds() const
{
	sf::FloatRect box;
	if (isThick())
	{
		const sf::Transform transform{ getTransform() };
		const sf::Vector2f transformedPosition0{ transform.transformPoint(m_quad[0].position) };
		float minX, maxX, minY, maxY;
		minX = maxX = transformedPosition0.x;
		minY = maxY = transformedPosition0.y;
		for (unsigned int v{ 1u }; v < 4u; ++v)
		{
			const sf::Vector2f transformedPosition{ transform.transformPoint(m_quad[v].position) };
			minX = std::min(minX, transformedPosition.x);
			maxX = std::max(maxX, transformedPosition.x);
			minY = std::min(minY, transformedPosition.y);
			maxY = std::max(maxY, transformedPosition.y);
		}
		box.position.x = minX;
		box.position.y = minY;
		box.size.x = maxX - minX;
		box.size.y = maxY - minY;
	}
	else
	{
		const sf::Vector2f transformedStartPosition{ getTransform().transformPoint(m_vertices[0].position) };
		const sf::Vector2f transformedEndPosition{ getTransform().transformPoint(m_vertices[1].position) };
		box.position.x = std::min(transformedStartPosition.x, transformedEndPosition.x);
		box.position.y = std::min(transformedStartPosition.y, transformedEndPosition.y);
		box.size.x = std::max(transformedStartPosition.x, transformedEndPosition.x) - box.position.x;
		box.size.y = std::max(transformedStartPosition.y, transformedEndPosition.y) - box.position.y;
	}
	return box;
}

Line::PointIndex Line::getStartIndex() const
{
	return Start;
}

Line::PointIndex Line::getEndIndex() const
{
	return End;
}

sf::Color Line::getColor() const
{
	return m_vertices[0].color;
}

void Line::setColor(const sf::Color& color)
{
	m_vertices[0u].color = color;
	m_vertices[1u].color = color;
	m_quad[0u].color = color;
	m_quad[1u].color = color;
	m_quad[2u].color = color;
	m_quad[3u].color = color;
}

void Line::setTexture(const sf::Texture& texture)
{
	m_texture = &texture;
	m_textureRect = { { 0.f, 0.f }, sf::Vector2f(texture.getSize()) };
	updateQuad();
}

void Line::setTexture()
{
	m_texture = nullptr;
}

const sf::Texture& Line::getTexture() const
{
	return *m_texture;
}

void Line::setTextureRect(const sf::FloatRect& textureRect)
{
	m_textureRect = textureRect;
	updateQuad();
}

sf::FloatRect Line::getTextureRect() const
{
	return m_textureRect;
}



// PRIVATE

void Line::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = nullptr;
	if (isThick())
	{
		if (m_texture != nullptr)
			states.texture = m_texture;
		target.draw(m_quad, states);
	}
	else
		target.draw(m_vertices, states);
}

bool Line::isThick() const
{
	return (m_thickness >= thicknessEpsilon || m_thickness <= -thicknessEpsilon);
}

void Line::updateQuad()
{
	const sf::Vector2f lineVector{ m_vertices[0u].position - m_vertices[1u].position };
	const float lineLength{ std::sqrt(lineVector.x * lineVector.x + lineVector.y * lineVector.y) };
	const sf::Vector2f unitVector{ lineVector / lineLength };
	const sf::Vector2f unitNormalVector{ unitVector.y, -unitVector.x };
	const sf::Vector2f normalVector{ unitNormalVector * m_thickness / 2.f };

	m_quad[0u].position = m_vertices[0u].position + normalVector;
	m_quad[1u].position = m_vertices[0u].position - normalVector;
	m_quad[2u].position = m_vertices[1u].position + normalVector;
	m_quad[3u].position = m_vertices[1u].position - normalVector;

	m_quad[0u].texCoords = { m_textureRect.position.x, m_textureRect.position.y };
	m_quad[1u].texCoords = { m_textureRect.position.x, m_textureRect.position.y + m_textureRect.size.y };
	m_quad[2u].texCoords = { m_textureRect.position.x + m_textureRect.size.x, m_textureRect.position.x };
	m_quad[3u].texCoords = { m_textureRect.position.x + m_textureRect.size.x, m_textureRect.position.x + m_textureRect.size.y };
}

} // selbaward
