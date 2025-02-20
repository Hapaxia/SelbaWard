//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Starfield 3D
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

#include "Starfield3d.hpp"

#include <random>
#include <functional>
#include <cmath>

namespace
{

std::mt19937 randomGenerator;
std::uniform_int_distribution<unsigned short int> randomDistributionAlpha(1u, 255u);
std::function <unsigned short int()> randomAlpha;

inline void randomSeed()
{
	std::random_device rd;
	randomGenerator.seed(rd());
	randomAlpha = std::bind(randomDistributionAlpha, randomGenerator);
}

inline float randomValue(const float low, const float high)
{
	return std::uniform_real_distribution<float>{low, high}(randomGenerator);
}

template <class T>
inline T linearTween(const T start, const T end, const float alpha)
{
	return static_cast<T>((start * (1.f - alpha)) + (end * alpha));
}

inline sf::Color linearTween(const sf::Color& start, const sf::Color& end, const float alpha)
{
	return{ linearTween(start.r, end.r, alpha), linearTween(start.g, end.g, alpha), linearTween(start.b, end.b, alpha), linearTween(start.a, end.a, alpha) };
}

} // namespace

namespace selbaward
{

Starfield3d::Starfield3d(const sf::Vector2f size, const std::size_t numberOfStars, const float maxDepth, const sf::Color& frontColor, const sf::Color& backColor, const float frontScale, const float backScale)
	: m_depthCalibration{ 0.001f }
	, m_depthSpeedCalibration{ 0.93f }
	, m_isUpdateRequired{ true }
	, m_size{ size }
	, m_numberOfStars{ numberOfStars }
	, m_positions(numberOfStars)
	, m_starTemplate(6u)
	, m_primitiveType{ sf::PrimitiveType::Triangles }
	, m_vertices(numberOfStars)
	, m_deepestSliceBounds()
	, m_maxDepth{ maxDepth }
	, m_frontColor{ frontColor }
	, m_backColor{ backColor }
	, m_frontScale{ frontScale }
	, m_backScale{ backScale }
{
	randomSeed();
	regenerate();

	m_starTemplate[0u].position = { 0.f, 1.f };
	m_starTemplate[1u].position = { 1.f, -0.5f };
	m_starTemplate[2u].position = { -1.f, -0.5f };
	m_starTemplate[3u].position = { 0.f, -1.f };
	m_starTemplate[4u].position = { -1.f, 0.5f };
	m_starTemplate[5u].position = { 1.f, 0.5f };

	// color
	for (auto& v : m_starTemplate)
		v.color = sf::Color::White;
}

void Starfield3d::move(const sf::Vector3f movement)
{
	m_isUpdateRequired = true;
	for (auto& position : m_positions)
	{
		// move
		position -= movement * std::pow(m_maxDepth * m_depthCalibration, m_depthSpeedCalibration);

		// wrap depth
		if (position.z < 0.f)
			position = priv_generateRandomStarPosition(EdgeLock::Back);
		else if (position.z > m_maxDepth)
			position = priv_generateRandomStarPosition(EdgeLock::Front);

		// wrap 2D (xy slice)
		if (position.x < m_deepestSliceBounds.position.x)
			position = priv_generateRandomStarPosition(EdgeLock::Right);
		else if (position.x > (m_deepestSliceBounds.position.x + m_deepestSliceBounds.size.x))
			position = priv_generateRandomStarPosition(EdgeLock::Left);
		if (position.y < m_deepestSliceBounds.position.y)
			position = priv_generateRandomStarPosition(EdgeLock::Bottom);
		else if (position.y > (m_deepestSliceBounds.position.y + m_deepestSliceBounds.size.y))
			position = priv_generateRandomStarPosition(EdgeLock::Top);
		
	}
}

void Starfield3d::pan(const sf::Vector2f panAmount)
{
	m_isUpdateRequired = true;
	const sf::Vector3f movement{ panAmount.x, panAmount.y, 0.f };
	for (auto& position : m_positions)
	{
		// move
		position -= movement * (((1.f + position.z) * m_depthCalibration - 1.f));

		// wrap 2D (xy slice)
		if (position.x < m_deepestSliceBounds.position.x)
			position = priv_generateRandomStarPosition(EdgeLock::Right);
		else if (position.x > (m_deepestSliceBounds.position.x + m_deepestSliceBounds.size.x))
			position = priv_generateRandomStarPosition(EdgeLock::Left);
		if (position.y < m_deepestSliceBounds.position.y)
			position = priv_generateRandomStarPosition(EdgeLock::Bottom);
		else if (position.y > (m_deepestSliceBounds.position.y + m_deepestSliceBounds.size.y))
			position = priv_generateRandomStarPosition(EdgeLock::Top);

	}
}

void Starfield3d::regenerate()
{
	m_deepestSliceBounds = priv_calculateFrustumSliceBounds(m_maxDepth);

	m_positions.resize(m_numberOfStars);
	for (auto& position : m_positions)
		position = priv_generateRandomStarPosition();
}

void Starfield3d::regenerate(const sf::Vector2f size)
{
	m_size = size;
	regenerate();
}

void Starfield3d::regenerate(const sf::Vector2f size, const std::size_t numberOfStars)
{
	m_numberOfStars = numberOfStars;
	regenerate(size);
}

void Starfield3d::regenerate(const std::size_t numberOfStars)
{
	regenerate(m_size, numberOfStars);
}

void Starfield3d::setMaxDepth(const float maxDepth)
{
	m_isUpdateRequired = true;
	m_maxDepth = maxDepth;
}

void Starfield3d::setFrontColor(const sf::Color& color)
{
	m_isUpdateRequired = true;
	m_frontColor = color;
}

void Starfield3d::setBackColor(const sf::Color& color)
{
	m_isUpdateRequired = true;
	m_backColor = color;
}

void Starfield3d::setFrontScale(const float frontScale)
{
	m_isUpdateRequired = true;
	m_frontScale = frontScale;
}

void Starfield3d::setBackScale(const float backScale)
{
	m_isUpdateRequired = true;
	m_backScale = backScale;
}

void Starfield3d::setStarTemplate(const std::vector<sf::Vertex>& vertices)
{
	m_isUpdateRequired = true;
	m_starTemplate = vertices;
}

void Starfield3d::setStarTemplate(const std::vector<sf::Vector2f>& vertices)
{
	m_isUpdateRequired = true;
	m_starTemplate.resize(vertices.size());
	for (std::size_t i{ 0u }; i < m_starTemplate.size(); ++i)
	{
		m_starTemplate[i].position = vertices[i];
		m_starTemplate[i].color = sf::Color::White;
	}
}

void Starfield3d::setStarTemplate(const sf::VertexArray& vertexArray)
{
	m_isUpdateRequired = true;
	m_starTemplate.resize(vertexArray.getVertexCount());
	for (std::size_t i{ 0u }; i < m_starTemplate.size(); ++i)
		m_starTemplate[i] = vertexArray[i];
}



// PRIVATE

void Starfield3d::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_isUpdateRequired)
		priv_updateVertices();

	states.transform *= getTransform();
	const std::size_t size{ m_vertices.size() };
	if (size > 0u)
		target.draw(m_vertices.data(), size, m_primitiveType, states);
}

