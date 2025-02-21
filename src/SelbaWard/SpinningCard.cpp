//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// SpinningCard
//
// Copyright(c) 2014-2025 M.J.Silk
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

#include "SpinningCard.hpp"

#include <SFML/Graphics/Texture.hpp>

#include <cmath>

namespace
{

constexpr float pi{ 3.14159f };

} // namespace
namespace selbaward
{

SpinningCard::SpinningCard(const sf::Sprite& sprite)
	: m_vertices(sf::PrimitiveType::TriangleFan, 6u)
	, m_pTexture{ nullptr }
	, m_initial{}
	, m_depth{ 0.2f }
{
	// copy sprite's origin, position, rotation and scale in an attempt to look like the sprite
	setOrigin(sprite.getOrigin());
	setPosition(sprite.getPosition());
	setRotation(sprite.getRotation());
	setScale(sprite.getScale());

	// copy sprite's vertices and texture

	// save initial state/starting point
	m_initial = sprite.getLocalBounds();

	// start off by setting all vertices to top-left...
	for (std::size_t i{ 0u }; i < 5u; ++i)
		m_vertices[i].position = sprite.getPosition() - sprite.getOrigin(); // top-left

	// ...then, add offsets to individual vertices
	m_vertices[0u].position += m_initial.size / 2.f; // centre
	m_vertices[2u].position.x += m_initial.size.x; // top-right
	m_vertices[3u].position += m_initial.size; // bottom-right
	m_vertices[4u].position.y += m_initial.size.y; // bottom-left

	// set texture coordinates to the same as the sprite's (using the same method for texCoords as position above)
	for (std::size_t i{ 0u }; i < 5u; ++i)
		m_vertices[i].texCoords = static_cast<sf::Vector2f>(sprite.getTextureRect().position); // top-left
	m_vertices[0u].texCoords += static_cast<sf::Vector2f>(sprite.getTextureRect().size) / 2.f; // centre
	m_vertices[2u].texCoords.x += static_cast<float>(sprite.getTextureRect().size.x); // top-right
	m_vertices[3u].texCoords += static_cast<sf::Vector2f>(sprite.getTextureRect().size); // bottom-right
	m_vertices[4u].texCoords.y += static_cast<float>(sprite.getTextureRect().size.y); // bottom-left

	// set colours of vertices to match sprite's colour
	for (std::size_t i{ 0u }; i < 5u; ++i)
		m_vertices[i].color = sprite.getColor();

	// weld last vertex to second vertex to complete the solid (matches its position, texture co-ordinate, and colour)
	m_vertices[5u] = m_vertices[1u];

	// store a pointer to the sprite's texture
	m_pTexture = &sprite.getTexture();
}

void SpinningCard::spin(const sf::Angle angle)
{
	const float angleInRadians{ angle.asRadians() - (pi / 2.f) };
	m_vertices[0u].position = sf::Vector2f(m_initial.position.x + m_initial.size.x / 2.f, m_initial.position.y + m_initial.size.y / 2.f);
	m_vertices[1u].position = sf::Vector2f(m_initial.position.x + (std::sin(angleInRadians) + 1.f) * m_initial.size.x / 2.f, m_initial.position.y - std::cos(angleInRadians) * m_depth * m_initial.size.y / 2.f);
	m_vertices[2u].position = sf::Vector2f(m_initial.position.x + (std::sin(angleInRadians + pi) + 1.f) * m_initial.size.x / 2.f, m_initial.position.y - std::cos(angleInRadians + pi) * m_depth * m_initial.size.y / 2.f);
	m_vertices[3u].position = sf::Vector2f(m_initial.position.x + (std::sin(angleInRadians + pi) + 1.f) * m_initial.size.x / 2.f, m_initial.position.y + (std::cos(angleInRadians + pi) * m_depth + 2.f) * m_initial.size.y / 2.f);
	m_vertices[4u].position = sf::Vector2f(m_initial.position.x + (std::sin(angleInRadians) + 1.f) * m_initial.size.x / 2.f, m_initial.position.y + (std::cos(angleInRadians) * m_depth + 2.f) * m_initial.size.y / 2.f);

	// weld last vertex to second vertex (to complete the solid)
	m_vertices[5u] = m_vertices[1u];
}

void SpinningCard::spinVertically(const sf::Angle angle)
{
	const float angleInRadians{ angle.asRadians() - (pi / 2.f) };
	m_vertices[0u].position = sf::Vector2f(m_initial.position.x + m_initial.size.x / 2.f, m_initial.position.y + m_initial.size.y / 2.f);
	m_vertices[1u].position = sf::Vector2f(m_initial.position.x - std::cos(angleInRadians) * m_depth * m_initial.size.x / 2.f, m_initial.position.y + (std::sin(angleInRadians) + 1.f) * m_initial.size.y / 2.f);
	m_vertices[4u].position = sf::Vector2f(m_initial.position.x - std::cos(angleInRadians + pi) * m_depth * m_initial.size.x / 2.f, m_initial.position.y + (std::sin(angleInRadians + pi) + 1.f) * m_initial.size.y / 2.f);
	m_vertices[3u].position = sf::Vector2f(m_initial.position.x + (std::cos(angleInRadians + pi) * m_depth + 2.f) * m_initial.size.x / 2.f, m_initial.position.y + (std::sin(angleInRadians + pi) + 1.f) * m_initial.size.y / 2.f);
	m_vertices[2u].position = sf::Vector2f(m_initial.position.x + (std::cos(angleInRadians) * m_depth + 2.f) * m_initial.size.x / 2.f, m_initial.position.y + (std::sin(angleInRadians) + 1.f) * m_initial.size.y / 2.f);

	// weld last vertex to second vertex (to complete the solid)
	m_vertices[5u] = m_vertices[1u];
}

void SpinningCard::setDepth(float depth)
{
	m_depth = depth;
}

void SpinningCard::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	states.texture = m_pTexture;
	target.draw(m_vertices, states);
}

} // namespace selbaward
