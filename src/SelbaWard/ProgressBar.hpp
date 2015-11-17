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

#ifndef SELBAWARD_PROGRESSBAR_HPP
#define SELBAWARD_PROGRESSBAR_HPP

#include "Common.hpp"

#include <SFML/Graphics/RectangleShape.hpp> // simple box drawing

namespace selbaward
{

class ProgressBar : public sf::Drawable, public sf::Transformable
{
public:
	// creation and size
	ProgressBar(sf::Vector2f size = { 64.f, 8.f });
	void setSize(sf::Vector2f size);
	const sf::Vector2f getSize() const;
	
	// progress
	void setRatio(float ratio);
	const float getRatio() const;
	void setPercentage(float percentage);
	const float getPercentage() const;
	template <class T>
	void setFromValueInRange(const T& value, const T& minimum, const T& maximum);
	template <class T>
	void setFromValueInRange(const T& value, const T& range);
	
	// visual representation
	void setFrameThickness(float frameThickness);
	void setFrameColor(sf::Color frameColor);
	void setBackgroundColor(sf::Color backgroundColor);
	void setColor(sf::Color color);
	void setBackgroundAndFrameEnabled(bool backgroundAndFrameEnabled = true);
	const float getFrameThickness() const;
	const sf::Color getFrameColor() const;
	const sf::Color getBackgroundColor() const;
	const sf::Color getColor() const;
	const bool getBackgroundAndFrameEnabled() const;

	// bounds
	const sf::FloatRect getLocalBounds() const;
	const sf::FloatRect getGlobalBounds() const;

private:
	float m_amount;
	bool m_isBackgroundEnabled;
	sf::Vector2f m_size;
	sf::RectangleShape m_bar;
	sf::RectangleShape m_backgroundAndFrame;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void priv_updateGraphics();
};

inline const sf::Vector2f ProgressBar::getSize() const
{
	return m_size;
}

inline const float ProgressBar::getRatio() const
{
	return m_amount;
}

inline const float ProgressBar::getPercentage() const
{
	return m_amount * 100.f;
}

template <class T>
void ProgressBar::setFromValueInRange(const T& value, const T& minimum, const T& maximum)
{
	setRatio(static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum));
}

template <class T>
void ProgressBar::setFromValueInRange(const T& value, const T& range)
{
	setRatio(static_cast<float>(value) / static_cast<float>(range));
}

inline const float ProgressBar::getFrameThickness() const
{
	return m_backgroundAndFrame.getOutlineThickness();
}

inline const sf::Color ProgressBar::getFrameColor() const
{
	return m_backgroundAndFrame.getOutlineColor();
}

inline const sf::Color ProgressBar::getBackgroundColor() const
{
	return m_backgroundAndFrame.getFillColor();
}

inline const sf::Color ProgressBar::getColor() const
{
	return m_bar.getFillColor();
}

inline const bool ProgressBar::getBackgroundAndFrameEnabled() const
{
	return m_isBackgroundEnabled;
}

} // namespace selbaward
#endif // SELBAWARD_PROGRESSBAR_HPP
