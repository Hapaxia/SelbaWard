//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Polygon
//
// Copyright(c) 2022 M.J.Silk
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

#include "Polygon.hpp"

#include <assert.h>

namespace
{

const std::string exceptionPrefix{ "Polygon: " };

inline bool isSecondVectorAntiClockwiseOfFirstVector(const sf::Vector2f& first, const sf::Vector2f& second)
{
	return (first.x * second.y) < (first.y * second.x);
}

inline bool pointIsInsideTriangle(const std::vector<sf::Vector2f>& points, sf::Vector2f point)
{
	float denominatorMultiplier{ 1.f / ((points[1u].y - points[2u].y) * (points[0u].x - points[2u].x) + (points[2u].x - points[1u].x) * (points[0u].y - points[2u].y)) };
	float a{ ((points[1u].y - points[2u].y) * (point.x - points[2u].x) + (points[2u].x - points[1u].x) * (point.y - points[2u].y)) * denominatorMultiplier };
	float b{ ((points[2u].y - points[0u].y) * (point.x - points[2u].x) + (points[0u].x - points[2u].x) * (point.y - points[2u].y)) * denominatorMultiplier };
	float c{ 1.f - a - b };
	return a >= 0.f && a <= 1.f && b >= 0.f && b <= 1.f && c >= 0.f && c <= 1.f;
}

} // namespace

namespace selbaward
{

Polygon::Polygon()
	: m_outputVertices()
	, m_vertices()
	, m_color{ sf::Color::White }
	, m_triangulationMethod{ TriangulationMethod::BasicEarClip }
	, m_meshRefinementMethod{ MeshRefinementMethod::None }
	, m_throwExceptions{ true }
{

}

void Polygon::update()
{
	priv_update();
}

void Polygon::setColor(const sf::Color color)
{
	m_color = color;
}

sf::Color Polygon::getColor()
{
	return m_color;
}

void Polygon::setTriangulationMethod(const TriangulationMethod triangulationMethod)
{
	m_triangulationMethod = triangulationMethod;
}

Polygon::TriangulationMethod Polygon::getTriangulationMethod() const
{
	return m_triangulationMethod;
}

void Polygon::setMeshRefinementMethod(const MeshRefinementMethod meshRefinementMethod)
{
	m_meshRefinementMethod = meshRefinementMethod;
}

Polygon::MeshRefinementMethod Polygon::getMeshRefinementMethod() const
{
	return m_meshRefinementMethod;
}

void Polygon::reserveVertices(const std::size_t numberOfVertices)
{
	if (numberOfVertices == 0)
		return;

	m_vertices.reserve(numberOfVertices);
	m_outputVertices.reserve(m_vertices.size() * 3u);
}

void Polygon::setNumberOfVertices(const std::size_t numberOfVertices)
{
	m_vertices.resize(numberOfVertices);
}

std::size_t Polygon::getNumberOfVertices() const
{
	return m_vertices.size();
}

void Polygon::setVertexPosition(const std::size_t index, const sf::Vector2f position)
{
	if (!priv_testVertexIndex(index, "Cannot set vertex position."))
		return;

	m_vertices[index].position = position;
}

sf::Vector2f Polygon::getVertexPosition(const std::size_t index) const
{
	if (!priv_testVertexIndex(index, "Cannot get vertex position."))
		return{ 0.f, 0.f };

	return m_vertices[index].position;
}

void Polygon::reverseVertices()
{
	std::reverse(m_vertices.begin(), m_vertices.end());
}

void Polygon::importVertexPositions(const std::vector<sf::Vector2f>& positions)
{
	setNumberOfVertices(positions.size());
	for (std::size_t i{ 0u }; i < positions.size(); ++i)
		m_vertices[i].position = positions[i];
}

std::vector<sf::Vector2f> Polygon::exportTriangulatedPositions() const
{
	std::vector<sf::Vector2f> positions(m_triangles.size() * 3u);
	for (std::size_t i{ 0u }; i < m_triangles.size(); ++i)
	{
		positions[i * 3u + 0u] = m_vertices[m_triangles[i][0u]].position;
		positions[i * 3u + 1u] = m_vertices[m_triangles[i][1u]].position;
		positions[i * 3u + 2u] = m_vertices[m_triangles[i][2u]].position;
	}
	return positions;
}






// PRIVATE

void Polygon::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = nullptr;
	states.transform *= getTransform();
	if (m_outputVertices.size() > 0)
		target.draw(m_outputVertices.data(), m_outputVertices.size(), sf::Triangles, states);
}

void Polygon::priv_update()
{
	priv_triangulate();
	priv_updateOutputVertices();
}

void Polygon::priv_updateOutputVertices()
{
	m_outputVertices.resize(m_triangles.size() * 3u);
	for (std::size_t t{ 0u }; t < m_triangles.size(); ++t)
	{
		const std::size_t baseIndex{ t * 3u };
		for (std::size_t v{ 0u }; v < 3u; ++v)
		{
			m_outputVertices[baseIndex + v].position = m_vertices[m_triangles[t][v]].position;
			m_outputVertices[baseIndex + v].color = m_color;
		}
	}
}

