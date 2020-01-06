//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Spline
//
// Copyright(c) 2014-2020 M.J.Silk
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

#include "Spline.hpp"

#include <cmath>
#include <assert.h>
#include <random>
#include <algorithm>
#include <initializer_list>

namespace
{

const std::string exceptionPrefix = "Spline: ";
constexpr float thicknessEpsilon{ 0.001f };
constexpr float zeroEpsilon{ 0.00001f };
constexpr float pi{ 3.141592653f };
#ifdef USE_SFML_PRE_2_4
const sf::PrimitiveType thickPrimitiveType{ sf::PrimitiveType::TrianglesStrip };
#else // USE_SFML_PRE_2_4
const sf::PrimitiveType thickPrimitiveType{ sf::PrimitiveType::TriangleStrip };
#endif // USE_SFML_PRE_2_4

std::mt19937 randomGenerator;

inline void randomSeed()
{
	std::random_device rd;
	randomGenerator.seed(rd());
}

inline float randomValue(const float range)
{
	assert(range > 0.f);
	return std::uniform_real_distribution<float>{ 0.f, range }(randomGenerator);
}

template <class T>
inline T linearInterpolation(const T start, const T end, const float alpha)
{
	return static_cast<T>(end * alpha + start * (1 - alpha));
}

sf::Color linearInterpolation(const sf::Color start, const sf::Color end, const float alpha)
{
	return sf::Color(linearInterpolation(start.r, end.r, alpha), linearInterpolation(start.g, end.g, alpha), linearInterpolation(start.b, end.b, alpha));
}

inline sf::Vector2f bezierInterpolation(const sf::Vector2f start, const sf::Vector2f end, const sf::Vector2f startHandle, const sf::Vector2f endHandle, const float alpha)
{
	const sf::Vector2f startControl{ start + startHandle };
	const sf::Vector2f endControl{ end + endHandle };
	const float alpha2{ 1.f - alpha };
	const float alphaSquared{ alpha * alpha };
	const float alpha2Squared{ alpha2 * alpha2 };

	return
	{
		start.x * alpha2Squared * alpha2 + startControl.x * 3 * alpha2Squared * alpha + endControl.x * 3 * alpha2 * alphaSquared + end.x * alpha * alphaSquared,
		start.y * alpha2Squared * alpha2 + startControl.y * 3 * alpha2Squared * alpha + endControl.y * 3 * alpha2 * alphaSquared + end.y * alpha * alphaSquared
	};
}

inline float dot(const sf::Vector2f a, const sf::Vector2f b)
{
	return a.x * b.x + a.y * b.y;
}

inline float vectorLength(const sf::Vector2f vector)
{
	return std::sqrt(dot(vector, vector));
}

inline void copyAngle(const sf::Vector2f source, sf::Vector2f destination)
{
	destination = -(source / vectorLength(source)) * vectorLength(destination);
}

template <class T>
inline T abs(const T value)
{
	return value < 0 ? -value : value;
}

inline bool isConsideredZero(const float value)
{
	return abs(value) < zeroEpsilon;
}

inline sf::Vector2f vectorNormal(const sf::Vector2f vector)
{
	return{ vector.y, -vector.x };
}

inline sf::Vector2f vectorUnit(const sf::Vector2f vector)
{
	if (isConsideredZero(vector.x) && isConsideredZero(vector.y))
		return{ 0.f, 0.f };
	else
		return vector / vectorLength(vector);
}

} // namespace

