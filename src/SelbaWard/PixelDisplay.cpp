//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Pixel Display
//
// Copyright(c) 2019-2026 M.J.Silk
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
void shiftVectorDown(std::vector<T>& vector, const std::size_t amount, const std::size_t startIndex, std::size_t numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = vector.size() - startIndex;
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + i] = vector[startIndex + i + amount];
}

template <class T>
void shiftVectorUp(std::vector<T>& vector, const std::size_t amount, const std::size_t startIndex, std::size_t numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = vector.size() - startIndex;
	for (unsigned int i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + numberOfElements - i - 1u] = vector[startIndex + numberOfElements - (i + amount) - 1u];
}

template <class T>
void shiftVectorWrapDown(std::vector<T>& vector, const std::size_t amount, const std::size_t startIndex, std::size_t numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = vector.size() - startIndex;
	std::vector<T> tempPixels(amount);
	for (std::size_t i{ 0u }; i < amount; ++i)
		tempPixels[i] = vector[startIndex + i];
	for (std::size_t i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + i] = vector[startIndex + i + amount];
	for (std::size_t i{ 0u }; i < amount; ++i)
		vector[startIndex + numberOfElements - amount + i] = tempPixels[i];
}

template <class T>
void shiftVectorWrapUp(std::vector<T>& vector, const std::size_t amount, const std::size_t startIndex, std::size_t numberOfElements)
{
	if ((numberOfElements == 0u) || (startIndex + numberOfElements > vector.size()))
		numberOfElements = vector.size() - startIndex;
	std::vector<T> tempPixels(amount);
	for (std::size_t i{ 0u }; i < amount; ++i)
		tempPixels[i] = vector[startIndex + numberOfElements - amount + i];
	for (std::size_t i{ 0u }; i < numberOfElements - amount; ++i)
		vector[startIndex + numberOfElements - i - 1u] = vector[startIndex + numberOfElements - (i + amount) - 1u];
	for (std::size_t i{ 0u }; i < amount; ++i)
		vector[startIndex + i] = tempPixels[i];
}

} // namespace

