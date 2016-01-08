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

#ifndef SELBAWARD_CONSOLESCREEN_HPP
#define SELBAWARD_CONSOLESCREEN_HPP

#include "Common.hpp"

namespace selbaward
{

// SW Console Screen v1.0.0
class ConsoleScreen : public sf::Drawable, public sf::Transformable
{
public:
	struct Cell;

	// setup
	ConsoleScreen(sf::Vector2u numberOfCells = { 80u, 45u });
	void create(sf::Vector2u numberOfCells);
	void setTexture(const sf::Texture& texture);
	void setTexture(nullptr_t nullPointer);
	void setTextureTileSize(sf::Vector2u tileSize = { 8u, 8u });
	void setNumberOfTextureTilesPerRow(unsigned int numberOfTextureTilesPerRow = 1u);
	sf::Vector2u getNumberOfCells() const;
	unsigned int getTotalNumberOfCells() const;
	sf::Vector2u getNumberOfTilesInTexture() const;
	unsigned int getTotalNumberOfTilesInTexture() const;

	// switches
	void setExceptionsEnabled(bool exceptionsEnabled = true);
	void setAutomaticUpdateEnabled(bool automaticUpdateEnabled = true);
	void setShowCursorEnabled(bool showCursorEnabled = true);
	void setShowBackgroundEnabled(bool showBackgroundEnabled = true);
	void setScrollEnabled(bool scrollEnabled = true);
	void setWrapOnManualScrollEnabled(bool wrapOnManualScrollEnabled = true);
	bool getExceptionsEnabled() const;
	bool getAutomaticUpdateEnabled() const;
	bool getShowCursorEnabled() const;
	bool getShowBackgroundEnabled() const;
	bool getScrollEnabled() const;
	bool getWrapOnManualScrollEnabled() const;

	// visual representation
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	// global
	void update();
	void clear();
	void clear(sf::Color backgroundColor);
	void crash();

	// current colours
	void setColor(sf::Color color);
	void setBackgroundColor(sf::Color backgroundColor);
	void setCursorColor(sf::Color cursorColor);
	void setColors(sf::Color color, sf::Color backgroundColor);
	void setColors(sf::Color color, sf::Color backgroundColor, sf::Color cursorColor);
	sf::Color getColor() const;
	sf::Color getBackgroundColor() const;
	sf::Color getCursorColor() const;

	// cursor
	void cursorLeft(unsigned int distance = 1);
	void cursorRight(unsigned int distance = 1);
	void cursorUp(unsigned int distance = 1);
	void cursorDown(unsigned int distance = 1);
	void moveCursor(sf::Vector2i offset);
	void setCursor(sf::Vector2u location);
	sf::Vector2u getCursor() const;

	// printing (using cursor)
	void printChar(unsigned char character, bool overwriteColor = true, bool overwriteBackgroundColor = true);
	void print(const std::string& string, bool overwriteColor = true, bool overwriteBackgroundColor = true);
	void printLine(const std::string& string, bool overwriteColor = true, bool overwriteBackgroundColor = true);
	void nextline();

	// printing (directly)
	void printAt(sf::Vector2u location, const std::string& string, bool overwriteColor = true, bool overwriteBackgroundColor = true);
	void printAt(sf::Vector2u location, const std::string& string, sf::Color color, bool overwriteBackgroundColor = true);
	void printAt(sf::Vector2u location, const std::string& string, sf::Color color, sf::Color backgroundColor);
	void printCharAt(sf::Vector2u location, unsigned char character, bool overwriteColor = true, bool overwriteBackgroundColor = true);
	void printCharAt(sf::Vector2u location, unsigned char character, sf::Color color, bool overwriteBackgroundColor = true);
	void printCharAt(sf::Vector2u location, unsigned char character, sf::Color color, sf::Color backgroundColor);

	// cell manipulation
	void clearCellAt(sf::Vector2u location);
	void setCellAt(sf::Vector2u location, const Cell& cell);
	void setValueAt(sf::Vector2u location, unsigned int value);
	void setColorsAt(sf::Vector2u location, sf::Color color, sf::Color backgroundColor);
	void setColorAt(sf::Vector2u location, sf::Color color);
	void setBackgroundColorAt(sf::Vector2u location, sf::Color backgroundColor);

	// manual scrolling
	void scrollUp(unsigned int amount = 1);
	void scrollDown(unsigned int amount = 1);
	void scrollLeft(unsigned int amount = 1);
	void scrollRight(unsigned int amount = 1);

	// direct manipulation
	void poke(unsigned int index, const Cell& cell);
	void poke(unsigned int index, unsigned int value);
	void poke(unsigned int index, const sf::Color& color);
	void poke(unsigned int index, const sf::Color& color, const sf::Color& backgroundColor);
	Cell peek(unsigned int index) const;

private:
	struct CurrentColors
	{
		sf::Color main;
		sf::Color background;
		sf::Color cursor;
	};

	struct Enabled
	{
		bool exceptions;
		bool automaticUpdate;
		bool showCursor;
		bool showBackround;
		bool scroll;
		bool wrapOnManualScroll;
	};

	// flags
	Enabled m_enabled;

	// definition
	using Cells = std::vector<Cell>;
	Cells m_cells;
	sf::Vector2u m_numberOfCells;
	std::vector<Cells> m_screenBuffers;

	// current draw state
	unsigned int m_cursorIndex;

	// current colours
	CurrentColors m_colors;

	// visual representation
	const sf::PrimitiveType m_primitiveType;
	std::vector<sf::Vertex> m_display;
	std::vector<sf::Vertex> m_backgroundDisplay;
	sf::Vector2f m_size;
	const sf::Texture* m_texture;
	sf::Vector2u m_tileSize;
	unsigned int m_numberOfTilesPerRow;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void priv_updateCell(unsigned int index);
	unsigned int priv_cellIndex(sf::Vector2u location) const;
	sf::Vector2u priv_cellLocation(unsigned int index) const;
	bool priv_isCellIndexInRange(unsigned int index) const;
	bool priv_isCellLocationInRange(sf::Vector2u location) const;
	bool priv_isCursorInRange() const;
	void priv_clearCell(unsigned int index, bool overwriteColor, bool overwriteBackgroundColor);
	void priv_clearCell(unsigned int index, sf::Color backgroundColor, bool overwriteColor);
	void priv_clearCell(unsigned int index, sf::Color color, sf::Color backgroundColor);
	void priv_setCursorIndex(unsigned int index);
	void priv_moveCursorToBeginningOfLine();
	void priv_moveCursorUp();
	void priv_moveCursorDown();
	void priv_moveCursorLeft();
	void priv_moveCursorRight();
	void priv_testCursorForScroll();
	void priv_scroll();
	unsigned int priv_getPrintIndex(sf::Vector2u location) const;
};

struct ConsoleScreen::Cell
{
	unsigned int value;
	sf::Color color;
	sf::Color backgroundColor;
};

} // namespace selbaward
#endif // SELBAWARD_CONSOLESCREEN_HPP