namespace selbaward
{

Spline::Spline(const unsigned int vertexCount, const sf::Vector2f initialPosition)
	: m_throwExceptions{ true }
	, m_isClosed{ false }
	, m_thickCornerType{ ThickCornerType::Point }
	, m_thickStartCapType{ ThickCapType::None }
	, m_thickEndCapType{ ThickCapType::None }
	, m_roundedThickCornerInterpolationLevel{ 5u }
	, m_roundedThickStartCapInterpolationLevel{ 5u }
	, m_roundedThickEndCapInterpolationLevel{ 5u }
	, m_maxPointLength{ 100.f }
	, m_automaticallyUpdateRandomNormalOffset{ true }
	, m_vertices(vertexCount, Vertex(initialPosition))
	, m_color(sf::Color::White)
	, m_thickness{ 0.f }
	, m_randomNormalOffsetRange{ 0.f }
	, m_interpolatedVertices()
	, m_interpolatedVerticesUnitTangents()
	, m_outputVertices()
#ifdef USE_SFML_PRE_2_4
	, m_primitiveType(sf::PrimitiveType::LinesStrip)
#else // USE_SFML_PRE_2_4
	, m_primitiveType(sf::PrimitiveType::LineStrip)
#endif // USE_SFML_PRE_2_4
	, m_interpolationSteps{ 0u }
	, m_useBezier{ false }
	, m_handlesVertices()
	, m_showHandles{ false }
	, m_lockHandleMirror{ true }
	, m_lockHandleAngle{ true }
{
	randomSeed();
}

Spline::Spline(std::initializer_list<sf::Vector2f> list)
	: Spline(static_cast<unsigned int>(list.size()))
{
	unsigned int index{ 0u };
	for (auto& position : list)
		m_vertices[index++].position = position;
}

float Spline::getLength() const
{
	if (m_vertices.size() < 2)
		return 0.f;

	float total{ 0.f };
	for (unsigned int v{ 0 }; v < getLastVertexIndex(); ++v)
		total += vectorLength(m_vertices[v + 1].position - m_vertices[v].position);
	return total;
}

float Spline::getInterpolatedLength() const
{
	if (m_interpolatedVertices.size() < 2)
		return 0.f;

	float total{ 0.f };
	for (std::vector<sf::Vertex>::const_iterator begin{ m_interpolatedVertices.begin() }, end{ m_interpolatedVertices.end() - 1 }, it{ begin }; it != end; ++it)
		total += vectorLength((it + 1)->position - it->position);
	return total;
}

void Spline::update()
{
	if (m_vertices.size() < 2)
	{
		m_interpolatedVertices.clear();
		m_outputVertices.clear();
		m_handlesVertices.clear();
		m_interpolatedVerticesUnitTangents.clear();
		return;
	}

	const unsigned int pointsPerVertex{ priv_getNumberOfPointsPerVertex() };
	if (m_isClosed)
		m_interpolatedVertices.resize((m_vertices.size() * pointsPerVertex) + 1);
	else
		m_interpolatedVertices.resize(((m_vertices.size() - 1) * pointsPerVertex) + 1);
	m_interpolatedVerticesUnitTangents.resize(m_interpolatedVertices.size());

	m_handlesVertices.resize((m_vertices.size()) * 4);
	std::vector<sf::Vertex>::iterator itHandle{ m_handlesVertices.begin() };

	for (std::vector<Vertex>::iterator begin{ m_vertices.begin() }, end{ m_vertices.end() }, it{ begin }; it != end; ++it)
	{
		itHandle->color = sf::Color(255, 255, 128, 32);
		itHandle++->position = it->position;
		itHandle->color = sf::Color(0, 255, 0, 128);
		itHandle++->position = it->position + it->backHandle;
		itHandle->color = sf::Color(255, 255, 128, 32);
		itHandle++->position = it->position;
		itHandle->color = sf::Color(0, 255, 0, 128);
		itHandle++->position = it->position + it->frontHandle;

		std::vector<sf::Vertex>::iterator itInterpolated{ m_interpolatedVertices.begin() + (it - begin) * pointsPerVertex };
		if (m_isClosed || it != end - 1)
		{
			for (unsigned int i{ 0u }; i < pointsPerVertex; ++i)
			{
				std::vector<Vertex>::iterator nextIt{ m_vertices.begin() };
				if (it != end - 1)
					nextIt = it + 1;
				if (m_useBezier)
					itInterpolated->position = bezierInterpolation(it->position, nextIt->position, it->frontHandle, nextIt->backHandle, static_cast<float>(i) / pointsPerVertex);
				else
					itInterpolated->position = linearInterpolation(it->position, nextIt->position, static_cast<float>(i) / pointsPerVertex);
				itInterpolated++->color = m_color;
			}
		}
		else
		{
			itInterpolated->position = it->position;
			itInterpolated->color = m_color;
		}
	}
	if (m_isClosed)
	{
		std::vector<sf::Vertex>::iterator itInterpolated{ m_interpolatedVertices.begin() + m_vertices.size() * pointsPerVertex };
		itInterpolated->position = m_vertices.front().position;
		itInterpolated->color = m_color;
	}

	// calculate tangents
	for (std::vector<sf::Vertex>::iterator begin{ m_interpolatedVertices.begin() }, end{ m_interpolatedVertices.end() }, current{ begin }; current != end; ++current)
	{
		std::vector<sf::Vertex>::iterator next{ current };
		std::vector<sf::Vertex>::iterator previous{ current };
		if (current != begin)
			previous = current - 1;
		else if (m_isClosed)
			previous = end - 2;
		if (current != end - 1)
			next = current + 1;
		else if (m_isClosed)
			next = begin + 1;

		const sf::Vector2f nextVectorUnit{ vectorUnit(next->position - current->position) };
		const sf::Vector2f previousVectorUnit{ vectorUnit(current->position - previous->position) };

		sf::Vector2f& tangent{ *(m_interpolatedVerticesUnitTangents.begin() + (current - begin)) };
		tangent = vectorUnit(previousVectorUnit + nextVectorUnit);
	}

	priv_updateOutputVertices();
}

void Spline::updateOutputVertices()
{
	priv_updateOutputVertices();
}

void Spline::setClosed(const bool isClosed)
{
	m_isClosed = isClosed;
}

void Spline::setThickCornerType(const ThickCornerType thickCornerType)
{
	m_thickCornerType = thickCornerType;
}

void Spline::setRoundedThickCornerInterpolationLevel(const unsigned int roundedThickCornerInterpolationLevel)
{
	m_roundedThickCornerInterpolationLevel = roundedThickCornerInterpolationLevel;
}

void Spline::setThickStartCapType(const ThickCapType thickStartCapType)
{
	m_thickStartCapType = thickStartCapType;
}

void Spline::setRoundedThickStartCapInterpolationLevel(const unsigned int roundedThickStartCapInterpolationLevel)
{
	m_roundedThickStartCapInterpolationLevel = roundedThickStartCapInterpolationLevel;
}

void Spline::setThickEndCapType(const ThickCapType thickEndCapType)
{
	m_thickEndCapType = thickEndCapType;
}

void Spline::setRoundedThickEndCapInterpolationLevel(const unsigned int roundedThickEndCapInterpolationLevel)
{
	m_roundedThickEndCapInterpolationLevel = roundedThickEndCapInterpolationLevel;
}

void Spline::setMaxCornerPointLength(const float maxCornerPointLength)
{
	m_maxPointLength = maxCornerPointLength;
}

void Spline::reserveVertices(const unsigned int numberOfVertices)
{
	if (numberOfVertices == 0)
		return;

	m_vertices.reserve(numberOfVertices);
	m_interpolatedVertices.reserve(numberOfVertices * priv_getNumberOfPointsPerVertex() + 1);
	m_outputVertices.reserve(m_interpolatedVertices.size() * 2);
	m_handlesVertices.reserve(numberOfVertices * 4);
}

void Spline::addVertices(const std::vector<sf::Vector2f>& positions)
{
	for (auto& position : positions)
		addVertex(position);
}

void Spline::addVertices(const unsigned int index, const std::vector<sf::Vector2f>& positions)
{
	for (auto& position : positions)
		addVertex(index, position);
}

void Spline::addVertices(const unsigned int numberOfVertices, const sf::Vector2f position)
{
	for (unsigned int i{ 0u }; i < numberOfVertices; ++i)
		addVertex(position);
}

void Spline::addVertices(const unsigned int numberOfVertices, const unsigned int index, const sf::Vector2f position)
{
	for (unsigned int i{ 0u }; i < numberOfVertices; ++i)
		addVertex(index + i, position);
}

void Spline::addVertex(const sf::Vector2f position)
{
	m_vertices.emplace_back(Vertex(position));
}

void Spline::addVertex(const unsigned int index, const sf::Vector2f position)
{
	if (index < getVertexCount())
		m_vertices.insert(m_vertices.begin() + index, Vertex(position));
	else
		addVertex(position);
}

void Spline::removeVertex(const unsigned int index)
{
	if (!priv_testVertexIndex(index, "Cannot remove vertex."))
		return;

	m_vertices.erase(m_vertices.begin() + index);
}

void Spline::removeVertices(const unsigned int index, const unsigned int numberOfVertices)
{
	if (!priv_testVertexIndex(index, "Cannot remove vertices") || ((numberOfVertices > 1) && (!priv_testVertexIndex(index + numberOfVertices - 1, "Cannot remove vertices"))))
		return;

	if (numberOfVertices == 0)
		m_vertices.erase(m_vertices.begin() + index, m_vertices.end());
	else
		m_vertices.erase(m_vertices.begin() + index, m_vertices.begin() + index + numberOfVertices);
}

void Spline::reverseVertices()
{
	std::reverse(m_vertices.begin(), m_vertices.end());

	// swap all handles
	sf::Vector2f tempHandle;
	for (auto& vertex : m_vertices)
	{
		tempHandle = vertex.frontHandle;
		vertex.frontHandle = vertex.backHandle;
		vertex.backHandle = tempHandle;
	}
}

void Spline::setPosition(const unsigned int index, const sf::Vector2f position)
{
	if (!priv_testVertexIndex(index, "Cannot set vertex position."))
		return;

	m_vertices[index].position = position;
}

void Spline::setPositions(const unsigned int index, unsigned int numberOfVertices, const sf::Vector2f position)
{
	if (!priv_testVertexIndex(index, "Cannot set vertices' positions") || (numberOfVertices > 1 && !priv_testVertexIndex(index + numberOfVertices - 1, "Cannot set vertices' positions")))
		return;

	if (numberOfVertices == 0)
		numberOfVertices = static_cast<unsigned int>(m_vertices.size()) - index;

	for (unsigned int v{ 0u }; v < numberOfVertices; ++v)
		m_vertices[index + v].position = position;
}

void Spline::setPositions(const std::vector<sf::Vector2f>& positions, unsigned int index)
{
	const unsigned int numberOfVertices{ static_cast<unsigned int>(positions.size()) };
	if ((numberOfVertices < 1) || (!priv_testVertexIndex(index, "Cannot set vertices' positions")) || ((numberOfVertices > 1) && (!priv_testVertexIndex(index + numberOfVertices - 1, "Cannot set vertices' positions"))))
		return;

	for (auto& position : positions)
	{
		m_vertices[index].position = position;
		++index;
	}
}

sf::Vector2f Spline::getPosition(const unsigned int index) const
{
	if (!priv_testVertexIndex(index, "Cannot get vertex position."))
		return{ 0.f, 0.f };

	return m_vertices[index].position;
}

void Spline::setFrontHandle(const unsigned int index, const sf::Vector2f offset)
{
	if (!priv_testVertexIndex(index, "Cannot set vertex front handle."))
		return;

	m_vertices[index].frontHandle = offset;
	if (m_lockHandleMirror)
		m_vertices[index].backHandle = -offset;
	else if (m_lockHandleAngle)
		copyAngle(m_vertices[index].frontHandle, m_vertices[index].backHandle);
}

sf::Vector2f Spline::getFrontHandle(const unsigned int index) const
{
	if (!priv_testVertexIndex(index, "Cannot get vertex front handle."))
		return{ 0.f, 0.f };

	return m_vertices[index].frontHandle;
}

void Spline::setBackHandle(const unsigned int index, const sf::Vector2f offset)
{
	if (!priv_testVertexIndex(index, "Cannot set vertex back handle."))
		return;

	m_vertices[index].backHandle = offset;
	if (m_lockHandleMirror)
		m_vertices[index].frontHandle = -offset;
	else if (m_lockHandleAngle)
		copyAngle(m_vertices[index].backHandle, m_vertices[index].frontHandle);
}

sf::Vector2f Spline::getBackHandle(const unsigned int index) const
{
	if (!priv_testVertexIndex(index, "Cannot get vertex back handle."))
		return{ 0.f, 0.f };

	return m_vertices[index].backHandle;
}

void Spline::resetHandles(const unsigned int index, unsigned int numberOfVertices)
{
	if ((!priv_testVertexIndex(index, "Cannot reset vertices' handles")) || ((numberOfVertices > 1) && (!priv_testVertexIndex(index + numberOfVertices - 1, "Cannot reset vertices' handles"))))
		return;

	if (numberOfVertices == 0)
		numberOfVertices = static_cast<unsigned int>(m_vertices.size()) - index;

	for (unsigned int v{ 0u }; v < numberOfVertices; ++v)
	{
		m_vertices[index + v].frontHandle = { 0.f, 0.f };
		m_vertices[index + v].backHandle = { 0.f, 0.f };
	}
}

void Spline::smoothHandles()
{
	for (unsigned int v{ 0 }; v < m_vertices.size() - 1; ++v)
	{
		const sf::Vector2f p1{ m_vertices[v].position };
		const sf::Vector2f p2{ m_vertices[v + 1].position };
		sf::Vector2f p0{ p1 };
		sf::Vector2f p3{ p2 };
		if (v > 0)
			p0 = m_vertices[v - 1].position;
		if (v < m_vertices.size() - 2)
			p3 = m_vertices[v + 2].position;

		const sf::Vector2f m0{ linearInterpolation(p0, p1, 0.5f) };
		const sf::Vector2f m1{ linearInterpolation(p1, p2, 0.5f) };
		const sf::Vector2f m2{ linearInterpolation(p2, p3, 0.5f) };

		const float p01{ vectorLength(p1 - p0) };
		const float p12{ vectorLength(p2 - p1) };
		const float p23{ vectorLength(p3 - p2) };
		float proportion0{ 0.f };
		float proportion1{ 0.f };
		if (p01 + p12 != 0.f)
			proportion0 = p01 / (p01 + p12);
		if (p12 + p23 != 0.f)
			proportion1 = p12 / (p12 + p23);

		const sf::Vector2f q0{ linearInterpolation(m0, m1, proportion0) };
		const sf::Vector2f q1{ linearInterpolation(m1, m2, proportion1) };

		m_vertices[v].frontHandle = m1 - q0;
		m_vertices[v + 1].backHandle = m1 - q1;
	}
	m_vertices.front().backHandle = { 0.f, 0.f };
	m_vertices.back().frontHandle = { 0.f, 0.f };
}

void Spline::setHandlesVisible(const bool handlesVisible)
{
	m_showHandles = handlesVisible;
}

void Spline::setColor(const sf::Color color)
{
	m_color = color;
}

void Spline::setColor(const unsigned int index, const sf::Color color)
{
	m_vertices[index].color = color;
}

void Spline::setInterpolationSteps(const unsigned int interpolationSteps)
{
	m_interpolationSteps = interpolationSteps;
}

void Spline::setHandleAngleLock(const bool handleAngleLock)
{
	m_lockHandleAngle = handleAngleLock;
}

void Spline::setHandleMirrorLock(const bool handleMirrorLock)
{
	m_lockHandleMirror = handleMirrorLock;
}

void Spline::setBezierInterpolation(const bool bezierInterpolation)
{
	m_useBezier = bezierInterpolation;
}

void Spline::setPrimitiveType(const sf::PrimitiveType primitiveType)
{
	m_primitiveType = primitiveType;
}

sf::Vector2f Spline::getInterpolatedPosition(const unsigned int interpolationOffset, const unsigned int index) const
{
	return m_interpolatedVertices[priv_getInterpolatedIndex(interpolationOffset, index)].position;
}

unsigned int Spline::getInterpolatedPositionCount() const
{
	return ((m_isClosed) ? static_cast<unsigned int>(m_vertices.size() * priv_getNumberOfPointsPerVertex()) : static_cast<unsigned int>((m_vertices.size() - 1u) * priv_getNumberOfPointsPerVertex() + 1u));
}

sf::Vector2f Spline::getInterpolatedPositionTangent(const unsigned int interpolationOffset, const unsigned int index) const
{
	return m_interpolatedVerticesUnitTangents[priv_getInterpolatedIndex(interpolationOffset, index)];
}

sf::Vector2f Spline::getInterpolatedPositionNormal(const unsigned int interpolationOffset, const unsigned int index) const
{
	return vectorNormal(getInterpolatedPositionTangent(interpolationOffset, index));
}

float Spline::getInterpolatedPositionThickness(const unsigned int interpolationOffset, const unsigned int index) const
{
	if (!priv_isThick())
		return 0.f;

	const unsigned int interpolatedIndex{ priv_getInterpolatedIndex(interpolationOffset, index) };

	const unsigned int vertexIndex{ interpolatedIndex / priv_getNumberOfPointsPerVertex() };
	const unsigned int interpolatedVertexIndex{ vertexIndex * priv_getNumberOfPointsPerVertex() };
	const float vertexRatio{ static_cast<float>(interpolatedIndex - interpolatedVertexIndex) / priv_getNumberOfPointsPerVertex() };
	const std::vector<Vertex>::const_iterator currentVertex{ m_vertices.begin() + (vertexIndex % m_vertices.size()) };
	std::vector<Vertex>::const_iterator nextVertex{ currentVertex };
	if (currentVertex != m_vertices.end() - 1)
		nextVertex = currentVertex + 1;
	else if (m_isClosed)
		nextVertex = m_vertices.begin();

	return m_thickness * linearInterpolation(currentVertex->thickness, nextVertex->thickness, vertexRatio);
}

float Spline::getInterpolatedPositionThicknessCorrectionScale(const unsigned int interpolationOffset, const unsigned int index) const
{
	if (!priv_isThick())
		return 0.f;

	const unsigned int interpolatedIndex{ priv_getInterpolatedIndex(interpolationOffset, index) };

	const sf::Vector2f sideOffset{ m_outputVertices[interpolatedIndex * 2].position - m_interpolatedVertices[interpolatedIndex].position };
	const float sideOffsetLength{ vectorLength(sideOffset) };

	return sideOffsetLength * 2 / getInterpolatedPositionThickness(interpolationOffset, index);
}



// PRIVATE

void Spline::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = nullptr;
	if (m_outputVertices.size() > 0)
		target.draw(m_outputVertices.data(), m_outputVertices.size(), (priv_isThick() ? thickPrimitiveType : m_primitiveType), states);
	if (m_showHandles && m_handlesVertices.size() > 1)
		target.draw(&m_handlesVertices.front(), m_handlesVertices.size(), sf::PrimitiveType::Lines, states);
}

