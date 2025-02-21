//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Sprite 3D *EXAMPLE* (Card Fan Animation)
//
//  by Hapaxia (https://github.com/Hapaxia)
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
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Sprite3d.hpp>
#include <iostream>
#include <cmath>

namespace
{

inline float ease(const float start, const float end, const float alpha)
{
	const float easedAlpha{ alpha * alpha * (3.f - 2.f * alpha) };
	return (start * (1.f - easedAlpha) + end * easedAlpha);
}

} // namespace

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 450u, 250u }), "Sprite3d - Card Fan Animation");
	window.setFramerateLimit(60u);

	std::vector<sf::Texture> cardTextures(7u); // 6 faces and 1 back
	if (!cardTextures[0u].loadFromFile("resources/Card Back - SFML.png") ||
		!cardTextures[1u].loadFromFile("resources/Card Face - Hearts H.png") ||
		!cardTextures[2u].loadFromFile("resources/Card Face - Spades A.png") ||
		!cardTextures[3u].loadFromFile("resources/Card Face - Diamonds P.png") ||
		!cardTextures[4u].loadFromFile("resources/Card Face - Hearts A.png") ||
		!cardTextures[5u].loadFromFile("resources/Card Face - Clubs X.png") ||
		!cardTextures[6u].loadFromFile("resources/Card Face - SFML.png"))
	{
		std::cerr << "Failed to load textures." << std::endl;
		return EXIT_FAILURE;
	}

	std::vector<sw::Sprite3d> cards{};
	cards.push_back(sw::Sprite3d(cardTextures[1u], cardTextures[0u]));
	cards.push_back(sw::Sprite3d(cardTextures[2u], cardTextures[0u]));
	cards.push_back(sw::Sprite3d(cardTextures[3u], cardTextures[0u]));
	cards.push_back(sw::Sprite3d(cardTextures[4u], cardTextures[0u]));
	cards.push_back(sw::Sprite3d(cardTextures[5u], cardTextures[0u]));
	cards.push_back(sw::Sprite3d(cardTextures[6u], cardTextures[0u]));

	for (auto& card : cards)
	{
		card.setFlipBack();
		card.setDepth(30.f);
		card.setColor(sf::Color::White);
		card.setSubdivision(2u);
		card.setOrigin({ card.getLocalBounds().size.x, card.getLocalBounds().size.y / 2.f }); // centre-right (left of back, which you see first)
		card.setYaw(180.f);
	}
	for (std::size_t c{ 0u }; c < cards.size(); ++c)
		cards[c].setPosition({ 150.f + 30.f * c, 125.f });

	// adjustable opacity
	float opacity{ 1.f }; // opacity of cards (0 = fully transparent, 1 = fully opaque)
	bool hasOpacityChanged{ true };

	// clock and pause
	sf::Clock clock{};
	bool isPaused{ true };
	float time{ 0.f };

	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
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

				// pause/unpause
				case sf::Keyboard::Key::Space:
					isPaused = !isPaused;
					clock.restart();
					break;

				// reset time
				case sf::Keyboard::Key::Backspace:
					time = 0.f;
					clock.restart();
					break;

				// adjust opacity
				case sf::Keyboard::Key::LBracket:
					opacity -= 0.05f;
					hasOpacityChanged = true;
					break;
				case sf::Keyboard::Key::RBracket:
					opacity += 0.05f;
					hasOpacityChanged = true;
					break;
				}

				/*
				// pause/unpause
				if (keyPressed->code == sf::Keyboard::Key::Space)
				{
					isPaused = !isPaused;
					clock.restart();
				}

				// reset time
				else if (keyPressed->code == sf::Keyboard::Key::Backspace)
				{
					time = 0.f;
					clock.restart();
				}

				// adjust opacity
				else if (keyPressed->code == sf::Keyboard::Key::LBracket)
				{
					opacity -= 0.05f;
					hasOpacityChanged = true;
				}
				else if (keyPressed->code == sf::Keyboard::Key::RBracket)
				{
					opacity += 0.05f;
					hasOpacityChanged = true;
				}
				*/
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
				card.setColor(sf::Color(255u, 255u, 255u, static_cast<unsigned char>(opacity * 255u)));
			std::cout << "Opacity is: " << std::round(opacity * 100.f) << "%" << std::endl;
			hasOpacityChanged = false;
		}

		// update time
		float frameTime{ 1.f };
		if (!isPaused)
		{
			frameTime = clock.restart().asSeconds();
			time += frameTime;
		}

		// update cards' angles
		const float controlAngle{ time * 100.f };
		for (std::size_t c{ 0 }; c < cards.size(); ++c)
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
		for (std::vector<sw::Sprite3d>::iterator begin{ cards.begin() }, end{ cards.end() }, it{ begin }; it != end; ++it)
		{
			// forwards iteration for cards face up
			if (it->getYaw() < 90.f)
				window.draw(*it);
		}
		for (std::vector<sw::Sprite3d>::reverse_iterator begin{ cards.rbegin() }, end{ cards.rend() }, it{ begin }; it != end; ++it)
		{
			// backwards iteration for cards face down
			if (it->getYaw() > 90.f)
				window.draw(*it);
		}
		window.display();
	}
}