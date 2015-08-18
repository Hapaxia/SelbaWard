//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward
// --
//
// BitmapFont
//
// Copyright(c) 2014-2015 M.J.Silk
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

#include "BitmapFont.hpp"

namespace selbaward
{

BitmapFont::BitmapFont() :
m_throwExceptions(true),
m_useExternalTexture(false),
m_texture(),
m_mExternalTexture(nullptr),
m_numberOfTilesPerRow(1),
m_tileSize({ 0, 0 }),
m_defaultTextureRect(),
m_glyphs(256) // create 256 glyphs to cover the base ascii set
{
	setAllGlyphsToDefault();
}

void BitmapFont::setExternalTexture(const sf::Texture& externalTexture)
{
	m_mExternalTexture = &externalTexture;
	m_useExternalTexture = true;
}

void BitmapFont::loadTexture(const std::string& filename)
{
	if (!m_texture.loadFromFile(filename))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "failed to load texture.");
		return;
	}
	m_useExternalTexture = false;
}

void BitmapFont::setSmooth(bool smooth)
{
	if (!m_useExternalTexture)
		m_texture.setSmooth(smooth);
}

void BitmapFont::setNumberOfTilesPerRow(unsigned int numberOfTilesPerRow)
{
	m_numberOfTilesPerRow = numberOfTilesPerRow;
}

void BitmapFont::setThrowExceptions(bool throwExceptions)
{
	m_throwExceptions = throwExceptions;
}

const bool BitmapFont::getThrowExceptions() const
{
	return m_throwExceptions;
}








void BitmapFont::setDefaultTextureRect(const sf::IntRect& defaultTextureRect)
{
	m_defaultTextureRect = defaultTextureRect;
}

void BitmapFont::setTextureRect(const sf::IntRect& textureRect, unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot set texture rect - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].useDefaultTextureRect = false;;
	m_glyphs[glyphIndex].textureRect = textureRect;
	m_glyphs[glyphIndex].width = textureRect.width;
	m_glyphs[glyphIndex].baseline = textureRect.height;
	m_glyphs[glyphIndex].startX = 0;
}

void BitmapFont::setTextureRects(const std::vector<sf::IntRect>& textureRects, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < textureRects.size(); ++i)
		setTextureRect(textureRects[i], initialGlyphIndex + i);
}

void BitmapFont::clearTextureRect(unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot clear texture rect - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].useDefaultTextureRect = true;
}

void BitmapFont::clearAllTextureRects()
{
	for (unsigned int i{ 0 }; i < m_glyphs.size(); ++i)
		clearTextureRect(i);
}






void BitmapFont::setGlyphToDefault(unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot set glyph to default - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].useDefaultTextureRect = true;
	m_glyphs[glyphIndex].textureRect = m_defaultTextureRect;
	m_glyphs[glyphIndex].width = 0;
	m_glyphs[glyphIndex].baseline = -1;
	m_glyphs[glyphIndex].startX = 0;
}

void BitmapFont::setGlyphsToDefault(unsigned int numberOfGlyphs, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < numberOfGlyphs; ++i)
		setGlyphToDefault(initialGlyphIndex + i);
}

void BitmapFont::setAllGlyphsToDefault()
{
	setGlyphsToDefault(m_glyphs.size());
}









void BitmapFont::setBaseline(int baseline, unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot set glyph baseline - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].baseline = baseline;
}

void BitmapFont::setWidth(int width, unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot set glyph width - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].width = width;
}

void BitmapFont::setStartX(int startX, unsigned int glyphIndex)
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot set glyph start x - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		return;
	}

	m_glyphs[glyphIndex].startX = startX;
}

void BitmapFont::setBaselines(int baseline, unsigned int numberOfGlyphs, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < numberOfGlyphs; ++i)
		setBaseline(baseline, initialGlyphIndex + i);
}

void BitmapFont::setWidths(int width, unsigned int numberOfGlyphs, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < numberOfGlyphs; ++i)
		setWidth(width, initialGlyphIndex + i);
}

void BitmapFont::setStartXs(int startX, unsigned int numberOfGlyphs, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < numberOfGlyphs; ++i)
		setStartX(startX, initialGlyphIndex + i);
}

void BitmapFont::setBaselines(const std::vector<int>& baselines, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < baselines.size(); ++i)
		setBaseline(baselines[i], initialGlyphIndex + i);
}

void BitmapFont::setWidths(const std::vector<int>& widths, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < widths.size(); ++i)
		setWidth(widths[i], initialGlyphIndex + i);
}

void BitmapFont::setStartXs(const std::vector<int>& startXs, unsigned int initialGlyphIndex)
{
	for (unsigned int i{ 0 }; i < startXs.size(); ++i)
		setStartX(startXs[i], initialGlyphIndex + i);
}

/*
void BitmapFont::setBaseline(int baseline, const std::vector<unsigned int>& glyphs)
{
	for (auto& glyph : glyphs)
		setBaseline(baseline, glyph);
}
*/

void BitmapFont::setBaseline(int baseline, const std::string& glyphs)
{
	for (auto& glyph : glyphs)
		setBaseline(baseline, glyph);
}

void BitmapFont::setWidth(int width, const std::string& glyphs)
{
	for (auto& glyph : glyphs)
		setWidth(width, glyph);
}

void BitmapFont::setStartX(int startX, const std::string& glyphs)
{
	for (auto& glyph : glyphs)
		setStartX(startX, glyph);
}

const sf::Texture* BitmapFont::getTexture() const
{
	if (m_useExternalTexture)
		return m_mExternalTexture;
	else
		return &m_texture;
}

const BitmapFont::Glyph BitmapFont::getGlyph(unsigned int glyphIndex) const
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot get glyph - glyph index out of range.");
		return getGlyphWithDefaultTextureRect();
	}

	if (m_glyphs[glyphIndex].useDefaultTextureRect)
		return getGlyphWithDefaultTextureRect(glyphIndex);
	else
		return m_glyphs[glyphIndex];
}

const unsigned int BitmapFont::getNumberOfGlyphs() const
{
	return m_glyphs.size();
}



// PRIVATE

const bool BitmapFont::isGlyphIndexValid(unsigned int glyphIndex) const
{
	if (glyphIndex < m_glyphs.size())
		return true;
	else if (m_throwExceptions)
		throw Exception(exceptionPrefix + "glyph index (" + std::to_string(glyphIndex) + ") out of range.");
	else
		return false;
}

const BitmapFont::Glyph BitmapFont::getGlyphWithDefaultTextureRect(unsigned int glyphIndex) const
{
	if (!isGlyphIndexValid(glyphIndex))
	{
		if (m_throwExceptions)
			throw Exception(exceptionPrefix + "cannot get default glyph - glyph index (" + std::to_string(glyphIndex) + ") out of range.");
		glyphIndex = 0;
	}

	if (m_glyphs.size() == 0)
		throw Exception(exceptionPrefix + "BUG - no glyphs available.");

	Glyph defaultGlyph;
	defaultGlyph.useDefaultTextureRect = false;
	defaultGlyph.textureRect = m_defaultTextureRect;
	defaultGlyph.textureRect.left = m_defaultTextureRect.width * (glyphIndex % m_numberOfTilesPerRow);
	defaultGlyph.textureRect.top = m_defaultTextureRect.height * (glyphIndex / m_numberOfTilesPerRow);
	defaultGlyph.width = m_glyphs[glyphIndex].width;
	defaultGlyph.baseline = m_glyphs[glyphIndex].baseline;
	defaultGlyph.startX = m_glyphs[glyphIndex].startX;
	return defaultGlyph;
}

} // namespace selbaward