bool Spline::priv_isValidVertexIndex(const unsigned int vertexIndex) const
{
	return vertexIndex < m_vertices.size();
}

bool Spline::priv_testVertexIndex(const unsigned int vertexIndex, const std::string& exceptionMessage) const
{
	if (!priv_isValidVertexIndex(vertexIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + exceptionMessage + " Vertex index (" + std::to_string(vertexIndex) + ") out of range");
		return false;
	}
	return true;
}

bool Spline::priv_isThick() const
{
	return (m_thickness >= thicknessEpsilon || m_thickness <= -thicknessEpsilon);
}

unsigned int Spline::priv_getNumberOfPointsPerVertex() const
{
	return m_interpolationSteps + 1u;
}

// index is control vertex index and interpolation offset is index of interpolated vertices from that control vertex
// index should be: [0, numberOfVertices)
// interpolation should be (when used with index): [0, numberOfInterpolationSteps] or [0, numberOfInterpolatedPositionsPerVertex)
// (note that higher value of range is 'not' inclusive)
// interpolation can, however, extend beyond the range into the following vertex/ices so index can be omitted (it defaults to zero).
// index and interpolationOffset values must be valid
unsigned int Spline::priv_getInterpolatedIndex(const unsigned int interpolationOffset, const unsigned int index) const
{
	const unsigned int indexOffset{ index * priv_getNumberOfPointsPerVertex() };
	const unsigned int interpolatedPositionIndex{ indexOffset + interpolationOffset };
	assert(interpolatedPositionIndex < getInterpolatedPositionCount()); // index must be in valid range
	return interpolatedPositionIndex;
}

