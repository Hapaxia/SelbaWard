//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Line
//
// Copyright(c) 2015-2016 M.J.Silk
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

namespace selbaward
{

Line::Line() :
m_vertices(sf::Lines, 2),
m_rectangle(),
m_thickness(0.f),
m_color(sf::Color::White)
{
}

Line::Line(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition) :
Line()
{
	setPoints(startPosition, endPosition);
}

/*
template <typename T>
Line::Line(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, T thickness, const sf::Color& color) :
Line(),
m_color(color)
{
	setPoints(startPosition, endPosition);
	setThickness(thickness);
}
*/

void Line::setPoint(unsigned int index, const sf::Vector2f& position)
{
	if (index > 1)
		return;
	m_vertices[index].position = position;

	if (isThick())
		updateRectangle();
}

void Line::setPoints(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition)
{
	m_vertices[0].position = startPosition;
	m_vertices[1].position = endPosition;

	if (isThick())
		updateRectangle();
}

sf::Vector2f Line::getPoint(unsigned int index) const
{
	if (index > 1)
		return{ 0.f, 0.f };
	return m_vertices[index].position;
}

sf::FloatRect Line::getLocalBounds() const
{
	sf::FloatRect box;
	box.left = std::min(m_vertices[0].position.x, m_vertices[1].position.x);
	box.top = std::min(m_vertices[0].position.y, m_vertices[1].position.y);
	box.width = std::max(m_vertices[0].position.x, m_vertices[1].position.x) - box.left;
	box.height = std::max(m_vertices[0].position.y, m_vertices[1].position.y) - box.top;
	return box;
}

sf::FloatRect Line::getGlobalBounds() const
{
	const sf::Vector2f transformedStartPosition{ getTransform().transformPoint(m_vertices[0].position) };
	const sf::Vector2f transformedEndPosition{ getTransform().transformPoint(m_vertices[1].position) };

	sf::FloatRect box;
	box.left = std::min(transformedStartPosition.x, transformedEndPosition.x);
	box.top = std::min(transformedStartPosition.y, transformedEndPosition.y);
	box.width = std::max(transformedStartPosition.x, transformedEndPosition.x) - box.left;
	box.height = std::max(transformedStartPosition.y, transformedEndPosition.y) - box.top;
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

void Line::setColor(const sf::Color& color)
{
	m_color = color;
	m_vertices[0].color = color;
	m_vertices[1].color = color;
	m_rectangle.setFillColor(m_color);
}



// PRIVATE

void Line::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = NULL;
	if (isThick())
		target.draw(m_rectangle, states);
	else
		target.draw(m_vertices, states);
}

inline bool Line::isThick() const
{
	return (m_thickness >= 0.001f);
}

void Line::updateRectangle()
{
	const float pi{ 3.141592653f };
	const sf::Vector2f lineVector{ m_vertices[0].position - m_vertices[1].position };
	const float lineLength{ sqrtf(lineVector.x * lineVector.x + lineVector.y * lineVector.y) };
	m_rectangle.setSize({ m_thickness, lineLength });
	m_rectangle.setOrigin({ m_thickness / 2.f, 0.f });
	const float angle{ 90.f + atan2f(lineVector.y, lineVector.x) * 180.f / pi };
	m_rectangle.setRotation(angle);
	m_rectangle.setPosition(m_vertices[0].position);
}

} // selbaward
