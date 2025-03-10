//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Progress Bar
//
// Copyright(c) 2015-2025 M.J.Silk
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

#include "ProgressBar.hpp"

namespace selbaward
{

ProgressBar::ProgressBar(const sf::Vector2f size)
	: m_amount{ 0.f }
	, m_showBar{ true }
	, m_showBackground{ false }
	, m_size{ size }
	, m_color{ sf::Color::White }
	, m_bar(4u)
	, m_backgroundAndFrame(size)
	, m_texture{ nullptr }
	, m_backgroundTexture{ nullptr }
	, m_textureRectangle{}
	, m_backgroundTextureRectangle{}
{
	m_backgroundAndFrame.setFillColor(sf::Color::Black);
	m_backgroundAndFrame.setOutlineColor(sf::Color::White);
	m_backgroundAndFrame.setOutlineThickness(1.f);
}

void ProgressBar::setSize(const sf::Vector2f size)
{
	m_size = size;
	priv_updateGraphics();
}

void ProgressBar::setPercentage(const float percentage)
{
	setRatio(percentage / 100.f);
}

void ProgressBar::setRatio(const float ratio)
{
	m_amount = ratio;
	if (m_amount < 0.f)
		m_amount = 0.f;
	else if (m_amount > 1.f)
		m_amount = 1.f;
	priv_updateGraphics();
}

void ProgressBar::setFrameThickness(const float frameThickness)
{
	m_backgroundAndFrame.setOutlineThickness(frameThickness);
}

void ProgressBar::setFrameColor(const sf::Color frameColor)
{
	m_backgroundAndFrame.setOutlineColor(frameColor);
}

void ProgressBar::setBackgroundColor(const sf::Color backgroundColor)
{
	m_backgroundAndFrame.setFillColor(backgroundColor);
}

void ProgressBar::setColor(const sf::Color color)
{
	m_color = color;
	priv_updateGraphics();
}

void ProgressBar::setShowBar(const bool showBar)
{
	m_showBar = showBar;
}

void ProgressBar::setShowBackgroundAndFrame(const bool showBackgroundAndFrame)
{
	m_showBackground = showBackgroundAndFrame;
}

void ProgressBar::setTexture(const sf::Texture& texture, const bool resetRect)
{
	m_texture = &texture;
	if (resetRect)
	{
		m_textureRectangle.size.x = static_cast<int>(m_texture->getSize().x);
		m_textureRectangle.size.y = static_cast<int>(m_texture->getSize().y);
	}
	priv_updateGraphics();
}

void ProgressBar::setTexture()
{
	m_texture = nullptr;
	priv_updateGraphics();
}

void ProgressBar::setTextureRect(const sf::IntRect& textureRectangle)
{
	m_textureRectangle = textureRectangle;
	priv_updateGraphics();
}

void ProgressBar::setBackgroundTexture(const sf::Texture& backgroundTexture, const bool resetRect)
{
	m_backgroundTexture = &backgroundTexture;
	if (resetRect)
	{
		m_backgroundTextureRectangle.size.x = static_cast<int>(m_backgroundTexture->getSize().x);
		m_backgroundTextureRectangle.size.y = static_cast<int>(m_backgroundTexture->getSize().y);
	}
	priv_updateGraphics();
}

void ProgressBar::setBackgroundTexture()
{
	m_backgroundTexture = nullptr;
	priv_updateGraphics();
}

void ProgressBar::setBackgroundTextureRect(const sf::IntRect& backgroundTextureRectangle)
{
	m_backgroundTextureRectangle = backgroundTextureRectangle;
	priv_updateGraphics();
}

const sf::Texture& ProgressBar::getTexture() const
{
	return *m_texture;
}

const sf::Texture& ProgressBar::getBackgroundTexture() const
{
	return *m_backgroundTexture;
}

sf::FloatRect ProgressBar::getLocalBounds() const
{
	if (m_showBackground && (m_backgroundAndFrame.getOutlineThickness() > 0.f))
	{
		const float outlineThickness{ m_backgroundAndFrame.getOutlineThickness() };
		return{ sf::Vector2f{ 0.f - outlineThickness, 0.f - outlineThickness }, sf::Vector2f{ m_size.x + outlineThickness * 2.f, m_size.y + outlineThickness * 2.f } };
	}
	else
		return{ sf::Vector2f{ 0.f, 0.f }, m_size };
}

sf::FloatRect ProgressBar::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::Vector2f ProgressBar::getAnchorProgressTop() const
{
	return getTransform().transformPoint({ m_size.x * m_amount, 0.f });
}

sf::Vector2f ProgressBar::getAnchorProgressCenter() const
{
	return getTransform().transformPoint({ m_size.x * m_amount, m_size.y / 2.f });
}

sf::Vector2f ProgressBar::getAnchorProgressBottom() const
{
	return getTransform().transformPoint({ m_size.x * m_amount, m_size.y });
}



// PRIVATE
void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= getTransform();
	if (m_showBackground)
		target.draw(m_backgroundAndFrame, states);
	if (m_showBar)
	{
		states.texture = m_texture;
		target.draw(m_bar.data(), 4u, sf::PrimitiveType::TriangleStrip, states);
	}
}

void ProgressBar::priv_updateGraphics()
{
	m_backgroundAndFrame.setSize(m_size);
	m_backgroundAndFrame.setTexture(m_backgroundTexture);
	m_backgroundAndFrame.setTextureRect(m_backgroundTextureRectangle);

	const float width{ m_size.x * m_amount };
	m_bar[0u].position = { 0.f, 0.f };
	m_bar[1u].position = { 0.f, m_size.y };
	m_bar[2u].position = { width, 0.f };
	m_bar[3u].position = { width, m_size.y };
	sf::FloatRect textureRect{ m_textureRectangle };
	textureRect.size.x = textureRect.size.x * m_amount;
	m_bar[0u].texCoords = { textureRect.position.x, textureRect.position.y };
	m_bar[1u].texCoords = { textureRect.position.x, textureRect.position.y + textureRect.size.y };
	m_bar[2u].texCoords = { textureRect.position.x + textureRect.size.x, textureRect.position.y };
	m_bar[3u].texCoords = { textureRect.position.x + textureRect.size.x, textureRect.position.y + textureRect.size.y };
	for (auto& vertex : m_bar)
		vertex.color = m_color;
}

} // namespace selbward
