//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Pixel Display
//
// Copyright(c) 2019-2025 M.J.Silk
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

#ifndef SELBAWARD_PIXELDISPLAY_HPP
#define SELBAWARD_PIXELDISPLAY_HPP

#include "Common.hpp"

namespace selbaward
{

// SW PixelDisplay v1.0.2
class PixelDisplay : public sf::Drawable, public sf::Transformable
{
public:
	PixelDisplay();

	std::size_t getIndex(sf::Vector2<std::size_t> location) const { return m_resolution.x * location.y + location.x; }

	void setSize(sf::Vector2f size);
	void setResolution(sf::Vector2<std::size_t> resolution);
	sf::Vector2<std::size_t> getResolution() const { return m_resolution; }
	sf::Vector2f getSize() const { return m_size; }

	void setPixel(std::size_t index, std::size_t color);
	template <class T>
	void setPixels(const T* colors, std::size_t numberOfColors, std::size_t startIndex);
	void setPixels(const char* colors, std::size_t numberOfColors, std::size_t startIndex);
	template <class T>
	void getPixels(T* colors, std::size_t numberOfColors, std::size_t startIndex) const;
	void getPixels(char* colors, std::size_t numberOfColors, std::size_t startIndex) const;

	void fill(std::size_t color);
	void clear();
	void randomize();

	// manual shifting/scrolling
	void shiftLeft(std::size_t amount = 1u, std::size_t startIndex = 0u, std::size_t numberOfPixels = 0u);
	void shiftRight(std::size_t amount = 1u, std::size_t startIndex = 0u, std::size_t numberOfPixels = 0u);
	void scrollUp(std::size_t color = 0u, std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollDown(std::size_t color = 0u, std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollLeft(std::size_t color = 0u, std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollRight(std::size_t color = 0u, std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollWrapUp(std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollWrapDown(std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollWrapLeft(std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });
	void scrollWrapRight(std::size_t amount = 1u, sf::IntRect selectionRectangle = { { 0, 0 }, { 0, 0 } });

	// palette
	void setPalette(const std::vector<sf::Color>& palette);
	void setPaletteSize(std::size_t numberOfColors);
	void setColor(std::size_t color, sf::Color rgb);
	void removeColor(std::size_t color);
	void addRgb(sf::Color rgb);
	void cyclePaletteDown(std::size_t amount = 1u, std::size_t firstColor = 0u, std::size_t numberOfColors = 0u); // first colour and last colour of range to cycle
	void cyclePaletteUp(std::size_t amount = 1u, std::size_t firstColor = 0u, std::size_t numberOfColors = 0u); // first colour and last colour of range to cycle
	std::size_t getPaletteSize() const { return m_palette.size(); };
	sf::Color getRgb(std::size_t color) const { return m_palette[color]; }

	// buffers (clipboards/"screenshots"/captures)
	std::size_t copy(); // returns index of buffer
	std::size_t copy(sf::IntRect selectionRectangle); // returns index of buffer
	void paste(sf::Vector2i offset = { 0, 0 }); // replace with last saved buffer
	void removeBuffer(); // removes last saved buffer
	void copy(std::size_t index); // copies over (replaces) an existing buffer
	void copy(std::size_t index, sf::IntRect selectionRectangle);
	void paste(std::size_t index, sf::Vector2i offset = { 0, 0 }); // replace with saved buffer
	void removeBuffer(std::size_t index); // as usual, when one buffer is removed, the indices of all following buffers are decreased
	void removeAllBuffers() { m_buffers.clear(); }
	std::size_t addBuffer(sf::Vector2<std::size_t> size = { 1u, 1u }); // returns index of new buffer
	void resizeBuffer(std::size_t index, sf::Vector2<std::size_t> size);
	std::size_t getNumberOfBuffers() const { return m_buffers.size(); }
	sf::Vector2<std::size_t> getSizeOfBuffer(std::size_t index) const;


private:
	sf::Vector2<std::size_t> m_resolution;
	sf::Vector2f m_size;
	std::vector<std::size_t> m_pixels;
	std::vector<sf::Color> m_palette;
	std::vector<sf::Vertex> m_vertices;

	// buffers
	struct Buffer
	{
		std::size_t width{};
		std::vector<std::size_t> pixels{};
	};
	std::vector<Buffer> m_buffers;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void priv_updateVertices();
	void priv_updatePixels();
	void priv_updatePixel(const std::size_t index);
	std::size_t priv_getRandomColor() const;
	void priv_copyToBufferFromSelectionRectangle(Buffer& buffer, const sf::IntRect& selectionRectangle);
	void priv_pasteOffsetBuffer(const Buffer& buffer, const sf::Vector2i& offset);
	bool priv_isSelectionRectangleFullyContained(const sf::IntRect& selectionRectangle) const;
};







template <class T>
void PixelDisplay::setPixels(const T* colors, const std::size_t numberOfColors, const std::size_t startIndex)
{
	for (std::size_t i{ 0u }; i < numberOfColors; ++i)
		m_pixels[startIndex + i] = static_cast<std::size_t>(colors[i]);
	priv_updatePixels();
}

template <class T>
void PixelDisplay::getPixels(T* colors, const std::size_t numberOfColors, const std::size_t startIndex) const
{
	for (std::size_t i{ 0u }; i < numberOfColors; ++i)
		colors[i] = static_cast<T>(m_pixels[startIndex + i]);
}

} // namespace selbaward
#endif // SELBAWARD_PIXELDISPLAY_HPP
