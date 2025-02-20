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

#ifndef SELBAWARD_STARFIELD3D_HPP
#define SELBAWARD_STARFIELD3D_HPP

#include "Common.hpp"
#include <SFML/System/Vector3.hpp>
#include <SFML/Graphics/VertexArray.hpp>

namespace selbaward
{

// SW Starfield3d v1.0.3
class Starfield3d : public sf::Drawable, public sf::Transformable
{
public:
	Starfield3d(sf::Vector2f size = { 0.f, 0.f }, std::size_t numberOfStars = 400u, float maxDepth = 1000000.f, const sf::Color& frontColor = sf::Color(255u, 255u, 255u, 255u), const sf::Color& backColor = sf::Color(0u, 0u, 0u, 255u), float frontScale = 1.f, float backScale = 0.f);
	void regenerate();
	void regenerate(sf::Vector2f size);
	void regenerate(sf::Vector2f size, std::size_t numberOfStars);
	void regenerate(std::size_t numberOfStars);

	void setMaxDepth(float maxDepth);
	void setFrontColor(const sf::Color& color);
	void setBackColor(const sf::Color& color);
	void setFrontScale(float frontScale);
	void setBackScale(float backScale);

	void setStarTemplate(const std::vector<sf::Vertex>& vertices);
	void setStarTemplate(const std::vector<sf::Vector2f>& vertices);
	void setStarTemplate(const sf::VertexArray& vertexArray); // ignores primitive type - uses given vertices with sf::Triangles primitive type

	void move(sf::Vector3f movement);
	void pan(sf::Vector2f panAmount);

private:
	const float m_depthCalibration;
	const float m_depthSpeedCalibration;
	bool m_isUpdateRequired;
	sf::Vector2f m_size;
	std::size_t m_numberOfStars;
	std::vector<sf::Vector3f> m_positions;
	float m_maxDepth;
	sf::Color m_frontColor;
	sf::Color m_backColor;
	float m_frontScale;
	float m_backScale;

	std::vector<sf::Vertex> m_starTemplate;

	sf::PrimitiveType m_primitiveType;
	mutable std::vector<sf::Vertex> m_vertices;
	mutable std::vector<std::size_t> m_positionIndices;
	mutable sf::FloatRect m_deepestSliceBounds;

	enum class EdgeLock
	{
		Left,
		Right,
		Top,
		Bottom,
		Front,
		Back,
	};

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

	void priv_updateVertices() const;
	sf::Vector2f priv_projectPoint(sf::Vector3f point) const;
	sf::FloatRect priv_calculateFrustumSliceBounds(float z) const;
	sf::Vector3f priv_generateRandomStarPosition() const;
	sf::Vector3f priv_generateRandomStarPosition(EdgeLock edgeLock) const;

};

} // namespace selbaward
#endif // SELBAWARD_STARFIELD3D_HPP
