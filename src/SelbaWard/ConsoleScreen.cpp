//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Console Screen
//
// Copyright(c) 2014-2016 M.J.Silk
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

#include "ConsoleScreen.hpp"

#include <random>
#include <functional>
#include <SFML/Graphics/Texture.hpp>

namespace
{

const std::string exceptionPrefix{ "Console Screen: " };

const sf::Color defaultColor{ sf::Color::White };
const sf::Color defaultBackgroundColor{ sf::Color::Black };
const sf::Color defaultCursorColor{ sf::Color::White };

const selbaward::ConsoleScreen::Cell defaultCell{ 0u, sf::Color::White, sf::Color::Black };

std::mt19937 randomGenerator;
std::uniform_int_distribution<unsigned short int> randomDistribution(0u, 255u);
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

inline float linearInterpolation(const float start, const float end, const float alpha)
{
	return start * (1 - alpha) + end * alpha;
}

} // namespace

namespace selbaward
{

ConsoleScreen::ConsoleScreen(const sf::Vector2u numberOfCells)
	: m_cells()
	, m_numberOfCells(numberOfCells)
	, m_screenBuffers()
	, m_enabled({ true, true, true, true, true, false })
	, m_primitiveType(sf::PrimitiveType::Quads)
	, m_display()
	, m_backgroundDisplay()
	, m_size({ 100.f, 100.f })
	, m_texture(nullptr)
	, m_tileSize{ 8u, 8u }
	, m_numberOfTilesPerRow{ 8u }
	, m_colors({ defaultColor, defaultBackgroundColor, defaultCursorColor })
	, m_cursorIndex{ 0u }
{
	randomSeed();
	create(m_numberOfCells);
}

void ConsoleScreen::create(sf::Vector2u numberOfCells)
{
	if (numberOfCells.x == 0 || numberOfCells.y == 0)
		numberOfCells = { 0u, 0u };

	m_numberOfCells = numberOfCells;
	m_cells.resize(m_numberOfCells.x * m_numberOfCells.y, defaultCell);
	m_display.resize(m_cells.size() * 4);
	m_backgroundDisplay = m_display;
	m_screenBuffers.clear();

	clear();
}

void ConsoleScreen::setTexture(const sf::Texture& texture)
{
	m_texture = &texture;
}

void ConsoleScreen::setTexture(const nullptr_t nullPointer)
{
	m_texture = nullptr;
}

void ConsoleScreen::setSize(const sf::Vector2f size)
{
	m_size = size;

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::setTextureTileSize(const sf::Vector2u tileSize)
{
	m_tileSize = tileSize;

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::setNumberOfTextureTilesPerRow(const unsigned int numberOfTextureTilesPerRow)
{
	m_numberOfTilesPerRow = numberOfTextureTilesPerRow;

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::setExceptionsEnabled(const bool exceptionsEnabled)
{
	m_enabled.exceptions = exceptionsEnabled;
}

void ConsoleScreen::setAutomaticUpdateEnabled(const bool automaticUpdateEnabled)
{
	m_enabled.automaticUpdate = automaticUpdateEnabled;
}

void ConsoleScreen::setShowCursorEnabled(const bool showCursorEnabled)
{
	m_enabled.showCursor = showCursorEnabled;

	if (m_enabled.automaticUpdate)
		priv_updateCell(m_cursorIndex);
}

void ConsoleScreen::setShowBackgroundEnabled(const bool showBackgroundEnabled)
{
	m_enabled.showBackround = showBackgroundEnabled;
}

void ConsoleScreen::setScrollEnabled(const bool scrollEnabled)
{
	m_enabled.scroll = scrollEnabled;
}

void ConsoleScreen::setWrapOnManualScrollEnabled(const bool wrapOnManualScrollEnabled)
{
	m_enabled.wrapOnManualScroll = wrapOnManualScrollEnabled;
}

void ConsoleScreen::update()
{
	if (m_display.size() != (m_numberOfCells.x * m_numberOfCells.y * 4))
		return;
	if (m_backgroundDisplay.size() != m_display.size())
		m_backgroundDisplay.resize(m_display.size());

	if (m_display.size() < 4)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot update display. No cells available.");
		return;
	}

	for (unsigned int i{ 0 }; i < m_cells.size(); ++i)
		priv_updateCell(i);
}

sf::Vector2f ConsoleScreen::getSize() const
{
	return m_size;
}

sf::FloatRect ConsoleScreen::getLocalBounds() const
{
	return{ { 0.f, 0.f }, m_size };
}

sf::FloatRect ConsoleScreen::getGlobalBounds() const
{
	return getTransform().transformRect(getLocalBounds());
}

sf::Vector2u ConsoleScreen::getNumberOfCells() const
{
	return m_numberOfCells;
}

unsigned int ConsoleScreen::getTotalNumberOfCells() const
{
	return m_cells.size();
}

sf::Vector2u ConsoleScreen::getNumberOfTilesInTexture() const
{
	return{ m_numberOfTilesPerRow, m_texture->getSize().y / m_tileSize.y };
}

unsigned int ConsoleScreen::getTotalNumberOfTilesInTexture() const
{
	const sf::Vector2u numberOfTiles{ getNumberOfTilesInTexture() };
	return numberOfTiles.x * numberOfTiles.y;
}

bool ConsoleScreen::getExceptionsEnabled() const
{
	return m_enabled.exceptions;
}

bool ConsoleScreen::getAutomaticUpdateEnabled() const
{
	return m_enabled.automaticUpdate;
}

bool ConsoleScreen::getShowCursorEnabled() const
{
	return m_enabled.showCursor;
}

bool ConsoleScreen::getShowBackgroundEnabled() const
{
	return m_enabled.showBackround;
}

bool ConsoleScreen::getScrollEnabled() const
{
	return m_enabled.scroll;
}

bool ConsoleScreen::getWrapOnManualScrollEnabled() const
{
	return m_enabled.wrapOnManualScroll;
}

void ConsoleScreen::setColor(const sf::Color color)
{
	m_colors.main = color;
}

void ConsoleScreen::setBackgroundColor(const sf::Color backgroundColor)
{
	m_colors.background = backgroundColor;
}

void ConsoleScreen::setCursorColor(const sf::Color cursorColor)
{
	m_colors.cursor = cursorColor;
}

void ConsoleScreen::setColors(const sf::Color color, const sf::Color backgroundColor)
{
	setColor(color);
	setBackgroundColor(backgroundColor);
}

void ConsoleScreen::setColors(const sf::Color color, const sf::Color backgroundColor, const sf::Color cursorColor)
{
	setColors(color, backgroundColor);
	setCursorColor(cursorColor);
}

sf::Color ConsoleScreen::getColor() const
{
	return m_colors.main;
}

sf::Color ConsoleScreen::getBackgroundColor() const
{
	return m_colors.background;
}

sf::Color ConsoleScreen::getCursorColor() const
{
	return m_colors.cursor;
}

void ConsoleScreen::cursorLeft(const unsigned int distance)
{
	for (unsigned int i{ 0 }; i < distance; ++i)
		priv_moveCursorLeft();
}

void ConsoleScreen::cursorRight(const unsigned int distance)
{
	for (unsigned int i{ 0 }; i < distance; ++i)
		priv_moveCursorRight();
}

void ConsoleScreen::cursorUp(const unsigned int distance)
{
	for (unsigned int i{ 0 }; i < distance; ++i)
		priv_moveCursorUp();
}

void ConsoleScreen::cursorDown(const unsigned int distance)
{
	for (unsigned int i{ 0 }; i < distance; ++i)
		priv_moveCursorDown();
}

void ConsoleScreen::moveCursor(const sf::Vector2i offset)
{
	if (offset.x < 0)
		cursorLeft(abs(offset.x));
	else
		cursorRight(offset.x);
	if (offset.y < 0)
		cursorUp(abs(offset.y));
	else
		cursorDown(offset.y);
}

void ConsoleScreen::setCursor(const sf::Vector2u location)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set cursor.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	priv_setCursorIndex(priv_cellIndex(location));
}

sf::Vector2u ConsoleScreen::getCursor() const
{
	return priv_cellLocation(m_cursorIndex);
}

void ConsoleScreen::printChar(unsigned char character, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	if (!priv_isCursorInRange())
		return;

	const unsigned int currentIndex{ m_cursorIndex };
	sf::Color color{ m_colors.main };
	sf::Color backgroundColor{ m_colors.background };
	if (!overwriteColor)
		color = m_cells[currentIndex].color;
	if (!overwriteBackgroundColor)
		backgroundColor = m_cells[currentIndex].backgroundColor;
	poke(currentIndex, { static_cast<unsigned int>(character), color, backgroundColor });
	priv_moveCursorRight();

	if (m_enabled.automaticUpdate)
	{
		priv_updateCell(currentIndex);
		priv_updateCell(m_cursorIndex);
	}
}

void ConsoleScreen::print(const std::string& string, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	for (auto& character : string)
		printChar(character, overwriteColor, overwriteBackgroundColor);

	if (!priv_isCursorInRange())
		priv_setCursorIndex(m_cells.size() - 1);
}

void ConsoleScreen::printLine(const std::string& string, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	print(string, overwriteColor, overwriteBackgroundColor);
	nextline();
}

void ConsoleScreen::nextline()
{
	priv_moveCursorDown();
	priv_moveCursorToBeginningOfLine();

	if (m_enabled.automaticUpdate)
		priv_updateCell(m_cursorIndex);
}

void ConsoleScreen::clearCellAt(sf::Vector2u location)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot clear cell.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	priv_clearCell(priv_cellIndex(location), m_colors.main, m_colors.background);

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::setCellAt(const sf::Vector2u location, const Cell& cell)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set cell.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	m_cells[priv_cellIndex(location)] = cell;

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::printAt(const sf::Vector2u location, const std::string& string, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	if (string.size() == 0)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot print.\nString does not exist.");
		return;
	}

	for (unsigned int i{ 0 }; i < string.size(); ++i)
		printCharAt({ location.x + i, location.y }, string[i], overwriteColor, overwriteBackgroundColor);
}

void ConsoleScreen::printCharAt(sf::Vector2u location, const unsigned char character, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	unsigned int index{ priv_getPrintIndex(location) };
	m_cells[index].value = static_cast<unsigned int>(character);
	if (overwriteColor)
		m_cells[index].color = m_colors.main;
	if (overwriteBackgroundColor)
		m_cells[index].backgroundColor = m_colors.background;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::printAt(const sf::Vector2u location, const std::string& string, const sf::Color color, const bool overwriteBackgroundColor)
{
	if (string.size() == 0)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot print.\nString does not exist.");
		return;
	}

	for (unsigned int i{ 0 }; i < string.size(); ++i)
		printCharAt({ location.x + i, location.y }, string[i], color, overwriteBackgroundColor);
}

void ConsoleScreen::printCharAt(sf::Vector2u location, const unsigned char character, const sf::Color color, const bool overwriteBackgroundColor)
{
	unsigned int index{ priv_getPrintIndex(location) };
	m_cells[index].value = static_cast<unsigned int>(character);
	m_cells[index].color = color;
	if (overwriteBackgroundColor)
		m_cells[index].backgroundColor = m_colors.background;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::printAt(const sf::Vector2u location, const std::string& string, const sf::Color color, const sf::Color backgroundColor)
{
	if (string.size() == 0)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot print.\nString does not exist.");
		return;
	}

	for (unsigned int i{ 0 }; i < string.size(); ++i)
		printCharAt({ location.x + i, location.y }, string[i], color, backgroundColor);
}

void ConsoleScreen::printCharAt(sf::Vector2u location, const unsigned char character, const sf::Color color, const sf::Color backgroundColor)
{
	unsigned int index{ priv_getPrintIndex(location) };
	m_cells[index].value = static_cast<unsigned int>(character);
	m_cells[index].color = color;
	m_cells[index].backgroundColor = backgroundColor;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::setValueAt(const sf::Vector2u location, const unsigned int value)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set value.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	m_cells[priv_cellIndex(location)].value = value;

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::setColorsAt(const sf::Vector2u location, const sf::Color color, const sf::Color backgroundColor)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set colors.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	m_cells[priv_cellIndex(location)].color = color;
	m_cells[priv_cellIndex(location)].backgroundColor = backgroundColor;

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::setColorAt(const sf::Vector2u location, const sf::Color color)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set color.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	m_cells[priv_cellIndex(location)].color = color;

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::setBackgroundColorAt(const sf::Vector2u location, const sf::Color backgroundColor)
{
	if (!priv_isCellLocationInRange(location))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot set background colors.\nLocation (" + std::to_string(location.x) + ", " + std::to_string(location.y) + ") out of range.");
		return;
	}

	m_cells[priv_cellIndex(location)].backgroundColor = backgroundColor;

	if (m_enabled.automaticUpdate)
		priv_updateCell(priv_cellIndex(location));
}

void ConsoleScreen::scrollUp(const unsigned int amount)
{
	if (m_numberOfCells.y == 0 || amount == 0)
		return;

	std::vector<Cell> topRow(m_numberOfCells.x);
	for (unsigned int repeat{ 0 }; repeat < amount; ++repeat) // lazy way of scrolling multiple times - loop scrolling (entirely by 1 each time)
	{
		for (unsigned int y{ 0 }; y < m_numberOfCells.y; ++y)
		{
			for (unsigned int x{ 0 }; x < m_numberOfCells.x; ++x)
			{
				if (m_enabled.wrapOnManualScroll && y == 0)
					topRow[x] = m_cells[x];
				if (y < m_numberOfCells.y - 1)
					m_cells[priv_cellIndex({ x, y })] = m_cells[priv_cellIndex({ x, y + 1 })];
				else if (m_enabled.wrapOnManualScroll)
					m_cells[priv_cellIndex({ x, y })] = topRow[x];
				else
					priv_clearCell(priv_cellIndex({ x, y }), true, true);
			}
		}
	}

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::scrollDown(const unsigned int amount)
{
	if (m_numberOfCells.y == 0 || amount == 0)
		return;

	std::vector<Cell> bottomRow(m_numberOfCells.x);
	for (unsigned int repeat{ 0 }; repeat < amount; ++repeat) // lazy way of scrolling multiple times - loop scrolling (entirely by 1 each time)
	{
		for (unsigned int y{ 0 }; y < m_numberOfCells.y; ++y)
		{
			for (unsigned int x{ 0 }; x < m_numberOfCells.x; ++x)
			{
				const unsigned cellY{ m_numberOfCells.y - y - 1 };
				if (m_enabled.wrapOnManualScroll && y == 0)
					bottomRow[x] = m_cells[priv_cellIndex({ x, cellY })];
				if (cellY > 0)
					m_cells[priv_cellIndex({ x, cellY })] = m_cells[priv_cellIndex({ x, cellY - 1 })];
				else if (m_enabled.wrapOnManualScroll)
					m_cells[priv_cellIndex({ x, cellY })] = bottomRow[x];
				else
					priv_clearCell(priv_cellIndex({ x, cellY }), true, true);
			}
		}
	}

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::scrollLeft(const unsigned int amount)
{
	if (m_numberOfCells.x == 0 || amount == 0)
		return;

	Cell leftCell;
	for (unsigned int repeat{ 0 }; repeat < amount; ++repeat) // lazy way of scrolling multiple times - loop scrolling (entirely by 1 each time)
	{
		for (unsigned int cell{ 0 }; cell < m_cells.size(); ++cell)
		{
			if (cell % m_numberOfCells.x == 0)
				leftCell = m_cells[cell];
			if (cell % m_numberOfCells.x == m_numberOfCells.x - 1)
			{
				if (m_enabled.wrapOnManualScroll)
					m_cells[cell] = leftCell;
				else
					priv_clearCell(cell, true, true);
			}
			else
				m_cells[cell] = m_cells[cell + 1];
		}
	}

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::scrollRight(const unsigned int amount)
{
	if (m_numberOfCells.x == 0 || amount == 0)
		return;

	Cell rightCell;
	for (unsigned int repeat{ 0 }; repeat < amount; ++repeat) // lazy way of scrolling multiple times - loop scrolling (entirely by 1 each time)
	{
		for (unsigned int i{ 0 }; i < m_cells.size(); ++i)
		{
			const unsigned int cell{ m_cells.size() - i - 1 };
			if (i % m_numberOfCells.x == 0)
				rightCell = m_cells[cell];
			if (cell % m_numberOfCells.x == 0)
			{
				if (m_enabled.wrapOnManualScroll)
					m_cells[cell] = rightCell;
				else
					priv_clearCell(cell, true, true);
			}
			else
				m_cells[cell] = m_cells[cell - 1];
		}
	}

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::clear()
{
	clear(m_colors.background);
}

void ConsoleScreen::clear(const sf::Color backgroundColor)
{
	if (m_cells.size() == 0)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot clear cell.\nNo cells exist.");
		return;
	}

	for (unsigned int i{ 0 }; i < m_cells.size(); ++i)
		priv_clearCell(i, m_colors.main, backgroundColor);

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::crash()
{
	for (auto& cell : m_cells)
	{
		cell = { randomByte(), sf::Color(randomByte(), randomByte(), randomByte()), sf::Color(randomByte(), randomByte(), randomByte()) };
	}

	if (m_enabled.automaticUpdate)
		update();
}

void ConsoleScreen::poke(const unsigned int index, const Cell& cell)
{
	if (!priv_isCellIndexInRange(index))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot poke cell.\nCell number (" + std::to_string(index) + ") out of range.");
		return;
	}

	m_cells[index] = cell;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::poke(const unsigned int index, const unsigned int value)
{
	if (!priv_isCellIndexInRange(index))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot poke value.\nCell number (" + std::to_string(index) + ") out of range.");
		return;
	}

	m_cells[index].value = value;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::poke(const unsigned int index, const sf::Color& color)
{
	if (!priv_isCellIndexInRange(index))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot poke color.\nCell number (" + std::to_string(index) + ") out of range.");
		return;
	}

	m_cells[index].color = color;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

void ConsoleScreen::poke(const unsigned int index, const sf::Color& color, const sf::Color& backgroundColor)
{
	if (!priv_isCellIndexInRange(index))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot poke colors.\nCell number (" + std::to_string(index) + ") out of range.");
		return;
	}

	m_cells[index].color = color;
	m_cells[index].backgroundColor = backgroundColor;

	if (m_enabled.automaticUpdate)
		priv_updateCell(index);
}

ConsoleScreen::Cell ConsoleScreen::peek(const unsigned int index) const
{
	if (!priv_isCellIndexInRange(index))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Cannot peek cell.\nCell number (" + std::to_string(index) + ") out of range.");
		return Cell();
	}

	return m_cells[index];
}



// PRIVATE

void ConsoleScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform = getTransform();

	if (m_enabled.showBackround && m_backgroundDisplay.size() > 0)
	{
		states.texture = nullptr;
		target.draw(&m_backgroundDisplay.front(), m_backgroundDisplay.size(), m_primitiveType, states);
	}
	if (m_display.size() > 0)
	{
		states.texture = m_texture;
		target.draw(&m_display.front(), m_display.size(), m_primitiveType, states);
	}
}

void ConsoleScreen::priv_updateCell(const unsigned int index)
{
	if (!priv_isCellIndexInRange(index))
		return;

	if (m_display.size() != (m_numberOfCells.x * m_numberOfCells.y * 4))
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Bug: display does not match cells.");
		return;
	}

	if (m_display.size() < 4)
	{
		if (m_enabled.exceptions)
			throw Exception(exceptionPrefix + "Bug: cannot update cell. No cells available.");
		return;
	}

	// cursorCellValue is which character texture to use for cursor
	// 219 is solid block
	// 220 is solid, thick underline
	// 254 smaller solid block
	// 95/'_' is underline
	// 248-250 are small central circles
	const unsigned int cursorCellValue{ '_' };

	unsigned int cellValue{ m_cells[index].value };
	sf::Color cellColor{ m_cells[index].color };
	const sf::Color backgroundColor{ m_cells[index].backgroundColor };

	if (m_enabled.showCursor && m_cursorIndex == index)
	{
		cellValue = cursorCellValue;
		cellColor = m_colors.cursor;
	}

	const unsigned int cellX{ index % m_numberOfCells.x };
	const unsigned int cellY{ index / m_numberOfCells.x };
	const float left{ linearInterpolation(0.f, m_size.x, static_cast<float>(cellX) / m_numberOfCells.x) };
	const float right{ linearInterpolation(0.f, m_size.x, static_cast<float>(cellX + 1) / m_numberOfCells.x) };
	const float top{ linearInterpolation(0.f, m_size.y, static_cast<float>(cellY) / m_numberOfCells.y) };
	const float bottom{ linearInterpolation(0.f, m_size.y, static_cast<float>(cellY + 1) / m_numberOfCells.y) };

	sf::Vector2u textureCell{ cellValue % m_numberOfTilesPerRow, cellValue / m_numberOfTilesPerRow };
	const float textureLeft{ static_cast<float>(textureCell.x * m_tileSize.x) };
	const float textureTop{ static_cast<float>(textureCell.y * m_tileSize.y) };
	const float textureRight{ static_cast<float>((textureCell.x + 1) * m_tileSize.x) };
	const float textureBottom{ static_cast<float>((textureCell.y + 1) * m_tileSize.y) };

	const unsigned int baseVertex{ index * 4 };
	sf::Vertex* const pVertex1{ &m_display[baseVertex] };
	sf::Vertex* const pVertex2{ pVertex1 + 1 };
	sf::Vertex* const pVertex3{ pVertex1 + 2 };
	sf::Vertex* const pVertex4{ pVertex1 + 3 };
	sf::Vertex* const pBackgroundVertex1{ &m_backgroundDisplay[baseVertex] };
	sf::Vertex* const pBackgroundVertex2{ pBackgroundVertex1 + 1 };
	sf::Vertex* const pBackgroundVertex3{ pBackgroundVertex1 + 2 };
	sf::Vertex* const pBackgroundVertex4{ pBackgroundVertex1 + 3 };

	// foreground display
	pVertex1->position = { left, top };
	pVertex2->position = { right, top };
	pVertex3->position = { right, bottom };
	pVertex4->position = { left, bottom };

	pVertex1->texCoords = { textureLeft, textureTop };
	pVertex2->texCoords = { textureRight, textureTop };
	pVertex3->texCoords = { textureRight, textureBottom };
	pVertex4->texCoords = { textureLeft, textureBottom };

	pVertex1->color = cellColor;
	pVertex2->color = cellColor;
	pVertex3->color = cellColor;
	pVertex4->color = cellColor;

	// background display
	pBackgroundVertex1->position = pVertex1->position;
	pBackgroundVertex2->position = pVertex2->position;
	pBackgroundVertex3->position = pVertex3->position;
	pBackgroundVertex4->position = pVertex4->position;

	pBackgroundVertex1->color = backgroundColor;
	pBackgroundVertex2->color = backgroundColor;
	pBackgroundVertex3->color = backgroundColor;
	pBackgroundVertex4->color = backgroundColor;
}

inline unsigned int ConsoleScreen::priv_cellIndex(const sf::Vector2u location) const
{
	return location.y * m_numberOfCells.x + location.x;
}

inline sf::Vector2u ConsoleScreen::priv_cellLocation(const unsigned int index) const
{
	return{ index % m_numberOfCells.x, index / m_numberOfCells.x };
}

inline bool ConsoleScreen::priv_isCellIndexInRange(const unsigned int index) const
{
	return index < m_cells.size();
}

inline bool ConsoleScreen::priv_isCellLocationInRange(const sf::Vector2u location) const
{
	return (location.x < m_numberOfCells.x && location.y < m_numberOfCells.y);
}

inline bool ConsoleScreen::priv_isCursorInRange() const
{
	return priv_isCellIndexInRange(m_cursorIndex);
}

inline void ConsoleScreen::priv_clearCell(const unsigned int index, const bool overwriteColor, const bool overwriteBackgroundColor)
{
	const sf::Color color{ overwriteColor ? m_colors.main : m_cells[index].color };
	const sf::Color backgroundColor{ overwriteBackgroundColor ? m_colors.background : m_cells[index].backgroundColor };
	poke(index, { 0, color, backgroundColor });
}

inline void ConsoleScreen::priv_clearCell(const unsigned int index, const sf::Color backgroundColor, const bool overwriteColor)
{
	const sf::Color color{ overwriteColor ? m_colors.main : m_cells[index].color };
	poke(index, { 0, color, backgroundColor });
}

inline void ConsoleScreen::priv_clearCell(const unsigned int index, const sf::Color color, const sf::Color backgroundColor)
{
	poke(index, { 0, color, backgroundColor });
}

void ConsoleScreen::priv_setCursorIndex(const unsigned int index)
{
	const unsigned int previousIndex{ m_cursorIndex };
	m_cursorIndex = index;

	if (m_enabled.automaticUpdate)
	{
		priv_updateCell(previousIndex);
		priv_updateCell(m_cursorIndex);
	}
}

void ConsoleScreen::priv_moveCursorToBeginningOfLine()
{
	priv_setCursorIndex(m_cursorIndex - m_cursorIndex % m_numberOfCells.x);
}

void ConsoleScreen::priv_moveCursorUp()
{
	if (m_cursorIndex >= m_numberOfCells.x)
		priv_setCursorIndex(m_cursorIndex - m_numberOfCells.x);
}

void ConsoleScreen::priv_moveCursorDown()
{
	priv_setCursorIndex(m_cursorIndex + m_numberOfCells.x);
	priv_testCursorForScroll();
}

void ConsoleScreen::priv_moveCursorLeft()
{
	if (m_cursorIndex > 0)
		priv_setCursorIndex(m_cursorIndex - 1);
}

void ConsoleScreen::priv_moveCursorRight()
{
	priv_setCursorIndex(m_cursorIndex + 1);
	priv_testCursorForScroll();
}

void ConsoleScreen::priv_testCursorForScroll()
{
	if (m_cursorIndex >= m_cells.size())
	{
		if (m_enabled.scroll)
		{
			priv_scroll();
			if (m_enabled.automaticUpdate)
				update();
		}
		else
		{
			m_cursorIndex = m_cells.size() - 1;
			if (m_enabled.automaticUpdate)
				priv_updateCell(m_cursorIndex);
		}
	}
}

void ConsoleScreen::priv_scroll()
{
	for (unsigned int y{ 0 }; y < m_numberOfCells.y; ++y)
	{
		for (unsigned int x{ 0 }; x < m_numberOfCells.x; ++x)
		{
			if (y < m_numberOfCells.y - 1)
				m_cells[priv_cellIndex({ x, y })] = m_cells[priv_cellIndex({ x, y + 1 })];
			else
				priv_clearCell(priv_cellIndex({ x, y }), true, true);
		}
	}
	priv_moveCursorUp();
}

unsigned int ConsoleScreen::priv_getPrintIndex(sf::Vector2u location) const
{
	if (location.x >= m_numberOfCells.x)
	{
		location.y += location.x / m_numberOfCells.x;
		location.x %= m_numberOfCells.x;
	}
	if (location.y >= m_numberOfCells.y)
	{
		location.y = m_numberOfCells.y - 1;
		//location.x = m_numberOfCells.x - 1; // push cursor to the end of the line if cursor was below the last line
	}

	return priv_cellIndex(location);
}

} // namespace selbaward