void Spline::priv_updateOutputVertices()
{
	if (!priv_isThick())
	{
		m_outputVertices.resize(m_interpolatedVertices.size());
		for (std::vector<sf::Vertex>::iterator begin{ m_interpolatedVertices.begin() }, end{ m_interpolatedVertices.end() }, it{ begin }; it != end; ++it)
		{
			const unsigned int outputIndex{ static_cast<unsigned int>(it - begin) };
			const unsigned int index{ outputIndex / priv_getNumberOfPointsPerVertex() };
			const unsigned int interpolatedIndex{ index * priv_getNumberOfPointsPerVertex() };
			const float vertexRatio{ static_cast<float>(outputIndex - interpolatedIndex) / priv_getNumberOfPointsPerVertex() };

			m_outputVertices[outputIndex] = *it;

			const std::vector<Vertex>::iterator currentVertex{ m_vertices.begin() + (index % m_vertices.size()) };
			std::vector<Vertex>::iterator nextVertex{ currentVertex };
			if (currentVertex != m_vertices.end() - 1)
				nextVertex = currentVertex + 1;
			else if (m_isClosed)
				nextVertex = m_vertices.begin();

			sf::Vector2f tangentUnit{ *(m_interpolatedVerticesUnitTangents.begin() + (it - begin)) };
			if (m_isClosed || it != end - 1)
			{
				const sf::Vector2f forwardLine{ ((it != end - 1) ? (it + 1)->position - it->position : (begin + 1)->position - begin->position) };
				const sf::Vector2f forwardUnit{ forwardLine / vectorLength(forwardLine) };
				float dotUnits{ dot(forwardUnit , tangentUnit) };
				tangentUnit = tangentUnit / (isConsideredZero(dotUnits) ? zeroEpsilon : dotUnits);
			}
			const sf::Vector2f normalUnit{ vectorNormal(tangentUnit) };
			const float randomNormalOffsetRange{ m_randomNormalOffsetRange * linearInterpolation(currentVertex->randomNormalOffsetRange, nextVertex->randomNormalOffsetRange, vertexRatio) };
			sf::Vector2f randomOffset{ normalUnit * (isConsideredZero(randomNormalOffsetRange) ? 0.f : randomValue(randomNormalOffsetRange)) };
			const bool randomOffsetIsCentered{ true };
			if (randomOffsetIsCentered)
				randomOffset -= normalUnit * (randomNormalOffsetRange / 2.f);

			m_outputVertices[outputIndex].position += randomOffset;

			if (m_isClosed && (it == end - 1))
				m_outputVertices[outputIndex].position = m_outputVertices[0].position;
		}
	}
	else
	{
		unsigned int numberOfExtraVerticesForCaps{ 0u };
		if (!m_isClosed)
		{
			if (m_thickStartCapType == ThickCapType::Round)
				numberOfExtraVerticesForCaps += (m_roundedThickStartCapInterpolationLevel + 1u) * 2u;
			if (m_thickEndCapType == ThickCapType::Round)
				numberOfExtraVerticesForCaps += (m_roundedThickEndCapInterpolationLevel + 1u) * 2u;
		}
		switch (m_thickCornerType)
		{
		case ThickCornerType::Point:
			// ends and corners require 2 each.
			m_outputVertices.resize(m_interpolatedVertices.size() * 2u + numberOfExtraVerticesForCaps);
			break;
		case ThickCornerType::PointLimit:
			// ends require 2 each. corners require 6 each.
			m_outputVertices.resize(m_interpolatedVertices.size() * 6u - 8u + numberOfExtraVerticesForCaps);
			break;
		case ThickCornerType::PointClip:
			// ends require 2 each. corners require 6 each.
			m_outputVertices.resize(m_interpolatedVertices.size() * 6u - 8u + numberOfExtraVerticesForCaps);
			break;
		case ThickCornerType::Bevel:
			// ends require 2 each. corners require 4 each.
			m_outputVertices.resize((m_interpolatedVertices.size() - 1u) * 4u + numberOfExtraVerticesForCaps);
			break;
		case ThickCornerType::Round:
			// ends require 2 each. corners require (2 + interpolation) * 2 each.
			m_outputVertices.resize(((m_interpolatedVertices.size() - 2u) * (m_roundedThickCornerInterpolationLevel + 2u) + 2) * 2u + numberOfExtraVerticesForCaps);
			break;
		}

		std::vector<sf::Vertex>::iterator itThick{ m_outputVertices.begin() };
		if (!m_isClosed && (m_thickStartCapType == ThickCapType::Round))
		{
			std::vector<Vertex>::iterator vertex{ m_vertices.begin() };
			const float thickness{ m_thickness * vertex->thickness };
			const float halfWidth{ thickness / 2.f };
			const sf::Color color{ m_color * vertex->color };
			const sf::Vector2f tangentUnit{ *(m_interpolatedVerticesUnitTangents.begin()) };
			const sf::Vector2f normalUnit{ -vectorNormal(tangentUnit) };
			const float normalAngle{ std::atan2(normalUnit.y, normalUnit.x) };
			for (unsigned int i{ 0u }; i <= m_roundedThickStartCapInterpolationLevel; ++i)
			{
				const float ratio{ static_cast<float>(i) / (m_roundedThickStartCapInterpolationLevel + 1u) };
				const float angleOffset{ ratio * pi };
				const float angle{ normalAngle + angleOffset };
				const sf::Vector2f vector{ std::cos(angle) * halfWidth, std::sin(angle) * halfWidth };
				itThick->color = color;
				itThick++->position = vertex->position + vector;
				itThick->color = color;
				itThick++->position = vertex->position;
			}
		}
		for (std::vector<sf::Vertex>::const_iterator begin{ m_interpolatedVertices.begin() }, end{ m_interpolatedVertices.end() }, it{ begin }; it != end; ++it)
		{
			const unsigned int outputIndex{ static_cast<unsigned int>(it - begin) };
			const unsigned int index{ outputIndex / priv_getNumberOfPointsPerVertex() };
			const unsigned int interpolatedIndex{ index * priv_getNumberOfPointsPerVertex() };
			const float vertexRatio{ static_cast<float>(outputIndex - interpolatedIndex) / priv_getNumberOfPointsPerVertex() };
			const std::vector<Vertex>::iterator currentVertex{ m_vertices.begin() + (index % m_vertices.size()) };
			std::vector<Vertex>::iterator nextVertex{ currentVertex };
			if (currentVertex != m_vertices.end() - 1)
				nextVertex = currentVertex + 1;
			else if (m_isClosed)
				nextVertex = m_vertices.begin();
			const float thickness{ m_thickness * linearInterpolation(currentVertex->thickness, nextVertex->thickness, vertexRatio) };
			const float halfWidth{ thickness / 2.f };

			const sf::Color color{ m_color * linearInterpolation(currentVertex->color, nextVertex->color, vertexRatio) };

			sf::Vector2f tangentUnit{ *(m_interpolatedVerticesUnitTangents.begin() + (it - begin)) };
			sf::Vector2f pointTransformedTangentUnit{ tangentUnit };
			float dotUnits{ 1.f };
			bool pointIsTooLong{ false };
			if (m_isClosed || it != end - 1)
			{
				const sf::Vector2f forwardLine{ ((it != end - 1) ? (it + 1)->position - it->position : (begin + 1)->position - begin->position) };
				const sf::Vector2f forwardUnit{ forwardLine / vectorLength(forwardLine) };
				dotUnits = dot(forwardUnit, pointTransformedTangentUnit);
				pointTransformedTangentUnit = pointTransformedTangentUnit / (isConsideredZero(dotUnits) ? zeroEpsilon : dotUnits);
				pointIsTooLong = (vectorLength(pointTransformedTangentUnit) * halfWidth) > m_maxPointLength;
			}
			const sf::Vector2f normalUnit{ vectorNormal(tangentUnit) };
			const sf::Vector2f pointTransformedNormalUnit{ vectorNormal(pointTransformedTangentUnit) };
			const sf::Vector2f scaledNormal{ normalUnit * halfWidth };
			const sf::Vector2f scaledPointTransformedNormal{ pointTransformedNormalUnit * halfWidth };
			const float randomNormalOffsetRange{ m_randomNormalOffsetRange * linearInterpolation(currentVertex->randomNormalOffsetRange, nextVertex->randomNormalOffsetRange, vertexRatio) };
			sf::Vector2f randomOffset{ normalUnit * (isConsideredZero(randomNormalOffsetRange) ? 0.f : randomValue(randomNormalOffsetRange)) };
			const bool randomOffsetIsCentered{ true };
			if (randomOffsetIsCentered)
				randomOffset -= normalUnit * (randomNormalOffsetRange / 2.f);
			if (it == begin || it == end - 1)
				randomOffset = { 0.f, 0.f };

			sf::Vector2f capOffset{ 0.f, 0.f };
			if (!m_isClosed)
			{
				if ((it == begin) && (m_thickStartCapType == ThickCapType::Extended))
					capOffset = -tangentUnit * halfWidth;
				else if ((it == end - 1u) && (m_thickEndCapType == ThickCapType::Extended))
					capOffset = tangentUnit * halfWidth;
			}

			switch (m_thickCornerType)
			{
			case ThickCornerType::Point:
				itThick->color = color;
				itThick++->position = it->position + scaledPointTransformedNormal + capOffset + randomOffset;
				itThick->color = color;
				itThick++->position = it->position - scaledPointTransformedNormal + capOffset + randomOffset;
				break;
			case ThickCornerType::PointClip:
			case ThickCornerType::PointLimit:
			case ThickCornerType::Bevel:
			{
				const sf::Vector2f forwardLine{ ((it != end - 1) ? (it + 1)->position - it->position : begin->position - it->position) };
				const sf::Vector2f forwardUnit{ forwardLine / vectorLength(forwardLine) };
				const sf::Vector2f backwardLine{ ((it != begin) ? it->position - (it - 1)->position : it->position - (end - 1)->position) };
				const sf::Vector2f backwardUnit{ backwardLine / vectorLength(backwardLine) };

				const sf::Vector2f forwardNormalUnit{ vectorNormal(forwardUnit) };
				const sf::Vector2f backwardNormalUnit{ vectorNormal(backwardUnit) };
				const sf::Vector2f forwardNormal{ forwardNormalUnit * halfWidth };
				const sf::Vector2f backwardNormal{ backwardNormalUnit * halfWidth };

				if (it == begin)
				{
					itThick->color = color;
					itThick++->position = it->position + forwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - forwardNormal + capOffset + randomOffset;
				}
				else if (it == end - 1)
				{
					itThick->color = color;
					itThick++->position = it->position + backwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - backwardNormal + capOffset + randomOffset;
				}
				else
				{
					itThick->color = color;
					itThick++->position = it->position + backwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - backwardNormal + capOffset + randomOffset;

					if (m_thickCornerType != ThickCornerType::Bevel)
					{
						sf::Vector2f normalToUse{ scaledPointTransformedNormal };
						if (pointIsTooLong)
						{
							switch (m_thickCornerType)
							{
							case ThickCornerType::PointClip:
								normalToUse = scaledNormal * dotUnits;
								break;
							case ThickCornerType::PointLimit:
								normalToUse = normalUnit * m_maxPointLength;
								break;
							}
						}
						itThick->color = color;
						itThick++->position = it->position + normalToUse + capOffset + randomOffset;
						itThick->color = color;
						itThick++->position = it->position - normalToUse + capOffset + randomOffset;
					}

					itThick->color = color;
					itThick++->position = it->position + forwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - forwardNormal + capOffset + randomOffset;
				}
			}
				break;
			case ThickCornerType::Round:
			{
				const sf::Vector2f forwardLine{ ((it != end - 1) ? (it + 1)->position - it->position : begin->position - it->position) };
				const sf::Vector2f forwardUnit{ forwardLine / vectorLength(forwardLine) };
				const sf::Vector2f backwardLine{ ((it != begin) ? it->position - (it - 1)->position : it->position - (end - 1)->position) };
				const sf::Vector2f backwardUnit{ backwardLine / vectorLength(backwardLine) };

				if (it == begin)
				{
					const sf::Vector2f forwardNormalUnit{ vectorNormal(forwardUnit) };
					const sf::Vector2f forwardNormal{ forwardNormalUnit * halfWidth };
					itThick->color = color;
					itThick++->position = it->position + forwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - forwardNormal + capOffset + randomOffset;
				}
				else if (it == end - 1)
				{
					const sf::Vector2f backwardNormalUnit{ vectorNormal(backwardUnit) };
					const sf::Vector2f backwardNormal{ backwardNormalUnit * halfWidth };
					itThick->color = color;
					itThick++->position = it->position + backwardNormal + capOffset + randomOffset;
					itThick->color = color;
					itThick++->position = it->position - backwardNormal + capOffset + randomOffset;
				}
				else
				{
					const float backwardAngle{ std::atan2(backwardUnit.y, backwardUnit.x) };
					const float forwardAngle{ std::atan2(forwardUnit.y, forwardUnit.x) };
					for (unsigned int i{ 0u }; i <= (m_roundedThickCornerInterpolationLevel + 1u); ++i)
					{
						const float ratio{ static_cast<float>(i) / (m_roundedThickCornerInterpolationLevel + 1u) };
						const float angle{ linearInterpolation(backwardAngle, forwardAngle, ratio) };
						const sf::Vector2f unit{ std::cos(angle), std::sin(angle) };
						const sf::Vector2f thisNormalUnit{ vectorNormal(unit) };
						const sf::Vector2f thisNormal{ thisNormalUnit * halfWidth };
						itThick->color = color;
						itThick++->position = it->position + thisNormal + capOffset + randomOffset;
						itThick->color = color;
						itThick++->position = it->position - thisNormal + capOffset + randomOffset;
					}
				}
			}
				break;
			}

			///*
			if (m_isClosed && (it == end - 1))
			{
				(itThick - 2)->position = m_outputVertices.begin()->position;
				(itThick - 1)->position = (m_outputVertices.begin() + 1)->position;
			}
			//*/
		}
		if (!m_isClosed && (m_thickEndCapType == ThickCapType::Round))
		{
			std::vector<Vertex>::iterator vertex{ m_vertices.end() - 1u };
			const float thickness{ m_thickness * vertex->thickness };
			const float halfWidth{ thickness / 2.f };
			const sf::Color color{ m_color * vertex->color };
			const sf::Vector2f tangentUnit{ *(m_interpolatedVerticesUnitTangents.end() - 1u) };
			const sf::Vector2f normalUnit{ -vectorNormal(tangentUnit) };
			const float normalAngle{ std::atan2(normalUnit.y, normalUnit.x) };
			for (unsigned int i{ 0u }; i <= m_roundedThickEndCapInterpolationLevel; ++i)
			{
				const float ratio{ static_cast<float>(m_roundedThickEndCapInterpolationLevel - i) / (m_roundedThickEndCapInterpolationLevel + 1u) };
				const float angleOffset{ ratio * pi };
				const float angle{ normalAngle + angleOffset };
				const sf::Vector2f vector{ std::cos(angle) * halfWidth, std::sin(angle) * halfWidth };
				itThick->color = color;
				itThick++->position = vertex->position;
				itThick->color = color;
				itThick++->position = vertex->position - vector;
			}
		}
	}
}

} // namespace selbaward
