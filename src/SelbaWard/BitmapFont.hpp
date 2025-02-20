//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// BitmapFont
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

#ifndef SELBAWARD_BITMAPFONT_HPP
#define SELBAWARD_BITMAPFONT_HPP

#include "Common.hpp"
#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <map> // for kerning

namespace selbaward
{

// SW Bitmap Font v1.1.3
class BitmapFont
{
public:
	struct Glyph
	{
		bool useDefaultTextureRect{ true };
		sf::IntRect textureRect{};
		int width{}; // zero and below represent counting from full texture rect width e.g. 0 is full width, -1 is 1 less than full width.
		int baseline{}; // negative numbers represent counting from bottom e.g. -1 is bottom line, -2 is 1 above bottom.
		int startX{}; // negative numbers represent actual negative values
	};

	BitmapFont();

	// output
	const sf::Texture* getTexture() const;
	const Glyph getGlyph(std::size_t glyphIndex = 0u) const;
	const std::size_t getNumberOfGlyphs() const;
	const int getKerning(const std::string& glyphPair) const;

	// texture setup
	void setExternalTexture(const sf::Texture& externalTexture);
	void loadTexture(const std::string& filename);
	void setSmooth(bool smooth = true);
	void setNumberOfTilesPerRow(std::size_t numberOfTilesPerRow);

	// texture rect setup
	void setDefaultTextureRect(const sf::IntRect& defaultTextureRect);
	void setTextureRect(const sf::IntRect& textureRect, std::size_t glyphIndex = 0u);
	void setTextureRects(const std::vector<sf::IntRect>& textureRects, std::size_t initialGlyphIndex = 0u);
	void clearTextureRect(std::size_t glyphIndex);
	void clearAllTextureRects();

	// glyph setup
	void setGlyphToDefault(std::size_t glyphIndex = 0u);
	void setGlyphsToDefault(std::size_t numberOfGlyphs = 1u, std::size_t glyphIndex = 0u);
	void setAllGlyphsToDefault();

	// glyph attribute setup
	void setBaseline(int baseline, std::size_t glyphIndex = 0u);
	void setWidth(int width, std::size_t glyphIndex = 0u);
	void setStartX(int startX, std::size_t glyphIndex = 0u);
	void setBaselines(int baseline, std::size_t numberOfGlyphs = 1u, std::size_t initialGlyphIndex = 0u);
	void setWidths(int width, std::size_t numberOfGlyphs = 1u, std::size_t initialGlyphIndex = 0u);
	void setStartXs(int startX, std::size_t numberOfGlyphs = 1u, std::size_t initialGlyphIndex = 0u);
	void setBaselines(const std::vector<int>& baselines, std::size_t initialGlyphIndex = 0u);
	void setWidths(const std::vector<int>& widths, std::size_t initialGlyphIndex = 0u);
	void setStartXs(const std::vector<int>& startXs, std::size_t initialGlyphIndex = 0u);
	void setBaseline(int baseline, const std::string& glyphs);
	void setWidth(int width, const std::string& glyphs);
	void setStartX(int startX, const std::string& glyphs);

	// kerning pairs setup
	void setKerning(int kerning, const std::string& glyphPairs); // string length should be a multiple of 2
	void setKerning(int kerning, const std::vector<std::string>& glyphPairs); // vector of character pairs (each string must have length of 2)

	// general setup
	void setThrowExceptions(bool throwExceptions = true);
	const bool getThrowExceptions() const;







private:
	bool m_throwExceptions;
	bool m_useExternalTexture;
	sf::Texture m_texture;
	const sf::Texture* m_mExternalTexture;
	std::size_t m_numberOfTilesPerRow;
	//sf::Vector2u m_tileSize;
	sf::IntRect m_defaultTextureRect;
	mutable std::map<std::string, int> m_kernings;
	std::vector<Glyph> m_glyphs;

	const bool priv_isGlyphIndexValid(std::size_t glyphIndex) const;
	const Glyph priv_getGlyphWithDefaultTextureRect(std::size_t glyphIndex = 0u) const;
	void priv_setKerning(int kerning, const std::string& glyphs); // string must have length of 2
};

} // namespace selbaward
#endif // SELBAWARD_BITMAPFONT_HPP
