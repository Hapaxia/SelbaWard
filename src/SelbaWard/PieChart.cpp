//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Pie Chart
//
// Copyright(c) 2015 M.J.Silk
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

#include "PieChart.hpp"

#include <iostream>

namespace
{

inline float radiansFromDegrees(float degrees)
{
	return degrees * 0.0174532925f; // pi/180
}

} // namespace

namespace selbaward
{

PieChart::Slice::Slice()
	: size(0.1f)
	, scale(1.f)
	, explode(0.f)
	, color(sf::Color::White)
{
}

PieChart::PieChart()
	: m_primitive(sf::PrimitiveType::Triangles)
	, m_vertices()
	, m_size({ 32.f, 32.f })
{
}

void PieChart::update()
{
	const sf::Vector2f halfSize{ m_size / 2.f };
	unsigned int totalNumberOfTrianglesRequired{ 0u };
	std::vector<unsigned int> numberOfTrianglesRequiredPerSlice;
	for (auto& slice : slices)
	{
		numberOfTrianglesRequiredPerSlice.emplace_back(static_cast<unsigned int>(floor(1.f + slice.size * 50.f)));
		totalNumberOfTrianglesRequired += numberOfTrianglesRequiredPerSlice.back();
	}
	m_vertices.resize(totalNumberOfTrianglesRequired * 3);
	unsigned int currentVertex{ 0u };
	float currentAngle{ 0.f };
	for (unsigned int slice{ 0u }; slice < slices.size(); ++slice)
	{
		const float startAngle{ currentAngle };
		const float halfAngleDifference{ 180.f * slices[slice].size };
		const sf::Vector2f offset{ sf::Vector2f(sin(radiansFromDegrees(startAngle + halfAngleDifference)), -cos(radiansFromDegrees(startAngle + halfAngleDifference))) * slices[slice].explode };
		for (unsigned int triangle{ 0u }; triangle < numberOfTrianglesRequiredPerSlice[slice]; ++triangle)
		{
			m_vertices[currentVertex + 0].position = halfSize + sf::Vector2f(offset.x * halfSize.x, offset.y * halfSize.y);
			m_vertices[currentVertex + 1].position = halfSize + sf::Vector2f((offset.x + sin(radiansFromDegrees(currentAngle))) * halfSize.x, (offset.y - cos(radiansFromDegrees(currentAngle))) * halfSize.y) * slices[slice].scale;
			currentAngle += halfAngleDifference * 2.f / numberOfTrianglesRequiredPerSlice[slice];
			m_vertices[currentVertex + 2].position = halfSize + sf::Vector2f((offset.x + sin(radiansFromDegrees(currentAngle))) * halfSize.x, (offset.y - cos(radiansFromDegrees(currentAngle))) * halfSize.y) * slices[slice].scale;
			m_vertices[currentVertex + 0].color = slices[slice].color;
			m_vertices[currentVertex + 1].color = slices[slice].color;
			m_vertices[currentVertex + 2].color = slices[slice].color;
			currentVertex += 3;
		}
	}
}



// PRIVATE

void PieChart::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform = getTransform();

	if (m_vertices.size() > 0)
		target.draw(&m_vertices.front(), m_vertices.size(), m_primitive, states);
}

} // namespace selbaward
