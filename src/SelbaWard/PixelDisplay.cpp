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

#include "PixelDisplay.hpp"

#include <random>
#include <functional>
#include <assert.h>

namespace
{

std::mt19937 randomGenerator;
const std::uniform_int_distribution<unsigned short int> randomDistribution(0u, 255u);
std::function <unsigned short int()> getRandomByteValue;

inline void randomSeed()
{
	std::random_device rd;
	randomGenerator.seed(rd());
	getRandomByteValue = std::bind(randomDistribution, randomGenerator);
}

inline unsigned char randomByte()
{
	return static_cast<unsigned char>(getRandomByteValue());
}

template <class T>
void shiftVectorDown(std::vector<T>& vector, const unsigned int amount, const unsigned int startIndex, unsigned int numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = static_cast<unsigned int>(vector.size()) - startIndex;
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + i] = vector[startIndex + i + amount];
}

template <class T>
void shiftVectorUp(std::vector<T>& vector, const unsigned int amount, const unsigned int startIndex, unsigned int numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = static_cast<unsigned int>(vector.size()) - startIndex;
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + numberOfElements - i - 1u] = vector[startIndex + numberOfElements - (i + amount) - 1u];
}

template <class T>
void shiftVectorWrapDown(std::vector<T>& vector, const unsigned int amount, const unsigned int startIndex, unsigned int numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = static_cast<unsigned int>(vector.size()) - startIndex;
	std::vector<T> tempPixels(amount);
	for (unsigned int i{ 0u }; i < amount; ++i)
		tempPixels[i] = vector[startIndex + i];
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + i] = vector[startIndex + i + amount];
	for (unsigned int i{ 0u }; i < amount; ++i)
		vector[startIndex + numberOfElements - amount + i] = tempPixels[i];
}

template <class T>
void shiftVectorWrapUp(std::vector<T>& vector, const unsigned int amount, const unsigned int startIndex, unsigned int numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = static_cast<unsigned int>(vector.size()) - startIndex;
	std::vector<T> tempPixels(amount);
	for (unsigned int i{ 0u }; i < amount; ++i)
		tempPixels[i] = vector[startIndex + numberOfElements - amount + i];
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + numberOfElements - i - 1u] = vector[startIndex + numberOfElements - (i + amount) - 1u];
	for (unsigned int i{ 0u }; i < amount; ++i)
		vector[startIndex + i] = tempPixels[i];
}

} // namespace

