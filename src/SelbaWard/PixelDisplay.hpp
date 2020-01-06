//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Pixel Display
//
// Copyright(c) 2019-2020 M.J.Silk
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

// SW PixelDisplay v1.0.0
class PixelDisplay : public sf::Drawable, public sf::Transformable
{
public:
	PixelDisplay();

	unsigned int getIndex(sf::Vector2u location) const { return m_resolution.x * location.y + location.x; }

	void setSize(sf::Vector2f size);
	void setResolution(sf::Vector2u resolution);
	sf::Vector2u getResolution() const { return m_resolution; }
	sf::Vector2f getSize() const { return m_size; }

	void setPixel(unsigned int index, unsigned int color);
	template <class T>
	void setPixels(const T* colors, unsigned int numberOfColors, unsigned int startIndex);
	void setPixels(const char* colors, unsigned int numberOfColors, unsigned int startIndex);
	template <class T>
	void getPixels(T* colors, unsigned int numberOfColors, unsigned int startIndex) const;
	void getPixels(char* colors, unsigned int numberOfColors, unsigned int startIndex) const;

	void fill(unsigned int color);
	void clear();
	void randomize();

	// manual shifting/scrolling
	void shiftLeft(unsigned int amount = 1u, unsigned int startIndex = 0u, unsigned int numberOfPixels = 0u);
	void shiftRight(unsigned int amount = 1u, unsigned int startIndex = 0u, unsigned int numberOfPixels = 0u);
	void scrollUp(unsigned int color = 0u, unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollDown(unsigned int color = 0u, unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollLeft(unsigned int color = 0u, unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollRight(unsigned int color = 0u, unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollWrapUp(unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollWrapDown(unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollWrapLeft(unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });
	void scrollWrapRight(unsigned int amount = 1u, sf::IntRect selectionRectangle = { 0, 0, 0, 0 });

	// palette
	void setPalette(const std::vector<sf::Color>& palette);
	void setPaletteSize(unsigned int numberOfColors);
	void setColor(unsigned int color, sf::Color rgb);
	void removeColor(unsigned int color);
	void addRgb(sf::Color rgb);
	void cyclePaletteDown(unsigned int amount = 1u, unsigned int firstColor = 0u, unsigned int numberOfColors = 0u); // first colour and last colour of range to cycle
	void cyclePaletteUp(unsigned int amount = 1u, unsigned int firstColor = 0u, unsigned int numberOfColors = 0u); // first colour and last colour of range to cycle
	std::size_t getPaletteSize() const { return m_palette.size(); };
	sf::Color getRgb(unsigned int color) const { return m_palette[color]; }

	// buffers (clipboards/"screenshots"/captures)
	unsigned int copy(); // returns index of buffer
	unsigned int copy(sf::IntRect selectionRectangle); // returns index of buffer
	void paste(sf::Vector2i offset = { 0, 0 }); // replace with last saved buffer
	void removeBuffer(); // removes last saved buffer
	void copy(unsigned int index); // copies over (replaces) an existing buffer
	void copy(unsigned int index, sf::IntRect selectionRectangle);
	void paste(unsigned int index, sf::Vector2i offset = { 0, 0 }); // replace with saved buffer
	void removeBuffer(unsigned int index); // as usual, when one buffer is removed, the indices of all following buffers are decreased
	void removeAllBuffers() { m_buffers.clear(); }
	unsigned int addBuffer(sf::Vector2u size = { 1u, 1u }); // returns index of new buffer
	void resizeBuffer(unsigned int index, sf::Vector2u size);
	unsigned int getNumberOfBuffers() const { return static_cast<unsigned int>(m_buffers.size()); }
	sf::Vector2u getSizeOfBuffer(unsigned int index) const;


private:
	sf::Vector2u m_resolution;
	sf::Vector2f m_size;
	std::vector<unsigned int> m_pixels;
	std::vector<sf::Color> m_palette;
	std::vector<sf::Vertex> m_vertices;

	// buffers
	struct Buffer
	{
		unsigned int width;
		std::vector<unsigned int> pixels;
	};
	std::vector<Buffer> m_buffers;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void priv_updateVertices();
	void priv_updatePixels();
	void priv_updatePixel(const unsigned int index);
	unsigned int priv_getRandomColor() const;
	void priv_copyToBufferFromSelectionRectangle(Buffer& buffer, const sf::IntRect& selectionRectangle);
	void priv_pasteOffsetBuffer(const Buffer& buffer, const sf::Vector2i& offset);
	bool priv_isSelectionRectangleFullyContained(const sf::IntRect& selectionRectangle);
};







template <class T>
void PixelDisplay::setPixels(const T* colors, const unsigned int numberOfColors, const unsigned int startIndex)
{
	for (unsigned int i{ 0u }; i < numberOfColors; ++i)
		m_pixels[startIndex + i] = static_cast<unsigned int>(colors[i]);
	priv_updatePixels();
}

template <class T>
void PixelDisplay::getPixels(T* colors, const unsigned int numberOfColors, const unsigned int startIndex) const
{
	for (unsigned int i{ 0u }; i < numberOfColors; ++i)
		colors[i] = static_cast<T>(m_pixels[startIndex + i]);
}

} // namespace selbaward
#endif // SELBAWARD_PIXELDISPLAY_HPP
