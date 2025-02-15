//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Line *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//    Controls:
//
//  Escape key              Quit
//  Left mouse button       Move start position of both lines (thick and thin) to current mouse position
//  Right mouse button      Move end position of both lines (thick and thin) to current mouse position
//  Scroll mouse wheel      Change thickness of thick line
//
//  Please note that this example makes use of C++11 features
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Line.hpp>

int main()
{
	float lineThickness{ 45.f };

	sw::Line thickLine;
	thickLine.setPoint(thickLine.getStartIndex(), { 100, 100 });
	thickLine.setPoint(thickLine.getEndIndex(), { 250, 130 });
	thickLine.setColor(sf::Color(64, 64, 128));
	thickLine.setThickness(lineThickness);
	sw::Line line;
	line.setPoint(line.getStartIndex(), { 100, 100 });
	line.setPoint(line.getEndIndex(), { 250, 130 });
	line.setColor(sf::Color(128, 196, 255));

	struct
	{
		bool        left, right;
	} isButtonDown{ false, false };

	std::ios_base::iostate sfErrIoState = sf::err().rdstate(); // stores current state of SFML error stream
	sf::err().clear(std::ios::failbit); // disables SFML error stream
	sf::ContextSettings contextSettings;
	contextSettings.antiAliasingLevel = 8;
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Line test", sf::Style::Default, sf::State::Windowed, contextSettings);
	window.setFramerateLimit(60);
	sf::err().clear(sfErrIoState); // re-enables SFML error stream (re-enstates the stored state)

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
				window.close();
			else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonPressed>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
					isButtonDown.left = true;
				else if (mouseButton->button == sf::Mouse::Button::Right)
					isButtonDown.right = true;
			}
			else if (const auto* mouseButton = event->getIf<sf::Event::MouseButtonReleased>())
			{
				if (mouseButton->button == sf::Mouse::Button::Left)
					isButtonDown.left = false;
				else if (mouseButton->button == sf::Mouse::Button::Right)
					isButtonDown.right = false;
			}
			else if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>())
			{
				const float lineThicknessOffset{ 2.f };
				if (mouseWheel->delta > 0)
					lineThickness += lineThicknessOffset;
				else
					lineThickness -= lineThicknessOffset;
				if (lineThickness < 0.f)
					lineThickness = 0.f;
				thickLine.setThickness(lineThickness);
			}
		}

		if (isButtonDown.left)
		{
			const sf::Vector2f mousePosition{ sf::Mouse::getPosition(window) };
			line.setPoint(line.getStartIndex(), mousePosition); // "line.getStartIndex()" can be replaced with "sw::Line::PointIndex::Start", "sw::Line::Start", "line.PointIndex.Start", "line.Start" or "0"
			thickLine.setPoint(thickLine.getStartIndex(), mousePosition);
		}
		if (isButtonDown.right)
		{
			const sf::Vector2f mousePosition{ sf::Mouse::getPosition(window) };
			line.setPoint(line.getEndIndex(), mousePosition); // "line.getEndIndex()" can be replaced with "sw::Line::PointIndex::End", "sw::Line::End", "line.PointIndex.End", "line.End" or "1"
			thickLine.setPoint(thickLine.getEndIndex(), mousePosition);
		}

		window.clear();
		window.draw(thickLine);
		window.draw(line);
		window.display();
	}

	return EXIT_SUCCESS;
}