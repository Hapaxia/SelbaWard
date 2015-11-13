//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Line
//
// Copyright(c) 2015 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty.In no event will the authors be held liable for any damages
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

// currently uses sf::RectangleShape for a line with thickness. seems simpler to implement but has less control over the vertices than a dedicated vertex array

#ifndef SELBAWARD_LINE_HPP
#define SELBAWARD_LINE_HPP

#include "Common.hpp"

#include <math.h>

#include <SFML/Graphics/RectangleShape.hpp>

namespace selbaward
{

class Line : public sf::Drawable, public sf::Transformable
{
public:
	enum PointIndex
	{
		Start = 0,
		End = 1
	};

	Line();
	Line(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition);
	template <typename T>
	Line(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, T thickness, const sf::Color& color = sf::Color::White);
	void setPoint(unsigned int index, const sf::Vector2f& position);
	void setPoints(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition);
	sf::Vector2f getPoint(unsigned int index) const;
	sf::FloatRect getLocalBounds() const; // currently get bounds of only the points, not the corners of a line with thickness (basically, thickness is ignored)
	sf::FloatRect getGlobalBounds() const; // currently get bounds of only the points, not the corners of a line with thickness (basically, thickness is ignored)
	PointIndex getStartIndex() const;
	PointIndex getEndIndex() const;
	template <typename T>
	void setThickness(T thickness) { m_thickness = static_cast<float>(thickness); if (isThick()) updateRectangle(); }
	void setColor(const sf::Color& color);

private:
	sf::VertexArray m_vertices;
	float m_thickness; // 0 to draw as line, 1+ to be drawn as a rectangle (using a quad) (currently using a rectangle shape instead)
	sf::Color m_color;
	sf::RectangleShape m_rectangle;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	bool isThick() const;
	void updateRectangle();
};

template <typename T>
Line::Line(const sf::Vector2f& startPosition, const sf::Vector2f& endPosition, T thickness, const sf::Color& color) :
Line()
{
	setColor(color);
	setPoints(startPosition, endPosition);
	setThickness(thickness);
}

} // selbaward
#endif // SELBAWARD_LINE_HPP