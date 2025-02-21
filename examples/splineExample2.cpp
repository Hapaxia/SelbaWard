//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Spline *EXAMPLE 2*
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
//  Backspace key       Toggle closed Spline
//  Slash key (/)       Toggle random normal offsets (lightning effect)
//  [ key               Cycle start cap types
//  ] key               Cycle end cap types
//  Enter/Return key    Cycle corner types
//
//
//  Points primitive is only used when the Spline is not thick.
//  Corner types and cap types only affect the Spline when it is thick.
//
//  Low frame rate is intentional so that fewer points are added to show how bezier interpolation can create curves from very few points
//  The majority of this code is just processing the key controls.
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Spline.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Selba Ward - Spline example 2");
	window.setFramerateLimit(15u);

	const unsigned int numberOfVertices{ 50u }; // number of (control) vertices in spline (one vertex per frame)
	sw::Spline spline(numberOfVertices);
	spline.setInterpolationSteps(10u); // use interpolation steps so that when bezier interpolation is enabled, smooth curve will be shown

	bool isPaused{ false };

	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			else if (const auto keyPressed{ event->getIf<sf::Event::KeyPressed>() })
				switch (keyPressed->code)
				{
				case sf::Keyboard::Key::Escape:
					// close
					window.close();
					break;
				case sf::Keyboard::Key::F1:
					// toggle paused
					isPaused = !isPaused;
					break;
				case sf::Keyboard::Key::Space:
					// toggle interpolation type
					spline.setBezierInterpolation(!spline.getBezierInterpolation());
					break;
				case sf::Keyboard::Key::Tab:
					// toggle primitive type
					if (spline.getPrimitiveType() == sf::PrimitiveType::LineStrip)
						spline.setPrimitiveType(sf::PrimitiveType::Points);
					else
						spline.setPrimitiveType(sf::PrimitiveType::LineStrip);
					break;
				case sf::Keyboard::Key::Backspace:
					// toggle closed Spline
					spline.setClosed(!spline.getClosed());
					break;
				case sf::Keyboard::Key::Slash:
					// toggle random normal offsets (lightning effect) - activates when in use
					spline.setRandomNormalOffsetRange(20.f - spline.getRandomNormalOffsetRange());
					spline.setRandomNormalOffsetsActivated(spline.getRandomNormalOffsetRange() > 10.f);
					break;
				case sf::Keyboard::Key::LBracket:
					// cycle start cap type
					switch (spline.getThickStartCapType())
					{
					case sw::Spline::ThickCapType::None:
						spline.setThickStartCapType(sw::Spline::ThickCapType::Extended);
						break;
					case sw::Spline::ThickCapType::Extended:
						spline.setThickStartCapType(sw::Spline::ThickCapType::Round);
						break;
					case sw::Spline::ThickCapType::Round:
						spline.setThickStartCapType(sw::Spline::ThickCapType::None);
						break;
					}
					break;
				case sf::Keyboard::Key::RBracket:
					// cycle end cap type
					switch (spline.getThickEndCapType())
					{
					case sw::Spline::ThickCapType::None:
						spline.setThickEndCapType(sw::Spline::ThickCapType::Extended);
						break;
					case sw::Spline::ThickCapType::Extended:
						spline.setThickEndCapType(sw::Spline::ThickCapType::Round);
						break;
					case sw::Spline::ThickCapType::Round:
						spline.setThickEndCapType(sw::Spline::ThickCapType::None);
						break;
					}
					break;
				case sf::Keyboard::Key::Enter:
					// cycle corner type
					switch (spline.getThickCornerType())
					{
					case sw::Spline::ThickCornerType::Point:
						spline.setThickCornerType(sw::Spline::ThickCornerType::Bevel);
						break;
					case sw::Spline::ThickCornerType::Bevel:
						spline.setThickCornerType(sw::Spline::ThickCornerType::Round);
						break;
					case sw::Spline::ThickCornerType::Round:
						spline.setThickCornerType(sw::Spline::ThickCornerType::Point);
						break;
					}
					break;
				}
			else if (const auto mouseWheel{ event->getIf<sf::Event::MouseWheelScrolled>() })
			{
				// mouse wheel changes thickness
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