namespace selbaward
{

PixelDisplay::PixelDisplay()
	: m_vertices()
	, m_resolution(320u, 240u)
	, m_size(640.f, 480.f)
	, m_palette(16u)
	, m_pixels()
	, m_buffers()
{
	priv_updateVertices();
	m_pixels.resize(m_resolution.x * m_resolution.y);
	for (auto& pixel : m_pixels)
		pixel = 0u;
	priv_updatePixels();
}

void PixelDisplay::setSize(const sf::Vector2f size)
{
	m_size = size;
	priv_updateVertices();
}

void PixelDisplay::setResolution(const sf::Vector2u resolution)
{
	m_resolution = resolution;
	m_pixels.resize(m_resolution.x * m_resolution.y);
	priv_updateVertices();
}

void PixelDisplay::setPaletteSize(const unsigned int numberOfColors)
{
	m_palette.resize(numberOfColors);
}

void PixelDisplay::setColor(const unsigned int color, const sf::Color rgb)
{
	assert(color < m_palette.size());
	m_palette[color] = rgb;
	priv_updatePixels();
}

void PixelDisplay::setPixel(const unsigned int index, const unsigned int color)
{
	assert(index < m_pixels.size());
	m_pixels[index] = color;
	priv_updatePixel(index);
}

void PixelDisplay::setPixels(const char* colors, const unsigned int numberOfColors, const unsigned int startIndex)
{
	assert(startIndex < m_pixels.size());
	setPixels(reinterpret_cast<const unsigned char*>(colors), numberOfColors, startIndex);
}

void PixelDisplay::getPixels(char* colors, const unsigned int numberOfColors, const unsigned int startIndex) const
{
	assert(startIndex < m_pixels.size());
	getPixels(reinterpret_cast<unsigned char*>(colors), numberOfColors, startIndex);
}

void PixelDisplay::fill(const unsigned int color)
{
	assert(color < m_palette.size());
	for (auto& pixel : m_pixels)
		pixel = color;
	priv_updatePixels();
}

void PixelDisplay::clear()
{
	assert(m_palette.size() != 0u);
	fill(0u);
}

void PixelDisplay::randomize()
{
	for (auto& pixel : m_pixels)
		pixel = priv_getRandomColor();
	priv_updatePixels();
}

void PixelDisplay::shiftLeft(const unsigned int amount, const unsigned int startIndex, const unsigned int numberOfPixels)
{
	assert(startIndex < m_pixels.size());
	shiftVectorWrapDown(m_pixels, amount, startIndex, numberOfPixels);
	priv_updatePixels();
}

void PixelDisplay::shiftRight(const unsigned int amount, const unsigned int startIndex, const unsigned int numberOfPixels)
{
	assert(startIndex < m_pixels.size());
	shiftVectorWrapUp(m_pixels, amount, startIndex, numberOfPixels);
	priv_updatePixels();
}

void PixelDisplay::scrollUp(const unsigned int color, const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		{
			const unsigned int index{ (rectangle.top + y) * m_resolution.x + rectangle.left + x };
			m_pixels[index] = (y == rectangle.height - 1u) ? color : m_pixels[index + m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollDown(const unsigned int color, const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		{
			const unsigned int index{ (rectangle.top + rectangle.height - y - 1u) * m_resolution.x + rectangle.left + x };
			m_pixels[index] = (y == rectangle.height - 1u) ? color : m_pixels[index - m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollLeft(const unsigned int color, const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		const unsigned int firstPixelIndex{ (rectangle.top + y) * m_resolution.x + rectangle.left };
		shiftVectorDown(m_pixels, amount, firstPixelIndex, rectangle.width);
		m_pixels[firstPixelIndex + rectangle.width - 1u] = color;
	}
	priv_updatePixels();
}

void PixelDisplay::scrollRight(const unsigned int color, const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		const unsigned int firstPixelIndex{ (rectangle.top + y) * m_resolution.x + rectangle.left };
		shiftVectorUp(m_pixels, amount, firstPixelIndex, rectangle.width);
		m_pixels[firstPixelIndex] = color;
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapUp(const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	std::vector<unsigned int> tempRow(rectangle.width);
	for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		tempRow[x] = m_pixels[rectangle.top * m_resolution.x + rectangle.left + x];
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		{
			const unsigned int index{ (rectangle.top + y) * m_resolution.x + rectangle.left + x };
			m_pixels[index] = (y == rectangle.height - 1u) ? tempRow[x] : m_pixels[index + m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapDown(const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	std::vector<unsigned int> tempRow(rectangle.width);
	for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		tempRow[x] = m_pixels[(rectangle.top + rectangle.height - 1u) * m_resolution.x + rectangle.left + x];
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		for (unsigned int x{ 0u }; x < rectangle.width; ++x)
		{
			const unsigned int index{ (rectangle.top + rectangle.height - y - 1u) * m_resolution.x + rectangle.left + x };
			m_pixels[index] = (y == rectangle.height - 1u) ? tempRow[x] : m_pixels[index - m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapLeft(const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
		shiftVectorWrapDown(m_pixels, amount, (rectangle.top + y) * m_resolution.x + rectangle.left, rectangle.width);
	priv_updatePixels();
}

void PixelDisplay::scrollWrapRight(const unsigned int amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.left < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.top < static_cast<int>(m_resolution.y));
	if (selectionRectangle.width == 0u)
		selectionRectangle.width = m_resolution.x - selectionRectangle.left;
	if (selectionRectangle.height == 0u)
		selectionRectangle.height = m_resolution.y - selectionRectangle.top;
	for (unsigned int y{ 0u }; y < static_cast<unsigned int>(selectionRectangle.height); ++y)
		shiftVectorWrapUp(m_pixels, amount, (selectionRectangle.top + y) * m_resolution.x + selectionRectangle.left, selectionRectangle.width);
	priv_updatePixels();
}

void PixelDisplay::setPalette(const std::vector<sf::Color>& colors)
{
	m_palette = colors;
	priv_updatePixels();
}

void PixelDisplay::removeColor(const unsigned int color)
{
	assert(color < m_palette.size());
	m_palette.erase(m_palette.begin() + color);
	priv_updatePixels();
}

void PixelDisplay::addRgb(const sf::Color rgb)
{
	m_palette.emplace_back(rgb);
}

void PixelDisplay::cyclePaletteDown(const unsigned int amount, const unsigned int firstColor, const unsigned int numberOfColors)
{
	assert(firstColor < m_palette.size());
	shiftVectorWrapDown(m_palette, amount, firstColor, numberOfColors);
	priv_updatePixels();
}

void PixelDisplay::cyclePaletteUp(const unsigned int amount, const unsigned int firstColor, const unsigned int numberOfColors)
{
	assert(firstColor < m_palette.size());
	shiftVectorWrapUp(m_palette, amount, firstColor, numberOfColors);
	priv_updatePixels();
}

unsigned int PixelDisplay::copy()
{
	m_buffers.push_back({ m_resolution.x, m_pixels });
	return static_cast<unsigned int>(m_buffers.size()) - 1u;
}

void PixelDisplay::copy(const unsigned int index)
{
	assert(index < m_buffers.size());
	m_buffers[index] = { m_resolution.x, m_pixels };
}

unsigned int PixelDisplay::copy(const sf::IntRect selectionRectangle)
{
	m_buffers.emplace_back();
	priv_copyToBufferFromSelectionRectangle(m_buffers.back(), selectionRectangle);
	return static_cast<unsigned int>(m_buffers.size()) - 1u;
}

void PixelDisplay::copy(const unsigned int index, const sf::IntRect selectionRectangle)
{
	assert(index < m_buffers.size());
	priv_copyToBufferFromSelectionRectangle(m_buffers[index], selectionRectangle);
}

void PixelDisplay::paste(const sf::Vector2i offset)
{
	assert(m_buffers.size() != 0u);
	priv_pasteOffsetBuffer(m_buffers.back(), offset);
}

void PixelDisplay::paste(const unsigned int index, const sf::Vector2i offset)
{
	assert(index < m_buffers.size());
	priv_pasteOffsetBuffer(m_buffers[index], offset);
}

void PixelDisplay::removeBuffer()
{
	assert(m_buffers.size() != 0u);
	m_buffers.pop_back();
}

void PixelDisplay::removeBuffer(const unsigned int index)
{
	assert(m_buffers.size() != 0u);
	m_buffers.erase(m_buffers.begin() + index);
}

unsigned int PixelDisplay::addBuffer(const sf::Vector2u size)
{
	const unsigned int newBufferIndex{ static_cast<unsigned int>(m_buffers.size()) };
	m_buffers.emplace_back();
	resizeBuffer(static_cast<unsigned int>(m_buffers.size()) - 1u, size);
	return newBufferIndex;
}

void PixelDisplay::resizeBuffer(const unsigned int index, const sf::Vector2u size)
{
	assert(index < m_buffers.size());
	Buffer& buffer{ m_buffers[index] };
	const unsigned int newNumberOfPixels{ size.x * size.y };
	if (buffer.pixels.empty())
	{
		buffer.pixels.resize(newNumberOfPixels, 0u);
		buffer.width = size.x;
		return;
	}

	const unsigned int currentBufferHeight{ static_cast<unsigned int>(buffer.pixels.size()) / buffer.width };
	if (size.x < buffer.width)
	{
		for (unsigned int i{ 0u }; i < size.x * currentBufferHeight; ++i)
		{
			const unsigned int targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
			buffer.pixels[i] = buffer.pixels[targetIndex];
		}
		buffer.pixels.resize(size.x * currentBufferHeight);
		buffer.width = size.x;
	}
	if (size.x > buffer.width)
	{
		buffer.pixels.resize(size.x * currentBufferHeight);
		for (int i{ static_cast<int>(size.x * currentBufferHeight - 1) }; i >= 0; --i)
		{
			if (i % size.x >= buffer.width)
			{
				buffer.pixels[i] = 0u;
				continue;
			}
			const unsigned int targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
			buffer.pixels[i] = buffer.pixels[targetIndex];
		}
		buffer.width = size.x;
	}
	if (size.y < currentBufferHeight)
	{
		for (unsigned int i{ 0u }; i < newNumberOfPixels; ++i)
		{
			const unsigned int targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
			buffer.pixels[i] = buffer.pixels[targetIndex];
		}
		buffer.pixels.resize(newNumberOfPixels);
	}
	if (size.y > currentBufferHeight)
	{
		buffer.pixels.resize(newNumberOfPixels);
		for (int i{ static_cast<int>(newNumberOfPixels - 1) }; i >= 0; --i)
		{
			if (i / size.x >= currentBufferHeight)
			{
				buffer.pixels[i] = 0u;
				continue;
			}
			const unsigned int targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
			buffer.pixels[i] = buffer.pixels[targetIndex];
		}
	}
}

sf::Vector2u PixelDisplay::getSizeOfBuffer(const unsigned int index) const
{
	assert(index < m_buffers.size());
	const Buffer& buffer{ m_buffers[index] };
	return{ static_cast<unsigned int>(buffer.pixels.size()) % buffer.width, static_cast<unsigned int>(buffer.pixels.size()) / buffer.width };
}








// PRIVATE

void PixelDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = nullptr;
	states.transform *= getTransform();
	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Quads, states);
}

void PixelDisplay::priv_updateVertices()
{
	m_vertices.resize(m_resolution.x * m_resolution.y * 4u);
	for (unsigned int y{ 0u }; y < m_resolution.y; ++y)
	{
		for (unsigned int x{ 0u }; x < m_resolution.x; ++x)
		{
			const sf::Vector2f topLeft{ m_size.x * x / m_resolution.x, m_size.y * y / m_resolution.y };
			const sf::Vector2f bottomRight{ m_size.x * (x + 1u) / m_resolution.x, m_size.y * (y + 1u) / m_resolution.y };
			m_vertices[(y * m_resolution.x + x) * 4u + 0u].position = topLeft;
			m_vertices[(y * m_resolution.x + x) * 4u + 1u].position = { bottomRight.x, topLeft.y };
			m_vertices[(y * m_resolution.x + x) * 4u + 2u].position = bottomRight;
			m_vertices[(y * m_resolution.x + x) * 4u + 3u].position = { topLeft.x, bottomRight.y };
		}
	}
}

void PixelDisplay::priv_updatePixels()
{
	const unsigned int numberOfPixels{ static_cast<unsigned int>(m_pixels.size()) };
	for (unsigned int i{ 0u }; i < numberOfPixels; ++i)
		priv_updatePixel(i);
}

void PixelDisplay::priv_updatePixel(const unsigned int index)
{
	const unsigned int baseVertexIndex{ index * 4u };
	const sf::Color rgb{ m_palette[m_pixels[index]] };
	m_vertices[baseVertexIndex + 0u].color = rgb;
	m_vertices[baseVertexIndex + 1u].color = rgb;
	m_vertices[baseVertexIndex + 2u].color = rgb;
	m_vertices[baseVertexIndex + 3u].color = rgb;
}

unsigned int PixelDisplay::priv_getRandomColor() const
{
	return std::uniform_int_distribution<unsigned int>(0u, static_cast<unsigned int>(m_palette.size()) - 1u)(randomGenerator);
}

void PixelDisplay::priv_copyToBufferFromSelectionRectangle(Buffer& buffer, const sf::IntRect& selectionRectangle)
{
	assert(priv_isSelectionRectangleFullyContained(selectionRectangle));
	buffer.width = selectionRectangle.width;
	buffer.pixels.resize(selectionRectangle.width * selectionRectangle.height);
	sf::Rect<unsigned int> rectangle(selectionRectangle);
	for (unsigned int y{ 0u }; y < rectangle.height; ++y)
	{
		for (unsigned int x{ 0u }; x < buffer.width; ++x)
			buffer.pixels[y * buffer.width + x] = m_pixels[x + rectangle.left, y + rectangle.top];
	}
}

void PixelDisplay::priv_pasteOffsetBuffer(const Buffer& buffer, const sf::Vector2i& offset)
{
	//const unsigned int bufferHeight{ buffer.pixels.size() / buffer.width };
	for (unsigned int i{ 0u }; i < buffer.pixels.size(); ++i)
	{
		const sf::Vector2i destination{ static_cast<int>(i % buffer.width) + offset.x, static_cast<int>(i / buffer.width) + offset.y };
		if (destination.x < 0 || destination.y < 0 || destination.x >= static_cast<int>(m_resolution.x) || destination.y >= static_cast<int>(m_resolution.y))
			continue;
		m_pixels[destination.y * m_resolution.x + destination.x] = buffer.pixels[i];
	}
	priv_updatePixels();
}

bool PixelDisplay::priv_isSelectionRectangleFullyContained(const sf::IntRect& selectionRectangle)
{
	return (selectionRectangle.left >= 0 &&
		selectionRectangle.top >= 0 &&
		selectionRectangle.width >= 0 &&
		selectionRectangle.height >= 0 &&
		static_cast<unsigned int>(selectionRectangle.left + selectionRectangle.width) <= m_resolution.x &&
		static_cast<unsigned int>(selectionRectangle.top + selectionRectangle.height) <= m_resolution.y);
}

} // namespace selbaward