void Starfield3d::priv_updateVertices() const
{
	// positions' indices sorted by depth
	m_positionIndices.resize(m_positions.size());
	for (std::size_t i{ 0u }; i < m_positionIndices.size(); ++i)
		m_positionIndices[i] = i;
	std::sort(m_positionIndices.begin(), m_positionIndices.end(), [&](std::size_t a, std::size_t b) { return m_positions[a].z > m_positions[b].z; });

	// vertices
	const std::size_t numberOfVerticesPerStar{ m_starTemplate.size() };
	m_vertices.resize(m_numberOfStars * numberOfVerticesPerStar);
	for (std::size_t star{ 0u }; star < m_numberOfStars; ++star)
	{
		const std::size_t starIndex{ m_positionIndices[star] };
		const float depthRatio{ m_positions[starIndex].z / m_maxDepth };
		const float depthInverseRatio{ 1.f - depthRatio };
		const sf::Color color{ linearTween(m_frontColor, m_backColor, depthRatio) };
		const float depthScale{ linearTween(m_frontScale, m_backScale, depthRatio) };
		sf::Vector2f starPosition{ priv_projectPoint(m_positions[starIndex]) };
		for (std::size_t vertex{ 0u }; vertex < numberOfVerticesPerStar; ++vertex)
		{
			m_vertices[star * numberOfVerticesPerStar + vertex].position = starPosition + (sf::Vector2f{ m_starTemplate[vertex].position.x, m_starTemplate[vertex].position.y } * depthScale);
			m_vertices[star * numberOfVerticesPerStar + vertex].color = m_starTemplate[vertex].color * color;
		}
	}
}

sf::Vector2f Starfield3d::priv_projectPoint(const sf::Vector3f point) const
{
	const sf::Vector2f center{ m_size / 2.f };
	const float depth{ (point.z < 0.f) ? 1.f : (m_depthCalibration * point.z) + 1.f };
	return { ((point.x - center.x) / depth) + center.x, ((point.y - center.y) / depth) + center.y };
}

sf::FloatRect Starfield3d::priv_calculateFrustumSliceBounds(float z) const
{
	z *= m_depthCalibration;
	++z;
	const sf::Vector2f center{ m_size / 2.f };
	const sf::Vector2f topLeft{ (-center * z) + center };
	const sf::Vector2f bottomRight{ (center * z) + center };
	return { topLeft, bottomRight - topLeft };
}

sf::Vector3f Starfield3d::priv_generateRandomStarPosition() const
{
	sf::Vector3f position;
	position.z = randomValue(0.f, m_maxDepth);
	position.x = randomValue(m_deepestSliceBounds.position.x, m_deepestSliceBounds.position.x + m_deepestSliceBounds.size.x);
	position.y = randomValue(m_deepestSliceBounds.position.y, m_deepestSliceBounds.position.y + m_deepestSliceBounds.size.y);
	return position;
}

sf::Vector3f Starfield3d::priv_generateRandomStarPosition(const EdgeLock edgeLock) const
{
	sf::Vector3f position;

	if (edgeLock == EdgeLock::Front)
		position.z = 0.f;
	else if (edgeLock == EdgeLock::Back)
		position.z = m_maxDepth;
	else
		position.z = randomValue(0.f, m_maxDepth);

	if (edgeLock == EdgeLock::Left)
		position.x = m_deepestSliceBounds.position.x;
	else if (edgeLock == EdgeLock::Right)
		position.x = m_deepestSliceBounds.position.x + m_deepestSliceBounds.size.x;
	else
		position.x = randomValue(m_deepestSliceBounds.position.x, m_deepestSliceBounds.position.x + m_deepestSliceBounds.size.x);

	if (edgeLock == EdgeLock::Top)
		position.y = m_deepestSliceBounds.position.y;
	else if (edgeLock == EdgeLock::Bottom)
		position.y = m_deepestSliceBounds.position.y + m_deepestSliceBounds.size.y;
	else
		position.y = randomValue(m_deepestSliceBounds.position.y, m_deepestSliceBounds.position.y + m_deepestSliceBounds.size.y);

	return position;
}

} // namespace selbaward
