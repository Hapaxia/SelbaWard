//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Sprite 3D *EXAMPLE* (similar to Spinning Card example)
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape      Quit
//  Space       Start/restart spin animation
//	Tab			Toggle spin axis (horizontal or vertical)
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Sprite3d.hpp>
#include <cmath>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 500u, 500u }), "Sprite3d simplifying the SpinningCard example");
	window.setFramerateLimit(60u);

	const std::string faceTextureFilename{ "resources/Card Face - SFML.png" };
	const std::string backTextureFilename{ "resources/Card Back - SFML.png" };

	sf::Texture faceTexture{}, backTexture{};
	if (!faceTexture.loadFromFile(faceTextureFilename))
		return EXIT_FAILURE;
	if (!backTexture.loadFromFile(backTextureFilename))
		return EXIT_FAILURE;
	faceTexture.setSmooth(true);
	backTexture.setSmooth(true);

	sw::Sprite3d card(backTexture, faceTexture); // swap textures so that we can treat it as face up when face down. also getting a sprite from sprite3d has the first texture (and we want the other sprites to have the back texture)

	// set up and position main card
	const sf::Vector2f positionOfSpinningCard{ window.getView().getSize() / 2.f };
	card.setOrigin(card.getLocalBounds().size / 2.f);
	card.setPosition(positionOfSpinningCard);
	card.setRotation(10.f);
	card.setSubdivision(2u);
	card.setDepth(30.f);
	card.setFlipBack(); // correct 'back' face as that it where the face of the card is
	//card.setColor(sf::Color::Green);

	// set up static cards
	std::vector<sf::Sprite> sideCards(2u, card.getSprite());
	sideCards.front().rotate(sf::degrees(-2.f));
	sideCards.back().rotate(sf::degrees(2.f));
	sideCards.front().move({ -120.f, 0.f });
	sideCards.back().move({ 120.f, 0.f });

	// spin control
	bool isSpinning{ false };
	constexpr float anglePerSecond{ 521.21f };
	float spinAngle{ 0.f };
	bool spinVertically{ true }; // spin horizontally (around the y axis) or vertically (around the x axis)
	sf::Clock clock{};

	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			if (const auto keyPressed{ event->getIf<sf::Event::KeyPressed>() })
			{
				switch (keyPressed->code)
				{
				case sf::Keyboard::Key::Escape:
					window.close();
					break;
				case sf::Keyboard::Key::Space:
					isSpinning = true;
					clock.restart();
					break;
				case sf::Keyboard::Key::Tab:
					spinVertically = !spinVertically;
					// we must also reset the rotation on the other axis as Sprite 3D can rotate on both of them at once
					if (spinVertically)
						card.setYaw(0.f);
					else
						card.setPitch(0.f);
					break;
				}
			}
		}

		// update spin angle
		if (isSpinning)
		{
			spinAngle = clock.getElapsedTime().asSeconds() * anglePerSecond;
			if (spinAngle >= 360.f)
			{
				spinAngle = 0.f;
				isSpinning = false;
			}

		}

		// update card
		const float scale{ 1.f + (0.5f * std::sin(spinAngle * 3.14159f / 360.f)) }; // smoothly move "out" when spinning
		card.setScale({ scale, scale });
		if (spinVertically)
			card.setPitch(spinAngle);
		else
			card.setYaw(spinAngle);

		// update display
		window.clear();
		for (auto& sideCard : sideCards)
			window.draw(sideCard);
		window.draw(card); // draw main card afterwards as it must appear in front
		window.display();
	}
}
