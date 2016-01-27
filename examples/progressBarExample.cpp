//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Progress Bar *EXAMPLE*
//
//  by Hapax (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape          Quit
//  - + (numpad)    Adjust progress
//  , . (< >)       Adjust rotation
//
//  Please note that this example makes use of C++11 features
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/ProgressBar.hpp>
#include <iostream>

int main()
{
	sf::Texture texture, backgroundTexture;
	if (!texture.loadFromFile("resources/uv map.jpg") ||
		!backgroundTexture.loadFromFile("resources/BlueYellowGradient.png"))
	{
		std::cerr << "Unable to load textures." << std::endl;
		return EXIT_FAILURE;
	}

	sf::RenderWindow window(sf::VideoMode(800, 600), "Progress Bar example");

	sw::ProgressBar progressBar({ 300.f, 40.f });

	// set origin and position
	progressBar.setOrigin(progressBar.getSize() / 2.f);
	progressBar.setPosition(sf::Vector2f(window.getSize() / 2u));

	// customise visual representation
	progressBar.setShowBackgroundAndFrame(true);
	progressBar.setBackgroundColor(sf::Color(128, 128, 128));
	progressBar.setFrameColor(sf::Color(128, 128, 255, 192));
	progressBar.setFrameThickness(2.f);
	progressBar.setRotation(-30);

	// set textures
	progressBar.setTexture(texture);
	progressBar.setTextureRect({ sf::Vector2i(0, texture.getSize().y / 10), sf::Vector2i(texture.getSize().x, texture.getSize().y / 10 + 1) });
	progressBar.setBackgroundTexture(backgroundTexture);
	progressBar.setBackgroundTextureRect({ { 0, 0 }, { 1, static_cast<int>(backgroundTexture.getSize().y) } });

	// set current progression
	progressBar.setFromValueInRange(9u, 0u, 10u);

	// marker
	std::vector<sf::CircleShape> markers(3, sf::CircleShape(3.f));
	for (auto& marker : markers)
		marker.setOrigin(marker.getRadius(), marker.getRadius());
	markers[0].setFillColor(sf::Color::Red);
	markers[1].setFillColor(sf::Color::Yellow);
	markers[2].setFillColor(sf::Color::Green);

	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::Closed) || (event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();
		}

		markers[0].setPosition(progressBar.getAnchorProgressTop());
		markers[1].setPosition(progressBar.getAnchorProgressCenter());
		markers[2].setPosition(progressBar.getAnchorProgressBottom());

		float frameTime{ clock.restart().asSeconds() };

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Add))				        // [+] (number pad)     increase progress
			progressBar.setRatio(progressBar.getRatio() + frameTime * 0.3f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Subtract))                 // [-] (number pad)     decrease progress
			progressBar.setRatio(progressBar.getRatio() - frameTime * 0.3f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Period))                   // [.]/[>]              rotate left
			progressBar.rotate(frameTime * 30.f);
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Comma))                    // [,]/[<]              rotate right
			progressBar.rotate(-frameTime * 30.f);

		window.clear();
		window.draw(progressBar);
		for (auto& marker : markers)
			window.draw(marker);
		window.display();
	}


	return EXIT_SUCCESS;
}