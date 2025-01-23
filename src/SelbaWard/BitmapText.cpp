//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// BitmapText
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

#include "BitmapText.hpp"

namespace selbaward
{

BitmapText::BitmapText()
	: m_pBitmapFont{ nullptr }
	, m_vertices(sf::PrimitiveType::Triangles)
	, m_string()
	, m_color{ sf::Color::White }
	, m_tracking{ 1 }
{
}

void BitmapText::setBitmapFont(const BitmapFont& bitmapFont)
{
	m_pBitmapFont = &bitmapFont;
	priv_updateVertices();
}

void BitmapText::setBitmapFont()
{
	m_pBitmapFont = nullptr;
	priv_updateVertices();
}

void BitmapText::setString(const std::string& string)
{
	m_string = string;
	priv_updateVertices();
}

const std::string BitmapText::getString() const
{
	return m_string;
}

void BitmapText::setTracking(const int tracking)
{
	m_tracking = tracking;
	priv_updateVertices();
}

const int BitmapText::getTracking() const
{
	return m_tracking;
}

void BitmapText::setColor(const sf::Color& color)
{
	m_color = color;
	priv_updateColor();
}

const sf::Color BitmapText::getColor() const
{
	return m_color;
}

void BitmapText::setScale(const std::size_t scale)
{
	setScale(scale, scale);
}

void BitmapText::setScale(const sf::Vector2<std::size_t> scale)
{
	this->Transformable::setScale(sf::Vector2f(scale));
}

void BitmapText::setScale(const std::size_t scaleX, const std::size_t scaleY)
{
	setScale({ scaleX, scaleY });
}

sf::FloatRect BitmapText::getGlobalBounds() const
{
	return getTransform().transformRect(m_bounds);
}

sf::FloatRect BitmapText::getLocalBounds() const
{
	return m_bounds;
}



// PRIVATE

void BitmapText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_vertices.getVertexCount() == 0)
		return;

	states.transform *= getTransform();
	states.texture = m_pBitmapFont->getTexture();
	target.draw(m_vertices, states);
}

void BitmapText::priv_updateVertices()
{
	if (m_pBitmapFont == nullptr)
	{
		m_vertices.clear();
		m_bounds = { { 0.f, 0.f }, { 0.f, 0.f } };
	}

	m_vertices.resize(m_string.length() * 6u);

	sf::Vector2f penPosition{ 0.f, 0.f };

	sf::Vector2f min{ 0.f, 0.f };
	sf::Vector2f max{ 0.f, 0.f };

	for (std::size_t character{ 0u }; character < m_string.length(); ++character)
	{
		const std::size_t glyphNumber{ (m_string[character] >= 0u) ? static_cast<std::size_t>(m_string[character]) : static_cast<std::size_t>(m_string[character] + 256) }; // after 125, 126, 127 is -128, -127, -126. this moves them to 128, 129, 130

		const int kerning{ (character < m_string.length() - 1u) ? m_pBitmapFont->getKerning(m_string.substr(character, 2u)) : 0 };

		BitmapFont::Glyph glyph{ m_pBitmapFont->getGlyph(glyphNumber) };

		const sf::Vector2f glyphOffset{ 0.f - glyph.startX, (glyph.baseline < 0) ? (0.f - glyph.baseline - glyph.textureRect.size.y) : (0.f - glyph.baseline) };
		const sf::Vector2f glyphPosition{ penPosition + glyphOffset };

		m_vertices[(character * 6u) + 0u].position = glyphPosition;
		m_vertices[(character * 6u) + 1u].position = glyphPosition + sf::Vector2f(0, static_cast<float>(glyph.textureRect.size.y));
		m_vertices[(character * 6u) + 2u].position = glyphPosition + sf::Vector2f(static_cast<float>(glyph.textureRect.size.x), 0);
		m_vertices[(character * 6u) + 3u].position = glyphPosition + sf::Vector2f(static_cast<float>(glyph.textureRect.size.x), static_cast<float>(glyph.textureRect.size.y));

		m_vertices[(character * 6u) + 0u].texCoords = sf::Vector2f(
			static_cast<float>(glyph.textureRect.position.x),
			static_cast<float>(glyph.textureRect.position.y));
		m_vertices[(character * 6u) + 1u].texCoords = sf::Vector2f(
			static_cast<float>(glyph.textureRect.position.x),
			static_cast<float>(glyph.textureRect.position.y + glyph.textureRect.size.y));
		m_vertices[(character * 6u) + 2u].texCoords = sf::Vector2f(
			static_cast<float>(glyph.textureRect.position.x + glyph.textureRect.size.x),
			static_cast<float>(glyph.textureRect.position.y));
		m_vertices[(character * 6u) + 3u].texCoords = sf::Vector2f(
			static_cast<float>(glyph.textureRect.position.x + glyph.textureRect.size.x),
			static_cast<float>(glyph.textureRect.position.y + glyph.textureRect.size.y));

		m_vertices[(character * 6u) + 4u] = m_vertices[(character * 6u) + 2u];
		m_vertices[(character * 6u) + 5u] = m_vertices[(character * 6u) + 1u];

		penPosition.x += (glyph.width > 0) ? (0.f + m_tracking + kerning + glyph.width) : (0.f + m_tracking + kerning + glyph.width + glyph.textureRect.size.x - glyph.startX);

		min.x = std::min(min.x, m_vertices[(character * 6u) + 0u].position.x);
		max.x = std::max(max.x, m_vertices[(character * 6u) + 3u].position.x);
		min.y = std::min(min.y, m_vertices[(character * 6u) + 0u].position.y);
		max.y = std::max(max.y, m_vertices[(character * 6u) + 3u].position.y);
	}

	priv_updateColor();

	m_bounds.position.x = min.x;
	m_bounds.position.y = min.y;
	m_bounds.size.x = max.x - min.x;
	m_bounds.size.y = max.y - min.y;
}

void BitmapText::priv_updateColor()
{
	for (std::size_t v{ 0u }; v < m_vertices.getVertexCount(); ++v)
		m_vertices[v].color = m_color;
}

} // namespace selbaward
