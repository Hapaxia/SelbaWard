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
//  Tab key             Toggle primitive type (only applies when line in not "thick")
//
//  Low frame rate is intentional so that fewer points are added to show how bezier interpolation can create curves from very few points
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Spline.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Selba Ward - Spline example");
	window.setFramerateLimit(15u);

	const std::size_t numberOfVertices{ 50u }; // number of (control) vertices in spline (one vertex per frame)
	sw::Spline spline(numberOfVertices);
	spline.setInterpolationSteps(10u); // use interpolation steps so that when bezier interpolation is enabled, smooth curve will be shown

	bool isPaused{ false };

	while (window.isOpen())
	{
		while (const std::optional event = window.pollEvent())
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			else if (const auto keyPressed{ event->getIf<sf::Event::KeyPressed>() })
			{
				switch (keyPressed->code)
				{
				case sf::Keyboard::Key::Escape:
					window.close();
					break;
				case sf::Keyboard::Key::F1: // toggle pause
					isPaused = !isPaused;
					break;
				case sf::Keyboard::Key::Space: // toggle bezier/linear curve mode
					spline.setBezierInterpolation(!spline.getBezierInterpolation());
					break;
				case sf::Keyboard::Key::Tab: // toggle primitive type
					if (spline.getPrimitiveType() == sf::PrimitiveType::LineStrip)
						spline.setPrimitiveType(sf::PrimitiveType::Points);
					else
						spline.setPrimitiveType(sf::PrimitiveType::LineStrip);
					break;
				}
			}
			else if (const auto mouseWheel{ event->getIf<sf::Event::MouseWheelScrolled>() })
			{
				const float thickness{ spline.getThickness() + mouseWheel->delta };
				spline.setThickness((thickness < 0.f) ? 0.f : thickness);
			}
		}

		if (!isPaused)
		{
			const sf::Vector2f mousePosition{ window.mapPixelToCoords(sf::Mouse::getPosition(window)) };
			spline.removeVertex(0u);
			spline.addVertex(mousePosition);
		}

		spline.smoothHandles();
		spline.update();

		window.clear();
		window.draw(spline);
		window.display();
	}
}