namespace selbaward
{

PixelDisplay::PixelDisplay()
	: m_vertices{}
	, m_resolution{ 320u, 240u }
	, m_size{ 640.f, 480.f }
	, m_palette(16u)
	, m_pixels{}
	, m_buffers{}
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

void PixelDisplay::setResolution(const sf::Vector2<std::size_t> resolution)
{
	m_resolution = resolution;
	m_pixels.resize(m_resolution.x * m_resolution.y);
	priv_updateVertices();
}

void PixelDisplay::setPaletteSize(const std::size_t numberOfColors)
{
	m_palette.resize(numberOfColors);
}

void PixelDisplay::setColor(const std::size_t color, const sf::Color rgb)
{
	assert(color < m_palette.size());
	m_palette[color] = rgb;
	priv_updatePixels();
}

void PixelDisplay::setPixel(const std::size_t index, const std::size_t color)
{
	assert(index < m_pixels.size());
	m_pixels[index] = color;
	priv_updatePixel(index);
}

void PixelDisplay::setPixels(const char* colors, const std::size_t numberOfColors, const std::size_t startIndex)
{
	assert(startIndex < m_pixels.size());
	setPixels(reinterpret_cast<const unsigned char*>(colors), numberOfColors, startIndex);
}

void PixelDisplay::getPixels(char* colors, const std::size_t numberOfColors, const std::size_t startIndex) const
{
	assert(startIndex < m_pixels.size());
	getPixels(reinterpret_cast<unsigned char*>(colors), numberOfColors, startIndex);
}

void PixelDisplay::fill(const std::size_t color)
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

void PixelDisplay::shiftLeft(const std::size_t amount, const std::size_t startIndex, const std::size_t numberOfPixels)
{
	assert(startIndex < m_pixels.size());
	shiftVectorWrapDown(m_pixels, amount, startIndex, numberOfPixels);
	priv_updatePixels();
}

void PixelDisplay::shiftRight(const std::size_t amount, const std::size_t startIndex, const std::size_t numberOfPixels)
{
	assert(startIndex < m_pixels.size());
	shiftVectorWrapUp(m_pixels, amount, startIndex, numberOfPixels);
	priv_updatePixels();
}

void PixelDisplay::scrollUp(const std::size_t color, const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		{
			const std::size_t index{ (rectangle.position.y + y) * m_resolution.x + rectangle.position.x + x };
			m_pixels[index] = (y == rectangle.size.y - 1u) ? color : m_pixels[index + m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollDown(const std::size_t color, const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		{
			const std::size_t index{ (rectangle.position.y + rectangle.size.y - y - 1u) * m_resolution.x + rectangle.position.x + x };
			m_pixels[index] = (y == rectangle.size.y - 1u) ? color : m_pixels[index - m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollLeft(const std::size_t color, const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		const std::size_t firstPixelIndex{ (rectangle.position.y + y) * m_resolution.x + rectangle.position.x };
		shiftVectorDown(m_pixels, amount, firstPixelIndex, rectangle.size.x);
		m_pixels[firstPixelIndex + rectangle.size.x - 1u] = color;
	}
	priv_updatePixels();
}

void PixelDisplay::scrollRight(const std::size_t color, const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(color < m_palette.size());
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		const std::size_t firstPixelIndex{ (rectangle.position.y + y) * m_resolution.x + rectangle.position.x };
		shiftVectorUp(m_pixels, amount, firstPixelIndex, rectangle.size.x);
		m_pixels[firstPixelIndex] = color;
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapUp(const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	std::vector<std::size_t> tempRow(rectangle.size.x);
	for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		tempRow[x] = m_pixels[rectangle.position.y * m_resolution.x + rectangle.position.x + x];
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		{
			const std::size_t index{ (rectangle.position.y + y) * m_resolution.x + rectangle.position.x + x };
			m_pixels[index] = (y == rectangle.size.y - 1u) ? tempRow[x] : m_pixels[index + m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapDown(const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	std::vector<std::size_t> tempRow(rectangle.size.x);
	for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		tempRow[x] = m_pixels[(rectangle.position.y + rectangle.size.y - 1u) * m_resolution.x + rectangle.position.x + x];
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < rectangle.size.x; ++x)
		{
			const std::size_t index{ (rectangle.position.y + rectangle.size.y - y - 1u) * m_resolution.x + rectangle.position.x + x };
			m_pixels[index] = (y == rectangle.size.y - 1u) ? tempRow[x] : m_pixels[index - m_resolution.x];
		}
	}
	priv_updatePixels();
}

void PixelDisplay::scrollWrapLeft(const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
		shiftVectorWrapDown(m_pixels, amount, (rectangle.position.y + y) * m_resolution.x + rectangle.position.x, rectangle.size.x);
	priv_updatePixels();
}

void PixelDisplay::scrollWrapRight(const std::size_t amount, sf::IntRect selectionRectangle)
{
	assert(selectionRectangle.position.x < static_cast<int>(m_resolution.x));
	assert(selectionRectangle.position.y < static_cast<int>(m_resolution.y));
	if (selectionRectangle.size.x == 0u)
		selectionRectangle.size.x = static_cast<int>(m_resolution.x - selectionRectangle.position.x);
	if (selectionRectangle.size.y == 0u)
		selectionRectangle.size.y = static_cast<int>(m_resolution.y - selectionRectangle.position.y);
	for (std::size_t y{ 0u }; y < static_cast<std::size_t>(selectionRectangle.size.y); ++y)
		shiftVectorWrapUp(m_pixels, amount, (selectionRectangle.position.y + y) * m_resolution.x + selectionRectangle.position.x, selectionRectangle.size.x);
	priv_updatePixels();
}

void PixelDisplay::setPalette(const std::vector<sf::Color>& colors)
{
	m_palette = colors;
	priv_updatePixels();
}

void PixelDisplay::removeColor(const std::size_t color)
{
	assert(color < m_palette.size());
	m_palette.erase(m_palette.begin() + color);
	priv_updatePixels();
}

void PixelDisplay::addRgb(const sf::Color rgb)
{
	m_palette.emplace_back(rgb);
}

void PixelDisplay::cyclePaletteDown(const std::size_t amount, const std::size_t firstColor, const std::size_t numberOfColors)
{
	assert(firstColor < m_palette.size());
	shiftVectorWrapDown(m_palette, amount, firstColor, numberOfColors);
	priv_updatePixels();
}

void PixelDisplay::cyclePaletteUp(const std::size_t amount, const std::size_t firstColor, const std::size_t numberOfColors)
{
	assert(firstColor < m_palette.size());
	shiftVectorWrapUp(m_palette, amount, firstColor, numberOfColors);
	priv_updatePixels();
}

std::size_t PixelDisplay::copy()
{
	m_buffers.push_back({ m_resolution.x, m_pixels });
	return m_buffers.size() - 1u;
}

void PixelDisplay::copy(const std::size_t index)
{
	assert(index < m_buffers.size());
	m_buffers[index] = { m_resolution.x, m_pixels };
}

std::size_t PixelDisplay::copy(const sf::IntRect selectionRectangle)
{
	m_buffers.emplace_back();
	priv_copyToBufferFromSelectionRectangle(m_buffers.back(), selectionRectangle);
	return m_buffers.size() - 1u;
}

void PixelDisplay::copy(const std::size_t index, const sf::IntRect selectionRectangle)
{
	assert(index < m_buffers.size());
	priv_copyToBufferFromSelectionRectangle(m_buffers[index], selectionRectangle);
}

void PixelDisplay::paste(const sf::Vector2i offset)
{
	assert(m_buffers.size() != 0u);
	priv_pasteOffsetBuffer(m_buffers.back(), offset);
}

void PixelDisplay::paste(const std::size_t index, const sf::Vector2i offset)
{
	assert(index < m_buffers.size());
	priv_pasteOffsetBuffer(m_buffers[index], offset);
}

void PixelDisplay::removeBuffer()
{
	assert(m_buffers.size() != 0u);
	m_buffers.pop_back();
}

void PixelDisplay::removeBuffer(const std::size_t index)
{
	assert(m_buffers.size() != 0u);
	m_buffers.erase(m_buffers.begin() + index);
}

std::size_t PixelDisplay::addBuffer(const sf::Vector2<std::size_t> size)
{
	const std::size_t newBufferIndex{ m_buffers.size() };
	m_buffers.emplace_back();
	resizeBuffer(m_buffers.size() - 1u, size);
	return newBufferIndex;
}

void PixelDisplay::resizeBuffer(const std::size_t index, const sf::Vector2<std::size_t> size)
{
	assert(index < m_buffers.size());
	Buffer& buffer{ m_buffers[index] };
	const std::size_t newNumberOfPixels{ size.x * size.y };
	if (buffer.pixels.empty())
	{
		buffer.pixels.resize(newNumberOfPixels, 0u);
		buffer.width = size.x;
		return;
	}

	const std::size_t currentBufferHeight{ buffer.pixels.size() / buffer.width };
	if (size.x < buffer.width)
	{
		for (std::size_t i{ 0u }; i < size.x * currentBufferHeight; ++i)
		{
			const std::size_t targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
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
				buffer.pixels[static_cast<std::size_t>(i)] = 0u;
				continue;
			}
			const std::size_t targetIndex{ static_cast<std::size_t>((i / size.x) * buffer.width + (i % size.x)) };
			buffer.pixels[static_cast<std::size_t>(i)] = buffer.pixels[targetIndex];
		}
		buffer.width = size.x;
	}
	if (size.y < currentBufferHeight)
	{
		for (std::size_t i{ 0u }; i < newNumberOfPixels; ++i)
		{
			const std::size_t targetIndex{ (i / size.x) * buffer.width + (i % size.x) };
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
				buffer.pixels[static_cast<std::size_t>(i)] = 0u;
				continue;
			}
			const std::size_t targetIndex{ static_cast<std::size_t>((i / size.x) * buffer.width + (i % size.x)) };
			buffer.pixels[static_cast<std::size_t>(i)] = buffer.pixels[targetIndex];
		}
	}
}

sf::Vector2<std::size_t> PixelDisplay::getSizeOfBuffer(const std::size_t index) const
{
	assert(index < m_buffers.size());
	const Buffer& buffer{ m_buffers[index] };
	return{ buffer.pixels.size() % buffer.width, buffer.pixels.size() / buffer.width };
}








// PRIVATE

void PixelDisplay::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.texture = nullptr;
	states.transform *= getTransform();
	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Triangles, states);
}

void PixelDisplay::priv_updateVertices()
{
	m_vertices.resize(m_resolution.x * m_resolution.y * 6u);
	for (std::size_t y{ 0u }; y < m_resolution.y; ++y)
	{
		for (std::size_t x{ 0u }; x < m_resolution.x; ++x)
		{
			const sf::Vector2f topLeft{ m_size.x * x / m_resolution.x, m_size.y * y / m_resolution.y };
			const sf::Vector2f bottomRight{ m_size.x * (x + 1u) / m_resolution.x, m_size.y * (y + 1u) / m_resolution.y };
			m_vertices[((y * m_resolution.x + x) * 6u) + 0u].position = topLeft;
			m_vertices[((y * m_resolution.x + x) * 6u) + 1u].position = { topLeft.x, bottomRight.y };
			m_vertices[((y * m_resolution.x + x) * 6u) + 2u].position = { bottomRight.x, topLeft.y };
			m_vertices[((y * m_resolution.x + x) * 6u) + 3u].position = bottomRight;
			m_vertices[((y * m_resolution.x + x) * 6u) + 4u].position = m_vertices[((y * m_resolution.x + x) * 6u) + 2u].position;
			m_vertices[((y * m_resolution.x + x) * 6u) + 5u].position = m_vertices[((y * m_resolution.x + x) * 6u) + 1u].position;
		}
	}
}

void PixelDisplay::priv_updatePixels()
{
	const std::size_t numberOfPixels{ m_pixels.size() };
	for (std::size_t i{ 0u }; i < numberOfPixels; ++i)
		priv_updatePixel(i);
}

void PixelDisplay::priv_updatePixel(const std::size_t index)
{
	const std::size_t baseVertexIndex{ index * 6u };
	const sf::Color rgb{ m_palette[m_pixels[index]] };
	m_vertices[baseVertexIndex + 0u].color = rgb;
	m_vertices[baseVertexIndex + 1u].color = rgb;
	m_vertices[baseVertexIndex + 2u].color = rgb;
	m_vertices[baseVertexIndex + 3u].color = rgb;
	m_vertices[baseVertexIndex + 4u].color = rgb;
	m_vertices[baseVertexIndex + 5u].color = rgb;
}

std::size_t PixelDisplay::priv_getRandomColor() const
{
	return std::uniform_int_distribution<std::size_t>(0u, m_palette.size() - 1u)(randomGenerator);
}

void PixelDisplay::priv_copyToBufferFromSelectionRectangle(Buffer& buffer, const sf::IntRect& selectionRectangle)
{
	assert(priv_isSelectionRectangleFullyContained(selectionRectangle));
	buffer.width = selectionRectangle.size.x;
	buffer.pixels.resize(static_cast<std::size_t>(selectionRectangle.size.x * selectionRectangle.size.y));
	sf::Rect<std::size_t> rectangle(selectionRectangle);
	for (std::size_t y{ 0u }; y < rectangle.size.y; ++y)
	{
		for (std::size_t x{ 0u }; x < buffer.width; ++x)
			buffer.pixels[y * buffer.width + x] = m_pixels[(y + rectangle.position.y) * buffer.width + (x + rectangle.position.x)];
	}
}

void PixelDisplay::priv_pasteOffsetBuffer(const Buffer& buffer, const sf::Vector2i& offset)
{
	//const unsigned int bufferHeight{ buffer.pixels.size() / buffer.width };
	for (std::size_t i{ 0u }; i < buffer.pixels.size(); ++i)
	{
		const sf::Vector2i destination{ static_cast<int>(i % buffer.width) + offset.x, static_cast<int>(i / buffer.width) + offset.y };
		if (destination.x < 0 || destination.y < 0 || destination.x >= static_cast<int>(m_resolution.x) || destination.y >= static_cast<int>(m_resolution.y))
			continue;
		m_pixels[static_cast<std::size_t>(destination.y * m_resolution.x + destination.x)] = buffer.pixels[i];
	}
	priv_updatePixels();
}

bool PixelDisplay::priv_isSelectionRectangleFullyContained(const sf::IntRect& selectionRectangle) const
{
	return (selectionRectangle.position.x >= 0 &&
		selectionRectangle.position.y >= 0 &&
		selectionRectangle.size.x >= 0 &&
		selectionRectangle.size.y >= 0 &&
		static_cast<std::size_t>(selectionRectangle.position.x + selectionRectangle.size.x) <= m_resolution.x &&
		static_cast<std::size_t>(selectionRectangle.position.y + selectionRectangle.size.y) <= m_resolution.y);
}

} // namespace selbaward
