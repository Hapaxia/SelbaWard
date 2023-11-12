//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Polygon
//
// Copyright(c) 2022-2023 M.J.Silk
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
#include <functional>
#include <algorithm>

namespace
{

const std::string exceptionPrefix{ "Polygon: " };

inline bool isSecondVectorAntiClockwiseOfFirstVector(const sf::Vector2f& first, const sf::Vector2f& second)
{
	return (first.x * second.y) < (first.y * second.x);
}

inline bool pointIsInsideTriangle(const std::vector<sf::Vector2f>& points, sf::Vector2f point)
{
	long double point1X{ static_cast<double>(points[0].x) };
	long double point1Y{ static_cast<double>(points[0].y) };
	long double point2X{ static_cast<double>(points[1].x) };
	long double point2Y{ static_cast<double>(points[1].y) };
	long double point3X{ static_cast<double>(points[2].x) };
	long double point3Y{ static_cast<double>(points[2].y) };
	long double pointX{ static_cast<double>(point.x) };
	long double pointY{ static_cast<double>(point.y) };

	long double denominatorMultiplier{ 1.l / ((point2Y - point3Y) * (point1X - point3X) + (point3X - point2X) * (point1Y - point3Y)) };
	long double a{ ((point2Y - point3Y) * (pointX - point3X) + (point3X - point2X) * (pointY - point3Y)) * denominatorMultiplier };
	long double b{ ((point3Y - point1Y) * (pointX - point3X) + (point1X - point3X) * (pointY - point3Y)) * denominatorMultiplier };
	long double c{ 1.l - a - b };
	return a >= 0.l && a <= 1.l && b >= 0.l && b <= 1.l && c >= 0.l && c <= 1.l;

	/*
	float denominatorMultiplier{ 1.f / ((points[1u].y - points[2u].y) * (points[0u].x - points[2u].x) + (points[2u].x - points[1u].x) * (points[0u].y - points[2u].y)) };
	float a{ ((points[1u].y - points[2u].y) * (point.x - points[2u].x) + (points[2u].x - points[1u].x) * (point.y - points[2u].y)) * denominatorMultiplier };
	float b{ ((points[2u].y - points[0u].y) * (point.x - points[2u].x) + (points[0u].x - points[2u].x) * (point.y - points[2u].y)) * denominatorMultiplier };
	float c{ 1.f - a - b };
	return a >= 0.f && a <= 1.f && b >= 0.f && b <= 1.f && c >= 0.f && c <= 1.f;
	*/
}

inline float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

inline float lengthSquared(const sf::Vector2f& a)
{
	return (a.x * a.x) + (a.y * a.y);
}

inline float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

inline float lengthSquared(const sf::Vector2f& a)
{
	return (a.x * a.x) + (a.y * a.y);
}

inline float crossProduct(const sf::Vector2f& a, const sf::Vector2f& b)
{
	return (a.x * b.y) - (a.y * b.x);
}

inline float lengthSquared(const sf::Vector2f& a)
{
	return (a.x * a.x) + (a.y * a.y);
}

} // namespace

namespace selbaward
{

Polygon::Polygon()
	: m_outputVertices()
	, m_vertices()
	, m_holeStartIndices()
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

void Polygon::addHoleStartIndex(const std::size_t index)
{
	m_holeStartIndices.push_back(index);
}

void Polygon::clearHoleStartIndices()
{
	m_holeStartIndices.clear();
}

void Polygon::setHoleStartIndices(const std::vector<std::size_t>& indices)
{
	m_holeStartIndices = indices;
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
	case TriangulationMethod::EarClip:
		priv_triangulateEarClip();
		break;
	}
}

