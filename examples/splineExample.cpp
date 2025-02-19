//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Spline *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//  Draw a spline in the window by moving the mouse.
//
//
//    Controls:
//
//  Escape key          Quit
//  Mouse movement      Draw
//  Mouse wheel         Adjust thickness
//  F1 key              Toggle pause
//  Space key           Toggle bezier/linear curve mode
//  Tab key             Toggle primitive type
//
//  Low frame rate is intentional so that fewer points are added to show how bezier interpolation can create curves from very few points
//
//  Please note that this example makes use of C++11 features
//    and also requires the SFML library (http://www.sfml-dev.org)
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Spline.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800, 600 }), "Selba Ward - Spline example", sf::Style::Default);
	window.setFramerateLimit(15);

	const unsigned int numberOfVertices{ 50u }; // number of (control) vertices in spline (one vertex per frame)
	sw::Spline spline(numberOfVertices);
	spline.setInterpolationSteps(10u); // use interpolation steps so that when bezier interpolation is enabled, smooth curve will be shown

	bool isPaused{ false };

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>() || event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
				window.close();
			else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
			{
				if (keyPressed->code == sf::Keyboard::Key::F1) // toggle pause
					isPaused = !isPaused;
				else if (keyPressed->code == sf::Keyboard::Key::Space) // toggle bezier/linear curve mode
					spline.setBezierInterpolation(!spline.getBezierInterpolation());
				else if (keyPressed->code == sf::Keyboard::Key::Tab) // toggle primitive type
				{
					if (spline.getPrimitiveType() == sf::PrimitiveType::LineStrip)
						spline.setPrimitiveType(sf::PrimitiveType::Points);
					else
						spline.setPrimitiveType(sf::PrimitiveType::LineStrip);
				}
			}
			else if (const auto* mouseWheel = event->getIf<sf::Event::MouseWheelScrolled>())
			{
				const float thickness{ spline.getThickness() + mouseWheel->delta };
				spline.setThickness((thickness < 0) ? 0.f : thickness);
			}
		}

		if (!isPaused)
		{
			const sf::Vector2f mousePosition = window.mapPixelToCoords(sf::Mouse::getPosition(window));
			spline.removeVertex(0);
			spline.addVertex(mousePosition);
		}

		spline.smoothHandles();
		spline.update();

		window.clear();
		window.draw(spline);
		window.display();
	}

	return EXIT_SUCCESS;
}
