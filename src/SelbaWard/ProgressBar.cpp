//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Progress Bar
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

#include "ProgressBar.hpp"

namespace selbaward
{

ProgressBar::ProgressBar(sf::Vector2f size)
	: m_amount(0.f)
	, m_size(size)
	, m_isBackgroundEnabled(false)
	, m_bar({ 0.f, size.y })
	, m_backgroundAndFrame(size)
{
	m_backgroundAndFrame.setFillColor(sf::Color::Black);
	m_backgroundAndFrame.setOutlineColor(sf::Color::White);
	m_bar.setFillColor(sf::Color::White);
	m_backgroundAndFrame.setOutlineThickness(1.f);
}

void ProgressBar::setSize(sf::Vector2f size)
{
	m_size = size;
	priv_updateGraphics();
}

void ProgressBar::setPercentage(float percentage)
{
	setRatio(percentage / 100.f);
}

void ProgressBar::setRatio(float ratio)
{
	m_amount = ratio;
	if (m_amount < 0.f)
		m_amount = 0.f;
	else if (m_amount > 1.f)
		m_amount = 1.f;
	priv_updateGraphics();
}

void ProgressBar::setFrameThickness(float frameThickness)
{
	m_backgroundAndFrame.setOutlineThickness(frameThickness);
}

void ProgressBar::setFrameColor(sf::Color frameColor)
{
	m_backgroundAndFrame.setOutlineColor(frameColor);
}

void ProgressBar::setBackgroundColor(sf::Color backgroundColor)
{
	m_backgroundAndFrame.setFillColor(backgroundColor);
}

void ProgressBar::setColor(sf::Color color)
{
	m_bar.setFillColor(color);
}

void ProgressBar::setBackgroundAndFrameEnabled(bool backgroundAndFrameEnabled)
{
	m_isBackgroundEnabled = backgroundAndFrameEnabled;
}

const sf::FloatRect ProgressBar::getLocalBounds() const
{
	if (m_isBackgroundEnabled && m_backgroundAndFrame.getOutlineThickness() > 0.f)
	{
		const float outlineThickness{ m_backgroundAndFrame.getOutlineThickness() };
		return{ { 0.f - outlineThickness, 0.f - outlineThickness }, { m_size.x + outlineThickness * 2, m_size.y + outlineThickness * 2 } };
	}
	else
		return{ { 0.f, 0.f }, m_size };
}

const sf::FloatRect ProgressBar::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}



// PRIVATE
void ProgressBar::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = NULL;
	states.transform = getTransform();
	if (m_isBackgroundEnabled)
		target.draw(m_backgroundAndFrame, states);
	target.draw(m_bar, states);
}

void ProgressBar::priv_updateGraphics()
{
	m_backgroundAndFrame.setSize(m_size);
	m_bar.setSize({ m_size.x * m_amount, m_size.y });
}

} // namespace selbward