void Polygon::priv_triangulateEarClip()
{
	constexpr std::size_t stopAfterThisNumberOfTrianglesHaveBeenCreated{ 100u };

	// ear clipping method
	// polygon points must be anti-clockwise
	// hole points must be clockwise
	// number of triangles will always be (number of points - 2)

	// vertexNumbers is the order of the vertices (can re-use vertices)
	// this is a single stream of vertices creating the polygon (after the holes are added, this becomes a single polygon)
	std::vector<std::size_t> vertexNumbers(m_vertices.size());
	std::size_t vertexNumbersSize{ vertexNumbers.size() };
	for (std::size_t i{ 0u }; i < vertexNumbersSize; ++i)
		vertexNumbers[i] = i;

	if (!m_holeStartIndices.empty())
	{
		std::sort(m_holeStartIndices.begin(), m_holeStartIndices.end());
		vertexNumbersSize = m_holeStartIndices[0u];
	}
	const std::vector<std::size_t> holeVertexNumbers(vertexNumbers.begin() + vertexNumbersSize, vertexNumbers.end());
	const std::size_t holeVertexNumbersSize{ holeVertexNumbers.size() };
	vertexNumbers.erase(vertexNumbers.begin() + vertexNumbersSize, vertexNumbers.end());
	// now, vertexNumbers is polygon vertex numbers (including any hole already cut in - later on) and holeVertexNumbers is the hole vertex numbers (all of them - no need to remove or rearrange this)

	// indices is the list of indices of the vertex numbers in order
	// each index is treated as a separate vertex, even if it's technically the same vertex as another (same vertex numbers)
	std::vector<std::size_t> indices(vertexNumbersSize);
	std::size_t indicesSize{ indices.size() };
	for (std::size_t i{ 0u }; i < indicesSize; ++i)
		indices[i] = i;

	// points lists (to keep track of their state)
	// note that these store indices - their numbers refer to which index to access: e.g. m_vertices[vertexNumbers[indices[reflex]]]
	std::vector<std::size_t> reflex;
	std::vector<std::size_t> convex;
	std::vector<std::size_t> ear;

	std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> isEar =
		[&](const std::size_t i, const std::size_t p, const std::size_t n, const std::size_t current)
	{
		bool aPointIsInside{ false };
		for (std::size_t other{ 0u }; other < vertexNumbers.size(); ++other)
		{
			if ((vertexNumbers[other] == vertexNumbers[i]) || (vertexNumbers[other] == vertexNumbers[p]) || (vertexNumbers[other] == vertexNumbers[n]) || (vertexNumbers[other] == vertexNumbers[current]))
				continue;

			if (pointIsInsideTriangle({ m_vertices[vertexNumbers[p]].position, m_vertices[vertexNumbers[i]].position, m_vertices[vertexNumbers[n]].position }, m_vertices[vertexNumbers[other]].position))
			{
				aPointIsInside = true;
				break;
			}
		}
		return !aPointIsInside;
	};

	std::function<bool(std::size_t, std::size_t, std::size_t)> isEarAnalysis =
		[&](const std::size_t i, const std::size_t p, const std::size_t n)
	{
		return isEar(i, p, n, i);
	};

	std::function<void(std::size_t, std::size_t, std::size_t, std::size_t)> retest =
		[&](const std::size_t i, const std::size_t p, const std::size_t n, const std::size_t current)
	{
		std::vector<std::size_t>::iterator reflexIt{ std::find(reflex.begin(), reflex.end(), indices[i]) };
		if (reflexIt != reflex.end())
		{
			// if reflex, re-test
			const sf::Vector2f pLine{ m_vertices[vertexNumbers[indices[i]]].position - m_vertices[vertexNumbers[indices[p]]].position };
			const sf::Vector2f nLine{ m_vertices[vertexNumbers[indices[n]]].position - m_vertices[vertexNumbers[indices[i]]].position };

			if (isSecondVectorAntiClockwiseOfFirstVector(pLine, nLine))
			{
				reflex.erase(reflexIt);
				convex.push_back(indices[i]);
			}
		}

		std::vector<std::size_t>::iterator convexIt{ std::find(convex.begin(), convex.end(), indices[i]) };
		if (convexIt != convex.end())
		{
			// if convex, re-test for ear only (must still be convex)
			const bool isNowEar{ isEar(indices[i], indices[p], indices[n], indices[current]) };
			const std::vector<std::size_t>::iterator it{ std::find(ear.begin(), ear.end(), indices[i]) };
			if (isNowEar && (it == ear.end()))
				ear.push_back(indices[i]);
			else if (!isNowEar && (it != ear.end()))
				ear.erase(it);
		}
	};

	std::function<void()> analysePoints =
		[&]()
	{
		for (std::size_t i{ 0u }; i < indicesSize; ++i)
		{
			const std::size_t prev{ (i > 0u) ? (i - 1u) : (indicesSize - 1u) };
			const std::size_t next{ (i < (indicesSize - 1u)) ? (i + 1u) : 0u };

			const sf::Vector2f prevLine{ m_vertices[vertexNumbers[indices[i]]].position - m_vertices[vertexNumbers[indices[prev]]].position };
			const sf::Vector2f nextLine{ m_vertices[vertexNumbers[indices[next]]].position - m_vertices[vertexNumbers[indices[i]]].position };

			if (!isSecondVectorAntiClockwiseOfFirstVector(prevLine, nextLine))
				reflex.push_back(indices[i]);
			else
			{
				convex.push_back(indices[i]);
				if (isEarAnalysis(indices[i], indices[prev], indices[next]))
					ear.push_back(indices[i]);
			}
		}
	};

	reflex.reserve(m_vertices.size() - 3u); // impossible for vertices to be reflex without enough convex (need at least 3 convex to make a polygon)
	convex.reserve(m_vertices.size()); // any number (up to all) of the vertices may be convex although at least 3 are required
	ear.reserve(m_vertices.size() - 2u); // with one ear per vertex, the final ear requires 3 vertices

	analysePoints();

	if (!m_holeStartIndices.empty())
	{
		struct Hole
		{
			std::size_t start;
			std::size_t length;
			float maxX;
		};
		std::vector<Hole> holeInfo(m_holeStartIndices.size());

		for (std::size_t h{ 0u }; h < holeInfo.size(); ++h)
		{
			holeInfo[h].start = m_holeStartIndices[h] - vertexNumbersSize;
			if (h < (holeInfo.size() - 1u))
				holeInfo[h].length = m_holeStartIndices[h + 1u] - m_holeStartIndices[h];
			else
				holeInfo[h].length = holeVertexNumbersSize - (m_holeStartIndices[h] - vertexNumbersSize);
			holeInfo[h].maxX = m_vertices[holeVertexNumbers[holeInfo[h].start]].position.x;
			for (std::size_t v{ 1u }; v < holeInfo[h].length; ++v)
				holeInfo[h].maxX = std::max(holeInfo[h].maxX, m_vertices[holeVertexNumbers[holeInfo[h].start + v]].position.x);
		}
		// sort order from the furthest right so that first hole is furthest right
		std::sort(holeInfo.begin(), holeInfo.end(), [](const Hole& left, const Hole& right) { return left.maxX > right.maxX; });


		float vertexMinX{ m_vertices[0u].position.x };
		float vertexMaxX{ m_vertices[0u].position.x };
		for (std::size_t v{ 1u }; v < m_vertices.size(); ++v)
		{
			vertexMinX = std::min(vertexMinX, m_vertices[v].position.x);
			vertexMaxX = std::max(vertexMaxX, m_vertices[v].position.x);
		}
		const float maxWidth{ vertexMaxX - vertexMinX };

		const sf::Vector2f rayVector{ 1.f, 0.f };

		for (std::size_t h{ 0u }; h < holeInfo.size(); ++h)
		{
			const std::size_t holeStart{ holeInfo[h].start };
			const std::size_t holeLength{ holeInfo[h].length };

			// choose where the cut appears
			std::size_t cutPolygonVertexNumber;
			std::size_t cutHoleVertexNumber;
			std::size_t cutHoleHoleIndex{ holeStart };



			// hole vertex (the easy bit)
			float furthestRight{ m_vertices[holeVertexNumbers[holeStart]].position.x };
			for (std::size_t v{ 1u }; v < holeLength; ++v)
			{
				const float currentRight{ m_vertices[holeVertexNumbers[holeStart + v]].position.x };
				if (currentRight > furthestRight)
				{
					furthestRight = currentRight;
					cutHoleHoleIndex = holeStart + v;
				}
			}
			cutHoleVertexNumber = holeVertexNumbers[cutHoleHoleIndex];



			// polygon vertex (the hard bit)
			const sf::Vector2f rayOrigin{ m_vertices[cutHoleVertexNumber].position };
			sf::Vector2f pointOfIntersection{ 0.f, 0.f };

			// find closest edge (to the right)
			std::size_t candidateIndex{ 0u };
			float distance{ maxWidth };
			bool isEndIndex{ false };
			for (std::size_t v{ 0u }; v < vertexNumbersSize; ++v)
			{
				const std::size_t edgeStartIndex{ v };
				const std::size_t edgeEndIndex{ (v < (vertexNumbersSize - 1u)) ? v + 1u : 0u };

				const sf::Vector2f edgeStart{ m_vertices[vertexNumbers[edgeStartIndex]].position };
				const sf::Vector2f edgeEnd{ m_vertices[vertexNumbers[edgeEndIndex]].position };
				const sf::Vector2f edgeVector{ edgeEnd - edgeStart };

				if (((edgeStart.x < rayOrigin.x) && (edgeEnd.x < rayOrigin.x)) || (edgeStart.y < rayOrigin.y) || (edgeEnd.y > rayOrigin.y))
					continue;

				// calculate distance (of intersection of edge)
				const float d{ crossProduct((edgeStart - rayOrigin), edgeVector) / crossProduct(rayVector, edgeVector) };

				if (d < distance)
				{
					distance = d;
					if (edgeStart.x > edgeEnd.x)
						candidateIndex = edgeStartIndex;
					else
					{
						candidateIndex = edgeEndIndex;
						isEndIndex = true;
					}
					pointOfIntersection = { rayOrigin.x + d, rayOrigin.y };
				}
			}

			const std::size_t candidateVertexNumber{ vertexNumbers[candidateIndex] };
			std::vector<std::size_t> insideTriangle;
			for (auto& r : reflex)
			{
				if ((candidateIndex == r) || (vertexNumbers[r] >= vertexNumbersSize))
					continue;

				if (pointIsInsideTriangle({ m_vertices[candidateVertexNumber].position, rayOrigin, pointOfIntersection }, m_vertices[vertexNumbers[r]].position))
					insideTriangle.push_back(r);
			}

			if (insideTriangle.empty())
				cutPolygonVertexNumber = vertexNumbers[candidateIndex];
			else
			{
				// choose the closest point to the ray origin within the triangle formed by the ray to intersection and the candidate vertex
				float distanceSquared{ maxWidth * maxWidth };
				for (auto& inside : insideTriangle)
				{
					const float thisLengthSquared{ lengthSquared(m_vertices[vertexNumbers[inside]].position - rayOrigin) };
					if (thisLengthSquared < distanceSquared)
					{
						distanceSquared = thisLengthSquared;
						cutPolygonVertexNumber = vertexNumbers[inside];
						isEndIndex = false;
					}
				}
			}



			// automatically insert vertices for the cut
			const std::size_t holeIndexOffsetCut{ cutHoleHoleIndex - holeStart };
			// prepare vertices to insert around the cut
			std::vector<std::size_t> holeInsertVertices(holeLength + 2u);
			holeInsertVertices[0u] = cutPolygonVertexNumber;
			for (std::size_t i{ 0u }; i <= holeLength; ++i)
				holeInsertVertices[i + 1u] = holeVertexNumbers[holeStart + ((holeIndexOffsetCut + i) % holeLength)];
			// then add them around the cut
			if (isEndIndex)
			{
				vertexNumbers.insert(std::find(vertexNumbers.begin(), vertexNumbers.end(), cutPolygonVertexNumber), holeInsertVertices.begin(), holeInsertVertices.end());
			}
			else
			{
				std::vector<std::size_t>::reverse_iterator vnRIt{ std::find(vertexNumbers.rbegin(), vertexNumbers.rend(), cutPolygonVertexNumber) };
				std::vector<std::size_t>::iterator vnIt{ (vnRIt + 1u).base() };
				//vertexNumbers.insert(std::find(vertexNumbers.rbegin(), vertexNumbers.rend(), cutPolygonVertexNumber).base() + 1u, holeInsertVertices.begin(), holeInsertVertices.end());
				vertexNumbers.insert(vnIt, holeInsertVertices.begin(), holeInsertVertices.end());
			}

			// indices for each of the vertex numbers (allowing re-using of vertices - needed for cutting polygon)
			vertexNumbersSize = vertexNumbers.size();
			indices.resize(vertexNumbersSize);
			indicesSize = indices.size();
			for (std::size_t i{ 0u }; i < indicesSize; ++i)
				indices[i] = i;



			//clear lists to allow re-analysis of entire new arrangement
			reflex.clear();
			convex.clear();
			ear.clear();

			// re-analyse points
			analysePoints();
		}
	}



	// avoid movement in memory when resizing
	m_triangles.clear();
	m_triangles.reserve(m_vertices.size() - 2u);



	// process
	while (indices.size() > 3u)
	{
		if (ear.empty())
			throw Exception("Polygon - ERROR: 0001");

		std::size_t currentPoint{ ear.front() };
		std::vector<std::size_t>::iterator currentIt{ std::find(indices.begin(), indices.end(), currentPoint) };
		std::size_t current{ static_cast<std::size_t>(std::distance(indices.begin(), currentIt)) };
		std::size_t prev{ (current > 0u) ? (current - 1u) : (indices.size() - 1u) };
		std::size_t next{ (current < (indices.size() - 1u)) ? (current + 1u) : 0u };

		TriangleIndices triangle{ vertexNumbers[indices[prev]], vertexNumbers[indices[current]], vertexNumbers[indices[next]] };
		m_triangles.push_back(triangle);

		retest(prev, ((prev > 0u) ? (prev - 1u) : (indices.size() - 1u)), next, current);
		retest(next, prev, ((next < (indices.size() - 1u)) ? (next + 1u) : 0u), current);

		// remove current (the one we clipped)
		convex.erase(std::find(convex.begin(), convex.end(), indices[current]));
		ear.erase(std::find(ear.begin(), ear.end(), indices[current]));
		indices.erase(currentIt);

		if (m_triangles.size() == stopAfterThisNumberOfTrianglesHaveBeenCreated)
			break;
	}

	// 3 vertices remaining; add final triangle
	if (m_triangles.size() < stopAfterThisNumberOfTrianglesHaveBeenCreated)
	{
		TriangleIndices triangle{ vertexNumbers[indices[0u]], vertexNumbers[indices[1u]], vertexNumbers[indices[2u]] };
		m_triangles.push_back(triangle);
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
	ear.reserve(m_vertices.size() - 2u); // with one ear per vertex, the final ear requires 3 vertices

	std::function<bool(std::size_t, std::size_t, std::size_t, std::size_t)> isEar =
		[&](const std::size_t i, const std::size_t p, const std::size_t n, const std::size_t current)
	{
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
		return !aPointIsInside;
	};

	std::function<bool(std::size_t, std::size_t, std::size_t)> isEarAnalysis =
		[&](const std::size_t i, const std::size_t p, const std::size_t n)
	{
		return isEar(i, p, n, i);
	};

	std::function<void(std::size_t, std::size_t, std::size_t, std::size_t)> retest =
		[&](const std::size_t i, const std::size_t p, const std::size_t n, const std::size_t current)
	{
		std::vector<std::size_t>::iterator reflexIt{ std::find(reflex.begin(), reflex.end(), indices[i]) };
		if (reflexIt != reflex.end())
		{
			// if reflex, re-test
			const sf::Vector2f pLine{ m_vertices[indices[i]].position - m_vertices[indices[p]].position };
			const sf::Vector2f nLine{ m_vertices[indices[n]].position - m_vertices[indices[i]].position };

			if (isSecondVectorAntiClockwiseOfFirstVector(pLine, nLine))
			{
				reflex.erase(reflexIt);
				convex.push_back(indices[i]);
			}
		}

		std::vector<std::size_t>::iterator convexIt{ std::find(convex.begin(), convex.end(), indices[i]) };
		if (convexIt != convex.end())
		{
			// if convex, re-test for ear only (must still be convex)
			const bool isNowEar{ isEar(i, p, n, current) };
			const std::vector<std::size_t>::iterator it{ std::find(ear.begin(), ear.end(), indices[i]) };
			if (isNowEar && (it == ear.end()))
				ear.push_back(indices[i]);
			else if (!isNowEar && (it != ear.end()))
				ear.erase(it);
		}
	};

	// analyse points
	for (std::size_t i{ 0u }; i < indicesSize; ++i)
	{
		const std::size_t prev{ (i > 0u) ? (i - 1u) : (indicesSize - 1u) };
		const std::size_t next{ (i < (indicesSize - 1u)) ? (i + 1u) : 0u };

		const sf::Vector2f prevLine{ m_vertices[indices[i]].position - m_vertices[indices[prev]].position };
		const sf::Vector2f nextLine{ m_vertices[indices[next]].position - m_vertices[indices[i]].position };

		if (!isSecondVectorAntiClockwiseOfFirstVector(prevLine, nextLine))
			reflex.push_back(indices[i]);
		else
		{
			convex.push_back(i);
			if (isEarAnalysis(i, prev, next))
				ear.push_back(indices[i]);
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

		retest(prev, ((prev > 0u) ? (prev - 1u) : (indices.size() - 1u)), next, current);
		retest(next, prev, ((next < (indices.size() - 1u)) ? (next + 1u) : 0u), current);

		// remove current (the one we clipped)
		convex.erase(std::find(convex.begin(), convex.end(), indices[current]));
		ear.erase(std::find(ear.begin(), ear.end(), indices[current]));
		indices.erase(currentIt);

		if (m_triangles.size() == stopAfterThisNumberOfTrianglesHaveBeenCreated)
			break;
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
