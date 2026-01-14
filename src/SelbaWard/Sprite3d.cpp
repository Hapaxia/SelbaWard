//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Sprite3d
//
// Copyright(c) 2015-2026 M.J.Silk
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



#include "Sprite3d.hpp"

#include <cmath>

namespace
{

inline float linearInterpolation(const float from, const float to, const float alpha)
{
	return from * (1.f - alpha) + to * alpha;
}

inline float mod(const float numerator, float denominator)
{
	// avoid division by zero (if more accuracy is required, only offset the divided denominator, still use the actual denominator to multiply back as zero multiplication is fine)
	if (denominator > -0.000001f && denominator < 0.000001f)
		denominator = 0.000001f;

	return numerator - (std::trunc(numerator / denominator) * denominator);
}

inline float min(const float a, const float b)
{
	return (a < b) ? a : b;
}

inline float max(const float a, const float b)
{
	return (a > b) ? a : b;
}

template <class T>
inline T abs(const T value)
{
	return value < 0 ? -value : value;
}

inline sf::Vector2i abs(const sf::Vector2i& vector)
{
	return{ abs(vector.x), abs(vector.y) };
}

} // namespace

namespace selbaward
{

Sprite3d::Sprite3d()
	: m_depthToShallownessConversionNumerator{ 10000.f }
	, m_pitch{ 0.f }
	, m_yaw{ 0.f }
	, m_depth{ 10.f }
	, m_shallowness{ m_depthToShallownessConversionNumerator / m_depth }
	, m_meshDensity{ 0u }
	, m_flipBack{ false }
	, m_pTexture{ nullptr }
	, m_pBackTexture{ nullptr }
	, m_size{}
	, m_textureOffset{}
	, m_backTextureOffset{}
	, m_useDynamicSubdivision{ false }
	, m_minSubdivision{ 1u }
	, m_maxSubdivision{ 4u }
	, m_subdivision{ 0u }
	, m_subdividedMeshDensity{ 0u }
	, m_points(4u)
	, m_transformedPoints(4u)
	, m_origin{}
	, m_vertices(4u)
	, m_compactTransformMatrix(8u, 0.f) // was 5. is now 8 to take into account z (for origin's z)
	, m_isBackFacing{ false }
	, m_topLeft{}
	, m_topRight{}
	, m_bottomLeft{}
	, m_bottomRight{}
{
}

Sprite3d::Sprite3d(const sf::Texture& texture)
	: Sprite3d()
{
	setTexture(texture);
}

Sprite3d::Sprite3d(const sf::Texture& texture, const sf::IntRect& textureRect)
	: Sprite3d()
{
	setTexture(texture);
	setTextureRect(textureRect);
}

Sprite3d::Sprite3d(const sf::Texture& texture, const sf::Texture& backTexture)
	: Sprite3d()
{
	setTexture(texture);
	setBackTexture(backTexture);
}

Sprite3d::Sprite3d(const sf::Texture& texture, const sf::IntRect& textureRect, const sf::Texture& backTexture, const sf::Vector2i backTextureOffset)
	: Sprite3d()
{
	setTexture(texture);
	setTextureRect(textureRect);
	setBackTexture(backTexture);
	setBackTextureOffset(backTextureOffset);
}

Sprite3d::Sprite3d(const sf::Sprite& sprite)
	: Sprite3d()
{
	setTexture(sprite.getTexture());
	setTextureRect(sprite.getTextureRect());
	this->setColor(sprite.getColor());
	this->setOrigin(sprite.getOrigin());
	this->setPosition(sprite.getPosition());
	this->setRotation(sprite.getRotation().asDegrees());
	this->setScale(sprite.getScale());
}

const sf::Sprite Sprite3d::getSprite() const
{
	assert(m_pTexture != nullptr); // texture must have been assigned to Sprite 3D before getting an sf::Sprite is possible.

	sf::IntRect textureRect(m_textureOffset, m_size);
	sf::Sprite sprite(*m_pTexture, textureRect);
	sprite.setColor(this->getColor());
	sprite.setOrigin(this->getOrigin());
	sprite.setPosition(this->getPosition());
	sprite.setRotation(this->getRotation());
	sprite.setScale(this->getScale());
	return sprite;
}

void Sprite3d::setTextureRect(const sf::IntRect& textureRectangle)
{
	m_textureOffset = textureRectangle.position;
	m_backTextureOffset = m_textureOffset;
	m_size = textureRectangle.size;
	createPointGrid();
	updateTransformedPoints();
	updateVertices();
	updateGlobalCorners();
}

void Sprite3d::setTexture(const sf::Texture& texture, const bool resetRect, const bool resetBackOffset)
{
	if (m_pTexture == nullptr || resetRect)
	{
		m_textureOffset = { 0, 0 };
		m_size = sf::Vector2i(texture.getSize());
		createPointGrid();
		m_vertices.resize(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
	}
	if (resetBackOffset)
		m_backTextureOffset = { 0, 0 };
	m_pTexture = &texture;
}

void Sprite3d::setTexture()
{
	m_pTexture = nullptr;
}

void Sprite3d::setBackTexture(const sf::Texture& texture, const bool resetOffset)
{
	m_pBackTexture = &texture;
	if (m_pBackTexture == nullptr || resetOffset)
		m_backTextureOffset = { 0, 0 };
}

void Sprite3d::setBackTexture()
{
	m_pBackTexture = nullptr;
}

void Sprite3d::setFlipBack(const bool flipBack)
{
	m_flipBack = flipBack;
}

const sf::Texture* Sprite3d::getTexture() const
{
	return m_pTexture;
}

sf::IntRect Sprite3d::getTextureRect() const
{
	return{ m_textureOffset, m_size };
}

const sf::Texture* Sprite3d::getBackTexture() const
{
	return m_pBackTexture;
}

bool Sprite3d::getFlipBack() const
{
	return m_flipBack;
}

sf::Vector2i Sprite3d::getTextureOffset() const
{
	return m_textureOffset;
}

void Sprite3d::setTextureOffset(const sf::Vector2i textureOffset)
{
	m_textureOffset = textureOffset;
}

sf::Vector2i Sprite3d::getBackTextureOffset() const
{
	return m_backTextureOffset;
}

void Sprite3d::setBackTextureOffset(const sf::Vector2i backTextureOffset)
{
	m_backTextureOffset = backTextureOffset;
}

void Sprite3d::setColor(const sf::Color& color)
{
	for (auto& vertex : m_vertices)
		vertex.color = color;
}

sf::Color Sprite3d::getColor() const
{
	return m_vertices[0].color;
}

float Sprite3d::getPitch() const
{
	return m_pitch;
}

float Sprite3d::getYaw() const
{
	return m_yaw;
}

float Sprite3d::getRoll() const
{
	return this->getRotation().asDegrees();
}

sf::Vector3f Sprite3d::getRotation3d() const
{
	return{ m_pitch, m_yaw, this->Transformable::getRotation().asDegrees()};
}

sf::Vector3f Sprite3d::getOrigin3d() const
{
	const sf::Vector2f origin2d{ this->Transformable::getOrigin() };
	return{ origin2d.x, origin2d.y, m_origin.z };
}

void Sprite3d::setPitch(const float pitch)
{
	m_pitch = pitch;
	while (m_pitch > 180.f)
		m_pitch -= 360.f;
	while (m_pitch < -180.f)
		m_pitch += 360.f;
}

void Sprite3d::setYaw(const float yaw)
{
	m_yaw = yaw;
	while (m_yaw > 180.f)
		m_yaw -= 360.f;
	while (m_yaw < -180.f)
		m_yaw += 360.f;
}

void Sprite3d::setRoll(const float roll)
{
	this->Transformable::setRotation(sf::degrees(roll));
}

void Sprite3d::setRotation(const float rotation)
{
	setRoll(rotation);
}

void Sprite3d::setRotation(const sf::Vector3f rotation)
{
	setRotation3d(rotation);
}

void Sprite3d::setRotation3d(const sf::Vector3f rotation)
{
	setPitch(rotation.x);
	setYaw(rotation.y);
	setRoll(rotation.z);
}

void Sprite3d::setOriginZ(const float originZ)
{
	m_origin.z = originZ;
}

void Sprite3d::setOrigin(const sf::Vector2f origin)
{
	this->Transformable::setOrigin(origin);
	m_origin.z = 0.f; // reset origin's z back to zero when setting a 2D origin
}

void Sprite3d::setOrigin(const sf::Vector3f origin)
{
	setOrigin3d(origin);
}

void Sprite3d::setOrigin3d(const sf::Vector3f origin)
{
	setOrigin({ origin.x, origin.y });
	setOriginZ(origin.z);
}

float Sprite3d::getMostExtremeAngle() const
{
	float pitch{ abs(m_pitch) };
	if (pitch > 90.f)
		pitch = 180.f - pitch;
	float yaw{ abs(m_yaw) };
	if (yaw > 90.f)
		yaw = 180.f - yaw;
	return max(pitch, yaw);
}

void Sprite3d::setMeshDensity(const std::size_t meshDensity)
{
	m_meshDensity = meshDensity;

	setSubdivision(m_subdivision);
}

std::size_t Sprite3d::getMeshDensity() const
{
	return m_meshDensity;
}

std::size_t Sprite3d::getSubdividedMeshDensity() const
{
	return m_subdividedMeshDensity;
}

void Sprite3d::reserveMeshDensity(const std::size_t meshDensity)
{
	const std::size_t numberOfPointsPerDimension{ meshDensity + 2u };
	m_points.reserve(numberOfPointsPerDimension * numberOfPointsPerDimension);
	m_transformedPoints.reserve(m_points.size());

	const std::size_t currentSubdividedMeshDensity{ m_subdividedMeshDensity };
	m_subdividedMeshDensity = meshDensity;
	m_vertices.reserve(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
	m_subdividedMeshDensity = currentSubdividedMeshDensity;
}

void Sprite3d::setDynamicSubdivision(const bool dynamicSubdivision)
{
	m_useDynamicSubdivision = dynamicSubdivision;
}

void Sprite3d::setDynamicSubdivisionRange(std::size_t maximum, std::size_t minimum)
{
	if (maximum < minimum)
	{
		const std::size_t temp{ maximum };
		maximum = minimum;
		minimum = temp;
	}
	m_maxSubdivision = maximum;
	m_minSubdivision = minimum;
	reserveMeshDensity(m_maxSubdivision);
}

bool Sprite3d::getDynamicSubdivision() const
{
	return m_useDynamicSubdivision;
}

void Sprite3d::setSubdivision(const std::size_t subdivision) const
{
	m_subdivision = subdivision;

	m_subdividedMeshDensity = m_meshDensity;

	for (std::size_t i{ 0u }; i < m_subdivision; ++i)
		m_subdividedMeshDensity = (m_subdividedMeshDensity * 2u) + 1u;

	createPointGrid();
	m_vertices.resize(getNumberOfVerticesNeededForCurrentSubdividedMeshDensity());
}

std::size_t Sprite3d::getSubdivision() const
{
	return m_subdivision;
}

void Sprite3d::setNumberOfPoints(const std::size_t numberOfPoints)
{
	const std::size_t root{ static_cast<std::size_t>(std::sqrt(numberOfPoints)) };
	if (root > 2u)
		setMeshDensity(root - 2u);
	else
		setMeshDensity(0u);
}

void Sprite3d::setNumberOfQuads(const std::size_t numberOfQuads)
{
	const std::size_t root{ static_cast<std::size_t>(std::sqrt(numberOfQuads)) };
	if (root > 1u)
		setMeshDensity(root - 1u);
	else
		setMeshDensity(0u);
}

void Sprite3d::minimalMesh()
{
	m_meshDensity = 0u;
	setSubdivision(0u);
}

sf::FloatRect Sprite3d::getLocalBounds() const
{
	return{ { 0.f, 0.f }, sf::Vector2f(abs(m_size)) };
}

sf::FloatRect Sprite3d::getGlobalBounds() const
{
	updateTransformedPoints();
	updateGlobalCorners();

	const float minX{ min(m_topLeft.x, min(m_topRight.x, min(m_bottomLeft.x, m_bottomRight.x))) };
	const float maxX{ max(m_topLeft.x, max(m_topRight.x, max(m_bottomLeft.x, m_bottomRight.x))) };
	const float minY{ min(m_topLeft.y, min(m_topRight.y, min(m_bottomLeft.y, m_bottomRight.y))) };
	const float maxY{ max(m_topLeft.y, max(m_topRight.y, max(m_bottomLeft.y, m_bottomRight.y))) };

	return{ { minX, minY }, { maxX - minX, maxY - minY } };
}

void Sprite3d::setDepth(const float depth)
{
	m_depth = depth;
	m_shallowness = m_depthToShallownessConversionNumerator / ((m_depth > -0.000001f && m_depth < 0.000001f) ? 0.000001f : m_depth); // avoid division by zero here but don't change m_depth from being zero
}

float Sprite3d::getDepth() const
{
	return m_depth;
}

void Sprite3d::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	updateTransformedPoints();
	updateVertices();
	states.transform *= getTransform();

	if (m_isBackFacing && (m_pBackTexture != nullptr))
		states.texture = m_pBackTexture;
	else
		states.texture = m_pTexture;

	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::TriangleStrip, states);
}

void Sprite3d::updateTransformedPoints() const
{
	if (m_useDynamicSubdivision)
		setSubdivision(static_cast<unsigned int>((m_maxSubdivision - m_minSubdivision) * getMostExtremeAngle() / 90.f + m_minSubdivision));

	m_origin = { this->getOrigin().x, this->getOrigin().y, m_origin.z };
	const float radiansFromDegreesMultiplier{ 0.0174532925f }; // pi / 180;
	const float pitchInRadians{ m_pitch * radiansFromDegreesMultiplier };
	const float yawInRadians{ m_yaw * radiansFromDegreesMultiplier };

	const float cosPitch{ std::cos(pitchInRadians) };
	const float sinPitch{ std::sin(pitchInRadians) };
	const float cosYaw{ std::cos(yawInRadians) };
	const float sinYaw{ std::sin(yawInRadians) };

	/*******************************************************
	*          Pitch and Yaw combined matrix               *
	*                                                      *
	*  cosYaw,  sinPitch * sinYaw, -cosPitch * sinYaw, 0,  *
	*  0,       cosPitch,           sinPitch,          0,  *
	*  sinYaw, -sinPitch * cosYaw,  cosPitch * cosYaw, 0,  *
	*  0,       0,                  0,                 1   *
	*******************************************************/

	//m_compactTransformMatrix = { cosYaw, sinYaw, sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw }; // only the five used elements
	m_compactTransformMatrix = { cosYaw, sinYaw, sinPitch * sinYaw, cosPitch, -sinPitch * cosYaw, -cosPitch * sinYaw, sinPitch, cosPitch * cosYaw }; // all eight used elements

	for (std::size_t v{ 0u }; v < m_points.size(); ++v)
	{
		sf::Vector3f point{ m_points[v] };

		point -= m_origin;
		point =
		{
			/*
			m_compactTransformMatrix[0u] * point.x + m_compactTransformMatrix[2u] * point.y,
			                                         m_compactTransformMatrix[3u] * point.y,
			m_compactTransformMatrix[1u] * point.x + m_compactTransformMatrix[4u] * point.y
			*/
			m_compactTransformMatrix[0u] * point.x + m_compactTransformMatrix[2u] * point.y + m_compactTransformMatrix[5u] * point.z,
			                                         m_compactTransformMatrix[3u] * point.y + m_compactTransformMatrix[6u] * point.z,
			m_compactTransformMatrix[1u] * point.x + m_compactTransformMatrix[4u] * point.y + m_compactTransformMatrix[7u] * point.z
		}; // apply rotations
		point *= m_shallowness / (m_shallowness + point.z); // apply depth
		point += m_origin;

		m_transformedPoints[v] = { point.x, point.y };
	}

	updateGlobalCorners();

	m_isBackFacing = false;
	if (m_pitch < -90.f || m_pitch > 90.f)
		m_isBackFacing = true;
	if (m_yaw < -90.f || m_yaw > 90.f)
		m_isBackFacing = !m_isBackFacing;
}

void Sprite3d::updateVertices() const
{
	sf::Vector2i currentTextureOffset{ m_textureOffset };
	if (m_isBackFacing)
		currentTextureOffset = m_backTextureOffset;

	// create a mesh (triangle strip) from the points
	for (std::size_t v{ 0u }; v < m_vertices.size(); ++v)
	{
		const std::size_t pointIndex{ getPointIndexForVertexIndex(v) };
		const std::size_t texturePointIndex{ getPointIndexForVertexIndex(v, m_isBackFacing && m_flipBack) };

		// update vertex
		m_vertices[v].position = m_transformedPoints[pointIndex];
		m_vertices[v].texCoords.x = (m_points[texturePointIndex].x * (m_size.x < 0 ? -1 : 1)) + currentTextureOffset.x;
		m_vertices[v].texCoords.y = (m_points[texturePointIndex].y * (m_size.y < 0 ? -1 : 1)) + currentTextureOffset.y;
	}
}

void Sprite3d::updateGlobalCorners() const
{
	m_topLeft = getTransform().transformPoint(m_transformedPoints.front());
	m_topRight = getTransform().transformPoint(*(m_transformedPoints.begin() + m_subdividedMeshDensity + 1u));
	m_bottomLeft = getTransform().transformPoint(*(m_transformedPoints.end() - m_subdividedMeshDensity - 2u)); // end() - (m_subdividedMeshDensity + 1u) - 1u
	m_bottomRight = getTransform().transformPoint(m_transformedPoints.back());

}

void Sprite3d::createPointGrid() const
{
	sf::Vector2f leftTop{ 0.f, 0.f };
	sf::Vector2f rightBottom(abs(m_size));

	const std::size_t numberOfPointsPerDimension{ m_subdividedMeshDensity + 2u };

	// create a grid of points
	m_points.resize(numberOfPointsPerDimension * numberOfPointsPerDimension);
	for (std::size_t y{ 0u }; y < numberOfPointsPerDimension; ++y)
	{
		for (std::size_t x{ 0u }; x < numberOfPointsPerDimension; ++x)
		{
			m_points[y * numberOfPointsPerDimension + x].x = linearInterpolation(leftTop.x, rightBottom.x, static_cast<float>(x) / (numberOfPointsPerDimension - 1u));
			m_points[y * numberOfPointsPerDimension + x].y = linearInterpolation(leftTop.y, rightBottom.y, static_cast<float>(y) / (numberOfPointsPerDimension - 1u));
			m_points[y * numberOfPointsPerDimension + x].z = 0.f;
		}
	}

	m_transformedPoints.resize(m_points.size());
}

std::size_t Sprite3d::getPointIndexForVertexIndex(const std::size_t vertexIndex, const bool invertPointX) const
{
	const std::size_t numberOfPointsPerDimension{ m_subdividedMeshDensity + 2u };
	const std::size_t numberOfVerticesPerRow{ (numberOfPointsPerDimension * 2u) - 1u };

	const bool isOddRow{ ((vertexIndex / numberOfVerticesPerRow) % 2u) == 1u };
	std::size_t pointX{ (vertexIndex % numberOfVerticesPerRow) / 2u };
	if (isOddRow)
		pointX = numberOfPointsPerDimension - pointX - 1u;
	if (invertPointX)
		pointX = numberOfPointsPerDimension - pointX - 1u;
	std::size_t pointY{ (vertexIndex / numberOfVerticesPerRow) + ((vertexIndex % numberOfVerticesPerRow) % 2u) };

	return pointY * numberOfPointsPerDimension + pointX;
}

std::size_t Sprite3d::getNumberOfVerticesNeededForCurrentSubdividedMeshDensity() const
{
	//const std::size_t numberOfPointsPerDimension = m_meshDensity + 2u;
	//const std::size_t numberOfVerticesPerRow = numberOfPointsPerDimension * 2u - 1u;
	//return numberOfVerticesPerRow * (numberOfPointsPerDimension - 1u) + 1u;
	/*
	= v * (p - 1) + 1

	v = p * 2 - 1

	= (p * 2 - 1) * (p - 1) + 1

	= (2p - 1)(p - 1) + 1

	p = m + 2

	= (2(m + 2) - 1)(m + 2 - 1) + 1

	= (2m + 4 - 1)(m + 1) + 1

	= (2m + 3)(m + 1) + 1

	= (2m² + 3m + 2m + 3) + 1

	= 2m² + 5m + 4

	= m(2m + 5) + 4
	*/
	return (((m_subdividedMeshDensity * 2u) + 5u) * m_subdividedMeshDensity) + 4u;
}

} // namespace selbaward
