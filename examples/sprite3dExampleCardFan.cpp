//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Sprite 3D *EXAMPLE* (Card Fan Animation)
//
//  by Hapax (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape          Quit
//  Space           Pause/unpause
//  Backspace       Rewind animation
//  [               Reduce opacity
//  ]               Increase opacity
//
//  Please note that this example makes use of C++11 features
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Sprite3d.hpp>
#include <iostream>

inline float ease(float start, float end, float alpha)
{
	const float easedAlpha{ alpha * alpha * (3 - 2 * alpha) };
	return (start * (1 - easedAlpha) + end * easedAlpha);
}

int main()
{
	sf::RenderWindow window(sf::VideoMode(450, 250), "Sprite3d - Card Fan Animation", sf::Style::Default);
	window.setFramerateLimit(60);

	std::vector<sf::Texture> cardTextures(7); // 6 faces and 1 back
	if (!cardTextures[0].loadFromFile("resources/Card Back - SFML.png") ||
		!cardTextures[1].loadFromFile("resources/Card Face - Hearts H.png") ||
		!cardTextures[2].loadFromFile("resources/Card Face - Spades A.png") ||
		!cardTextures[3].loadFromFile("resources/Card Face - Diamonds P.png") ||
		!cardTextures[4].loadFromFile("resources/Card Face - Hearts A.png") ||
		!cardTextures[5].loadFromFile("resources/Card Face - Clubs X.png") ||
		!cardTextures[6].loadFromFile("resources/Card Face - SFML.png"))
	{
		std::cerr << "Failed to load textures." << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<sw::Sprite3d> cards;
	cards.push_back(sw::Sprite3d(cardTextures[1], cardTextures[0]));
	cards.push_back(sw::Sprite3d(cardTextures[2], cardTextures[0]));
	cards.push_back(sw::Sprite3d(cardTextures[3], cardTextures[0]));
	cards.push_back(sw::Sprite3d(cardTextures[4], cardTextures[0]));
	cards.push_back(sw::Sprite3d(cardTextures[5], cardTextures[0]));
	cards.push_back(sw::Sprite3d(cardTextures[6], cardTextures[0]));

	for (auto& card : cards)
	{
		card.setBackFlipEnabled();
		card.setDepth(30.f);
		card.setColor(sf::Color::White);
		card.setSubdivision(2);
		card.setOrigin({ card.getLocalBounds().width, card.getLocalBounds().height / 2.f }); // centre-right (left of back, which you see first)
		card.setYaw(180.f);
	}
	for (unsigned int c{ 0 }; c < cards.size(); ++c)
		cards[c].setPosition(150.f + 30.f * c, 125.f);

	// adjustable opacity
	float opacity{ 1.f }; // opacity of cards (0 = fully transparent, 1 = fully opaque)
	bool hasOpacityChanged{ true };

	// clock and pause
	sf::Clock clock;
	bool isPaused{ true };
	float time{ 0.f };

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed || event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
				window.close();
			else if (event.type == sf::Event::KeyPressed)
			{
				// pause/unpause
				if (event.key.code == sf::Keyboard::Space)
				{
					isPaused = !isPaused;
					clock.restart();
				}

				// reset time
				else if (event.key.code == sf::Keyboard::BackSpace)
				{
					time = 0.f;
					clock.restart();
				}

				// adjust opacity
				else if (event.key.code == sf::Keyboard::LBracket)
				{
					opacity -= 0.05f;
					hasOpacityChanged = true;
				}
				else if (event.key.code == sf::Keyboard::RBracket)
				{
					opacity += 0.05f;
					hasOpacityChanged = true;
				}
			}
		}

		// clamp opacity
		if (opacity < 0.f)
			opacity = 0.f;
		else if (opacity > 1.f)
			opacity = 1.f;

		// output opacity when changed
		if (hasOpacityChanged)
		{
			for (auto& card : cards)
				card.setColor(sf::Color(255, 255, 255, static_cast<unsigned char>(opacity * 255)));
			std::cout << "Opacity is: " << round(opacity * 100) << "%" << std::endl;
			hasOpacityChanged = false;
		}

		// update time
		float frameTime = 1.f;
		if (!isPaused)
		{
			frameTime = clock.restart().asSeconds();
			time += frameTime;
		}

		// update cards' angles
		const float controlAngle{ time * 100.f };
		for (unsigned int c{ 0 }; c < cards.size(); ++c)
		{
			const float angleDifference{ 7.1f }; // angle difference between each pair of cards. this value was chosen to make the cards' edges touch
			float currentAngle{ 270.f - controlAngle - (angleDifference * c) };
			if (currentAngle > 180.f)
				currentAngle = 180.f;
			else if (currentAngle < 0.f)
				currentAngle = 0.f;
			currentAngle = ease(0.f, 180.f, currentAngle / 180.f);
			cards[c].setYaw(currentAngle);
		}

		// update display
		window.clear();
		for (std::vector<sw::Sprite3d>::iterator begin = cards.begin(), end = cards.end(), it = begin; it != end; ++it)
		{
			// forwards iteration for cards face up
			if (it->getYaw() < 90.f)
				window.draw(*it);
		}
		for (std::vector<sw::Sprite3d>::reverse_iterator begin = cards.rbegin(), end = cards.rend(), it = begin; it != end; ++it)
		{
			// backwards iteration for cards face down
			if (it->getYaw() > 90.f)
				window.draw(*it);
		}
		window.display();
	}

	return EXIT_SUCCESS;
}