void Polygon::priv_triangulate()
{
	assert(m_vertices.size() > 2u); // MUST have more than 2 points to be triangulatable.

	switch (m_triangulationMethod)
	{
	case TriangulationMethod::BasicEarClip:
		priv_triangulateBasicEarClip();
		break;
	}
}

void Polygon::priv_triangulateBasicEarClip()
{
	constexpr std::size_t stopAfterThisNumberOfTrianglesHaveBeenCreated{ 100u };

	// ear clipping method
	// polygon points must be anti-clockwise
	// number of triangles will always be (number of points - 2)

	// avoid movement in memory when resizing
	m_triangles.clear();
	m_triangles.reserve(m_vertices.size() - 2u);

	// indices for each of the vertices/points
	std::vector<std::size_t> indices(m_vertices.size());
	const size_t indicesSize{ indices.size() };
	for (std::size_t i{ 0u }; i < indicesSize; ++i)
		indices[i] = i;

	// points lists (to keep track of their state)
	std::vector<std::size_t> reflex;
	std::vector<std::size_t> convex;
	std::vector<std::size_t> ear;
	reflex.reserve(m_vertices.size() - 3u); // impossible for vertices to be reflex without enough convex (need at least 3 convex to make a polygon)
	convex.reserve(m_vertices.size());
	ear.reserve(m_vertices.size());

	// analyse points
	for (std::size_t i{ 0u }; i < indicesSize; ++i)
	{
		const std::size_t prev{ (i > 0u) ? (i - 1u) : (indicesSize - 1u) };
		const std::size_t next{ (i < (indicesSize - 1u)) ? (i + 1u) : 0u };

		const sf::Vector2f prevLine{ m_vertices[indices[i]].position - m_vertices[indices[prev]].position };
		const sf::Vector2f nextLine{ m_vertices[indices[next]].position - m_vertices[indices[i]].position };

		if (isSecondVectorAntiClockwiseOfFirstVector(prevLine, nextLine))
		{
			convex.push_back(i);
			bool aPointIsInside{ false };
			for (std::size_t other{ 0u }; other < indicesSize; ++other)
			{
				if ((other == i) || (other == prev) || (other == next))
					continue;

				if (pointIsInsideTriangle({ m_vertices[indices[prev]].position, m_vertices[indices[i]].position, m_vertices[indices[next]].position }, m_vertices[indices[other]].position))
				{
					aPointIsInside = true;
					break;
				}
			}
			if (!aPointIsInside)
				ear.push_back(indices[i]);
		}
		else
		{
			reflex.push_back(indices[i]);
		}
	}

	// process
	while (indices.size() > 3u)
	{
		std::size_t currentPoint{ ear.front() };
		std::vector<std::size_t>::iterator currentIt{ std::find(indices.begin(), indices.end(), currentPoint) };
		std::size_t current{ static_cast<std::size_t>(std::distance(indices.begin(), currentIt)) };
		std::size_t prev{ (current > 0u) ? (current - 1u) : (indices.size() - 1u) };
		std::size_t next{ (current < (indices.size() - 1u)) ? (current + 1u) : 0u };

		TriangleIndices triangle{ indices[prev], indices[current], indices[next] };
		m_triangles.push_back(triangle);



		std::vector<std::size_t>::iterator earPrevIt{ std::find(ear.begin(), ear.end(), indices[prev]) };
		if (earPrevIt != ear.end())
		{
			// if ear, re-test for ear only (might not still be one)

			std::size_t i{ prev };

			const std::size_t p{ (i > 0u) ? (i - 1u) : (indices.size() - 1u) };
			const std::size_t n{ next };

			bool aPointIsInside{ false };
			for (std::size_t other{ 0u }; other < indices.size(); ++other)
			{
				if ((other == i) || (other == p) || (other == n) || (other == current))
					continue;

				if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
				{
					aPointIsInside = true;
					break;
				}
			}
			if (aPointIsInside)
				ear.erase(earPrevIt);
		}



		std::vector<std::size_t>::iterator earNextIt{ std::find(ear.begin(), ear.end(), indices[next]) };
		if (earNextIt != ear.end())
		{
			// if ear, re-test for ear only (might not still be one)

			std::size_t i{ next };

			const std::size_t p{ prev };
			const std::size_t n{ (i < (indices.size() - 1u)) ? (i + 1u) : 0u };

			bool aPointIsInside{ false };
			for (std::size_t other{ 0u }; other < indices.size(); ++other)
			{
				if ((other == i) || (other == p) || (other == n) || (other == current))
					continue;

				if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
				{
					aPointIsInside = true;
					break;
				}
			}
			if (aPointIsInside)
				ear.erase(earNextIt);
		}



		std::vector<std::size_t>::iterator convexPrevIt{ std::find(convex.begin(), convex.end(), indices[prev]) };
		if (convexPrevIt != convex.end())
		{
			// if convex, re-test for ear only (must still be convex)

			if (std::find(ear.begin(), ear.end(), indices[prev]) == ear.end())
			{
				std::size_t i{ prev };

				const std::size_t p{ (i > 0u) ? (i - 1u) : (indices.size() - 1u) };
				const std::size_t n{ next };

				bool aPointIsInside{ false };
				for (std::size_t other{ 0u }; other < indices.size(); ++other)
				{
					if ((other == i) || (other == p) || (other == n) || (other == current))
						continue;

					if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
					{
						aPointIsInside = true;
						break;
					}
				}
				if (!aPointIsInside)
					ear.push_back(indices[i]);
			}
		}



		std::vector<std::size_t>::iterator convexNextIt{ std::find(convex.begin(), convex.end(), indices[next]) };
		if (convexNextIt != convex.end())
		{
			// if convex, re-test for ear only (must still be convex)

			if (std::find(ear.begin(), ear.end(), indices[next]) == ear.end())
			{
				std::size_t i{ next };

				const std::size_t p{ prev };
				const std::size_t n{ (i < (indices.size() - 1u)) ? (i + 1u) : 0u };

				bool aPointIsInside{ false };
				for (std::size_t other{ 0u }; other < indices.size(); ++other)
				{
					if ((other == i) || (other == p) || (other == n) || (other == current))
						continue;

					if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
					{
						aPointIsInside = true;
						break;
					}
				}
				if (!aPointIsInside)
					ear.push_back(indices[i]);
			}
		}



		std::vector<std::size_t>::iterator reflexPrevIt{ std::find(reflex.begin(), reflex.end(), indices[prev]) };
		if (reflexPrevIt != reflex.end())
		{
			// if reflex, re-test
			std::size_t i{ prev };

			const std::size_t p{ (i > 0u) ? (i - 1u) : (indices.size() - 1u) };
			const std::size_t n{ next };

			const sf::Vector2f pLine{ m_vertices[indices[i]].position - m_vertices[indices[p]].position };
			const sf::Vector2f nLine{ m_vertices[indices[n]].position - m_vertices[indices[i]].position };

			if (isSecondVectorAntiClockwiseOfFirstVector(pLine, nLine))
			{
				reflex.erase(reflexPrevIt);

				convex.push_back(indices[i]);
				bool aPointIsInside{ false };
				for (std::size_t other{ 0u }; other < indices.size(); ++other)
				{
					if ((other == i) || (other == p) || (other == n) || (other == current))
						continue;

					if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
					{
						aPointIsInside = true;
						break;
					}
				}
				if (!aPointIsInside)
					ear.push_back(indices[i]);
			}
		}

		std::vector<std::size_t>::iterator reflexNextIt{ std::find(reflex.begin(), reflex.end(), indices[next]) };
		if (reflexNextIt != reflex.end())
		{
			// if reflex, re-test
			std::size_t i{ next };

			const std::size_t p{ prev };
			const std::size_t n{ (i < (indices.size() - 1u)) ? (i + 1u) : 0u };

			const sf::Vector2f pLine{ m_vertices[indices[i]].position - m_vertices[indices[p]].position };
			const sf::Vector2f nLine{ m_vertices[indices[n]].position - m_vertices[indices[i]].position };

			if (isSecondVectorAntiClockwiseOfFirstVector(pLine, nLine))
			{
				reflex.erase(reflexNextIt);

				convex.push_back(indices[i]);
				bool aPointIsInside{ false };
				for (std::size_t other{ 0u }; other < indices.size(); ++other)
				{
					if ((other == i) || (other == p) || (other == n) || (other == current))
						continue;

					if (pointIsInsideTriangle({ m_vertices[indices[p]].position, m_vertices[indices[i]].position, m_vertices[indices[n]].position }, m_vertices[indices[other]].position))
					{
						aPointIsInside = true;
						break;
					}
				}
				if (!aPointIsInside)
					ear.push_back(indices[i]);
			}
		}

		convex.erase(std::find(convex.begin(), convex.end(), indices[current]));
		if (std::find(ear.begin(), ear.end(), indices[current]) != ear.end())
			ear.erase(std::find(ear.begin(), ear.end(), indices[current]));
		indices.erase(currentIt);

		///*
		if (m_triangles.size() == stopAfterThisNumberOfTrianglesHaveBeenCreated)
			break;
		//*/
	}

	if (m_triangles.size() < stopAfterThisNumberOfTrianglesHaveBeenCreated)
	{
		TriangleIndices triangle{ indices[0u], indices[1u], indices[2u] };
		m_triangles.push_back(triangle);
	}
}

bool Polygon::priv_isValidVertexIndex(const std::size_t vertexIndex) const
{
	return vertexIndex < m_vertices.size();
}

bool Polygon::priv_testVertexIndex(const std::size_t vertexIndex, const std::string& exceptionMessage) const
{
	if (!priv_isValidVertexIndex(vertexIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + exceptionMessage + " Vertex index (" + std::to_string(vertexIndex) + ") out of range");
		return false;
	}
	return true;
}

} // namespace selbaward
