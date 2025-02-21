//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Spinning Card *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape      Quit
//  Space       Start/restart spin animation
//	Tab			Toggle spin axis (vertical or horizontal)
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/SpinningCard.hpp>
#include <cmath>

int main()
{
	sf::RenderWindow window(sf::VideoMode({ 500u, 500u }), "Spinning card effect");
	window.setFramerateLimit(60u);

	const std::string faceTextureFilename{ "resources/Card Face - SFML.png" };
	const std::string backTextureFilename{ "resources/Card Back - SFML.png" };

	sf::Texture faceTexture{};
	if (!faceTexture.loadFromFile(faceTextureFilename))
		return EXIT_FAILURE;
	faceTexture.setSmooth(true);
	sf::Sprite faceSprite(faceTexture);

	sf::Texture backTexture{};
	if (!backTexture.loadFromFile(backTextureFilename))
		return EXIT_FAILURE;
	backTexture.setSmooth(true);
	sf::Sprite backSprite(backTexture);

	// set up and position cards
	const sf::Vector2f positionOfSpinningCard{ window.getView().getSize() / 2.f };
	faceSprite.setOrigin(faceSprite.getLocalBounds().size / 2.f);
	backSprite.setOrigin(backSprite.getLocalBounds().size / 2.f);
	faceSprite.setPosition(positionOfSpinningCard);
	backSprite.setPosition(positionOfSpinningCard);
	faceSprite.setRotation(sf::degrees(10.f));
	backSprite.setRotation(sf::degrees(10.f));
	//faceSprite.setColor(sf::Color::Blue);
	//backSprite.setColor(sf::Color::Green);

	// prepare spinning card (one for face and one for back)
	sw::SpinningCard cardFace(faceSprite);
	sw::SpinningCard cardBack(backSprite);

	// spinDepth is depth of fake perspective is based on the size of the object (height when flipping horizontally, width when flipping vertically)
	// the higher the depth, the more "fake" it looks. if higher than 1, corners cross
	constexpr float spinDepth{ 0.15f };
	cardFace.setDepth(spinDepth);
	cardBack.setDepth(spinDepth);

	// spin control
	bool isSpinning{ false };
	constexpr float anglePerSecond{ 521.21f };
	float spinAngle{ 0.f };
	bool spinVertically{ false }; // spin horizontally (around the y axis) or vertically (around the x axis)
	sf::Clock clock;

	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>() || (event->is<sf::Event::KeyPressed>() && (event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)))
				window.close();
			else if (const auto keyPressed{ event->getIf<sf::Event::KeyPressed>() })
			{
				if (keyPressed->code == sf::Keyboard::Key::Space)
				{
					isSpinning = true;
					clock.restart();
				}
				else if (keyPressed->code == sf::Keyboard::Key::Tab)
					spinVertically = !spinVertically;
			}
		}

		// update card spin
		if (isSpinning)
		{
			spinAngle = clock.getElapsedTime().asSeconds() * anglePerSecond;
			if (spinAngle >= 360.f)
			{
				spinAngle = 0.f;
				isSpinning = false;
			}
			if (spinVertically)
			{
				cardFace.spinVertically(sf::degrees(spinAngle + 180.f)); // show the face the correct way around
				cardBack.spinVertically(sf::degrees(spinAngle));
			}
			else
			{
				cardFace.spin(sf::degrees(spinAngle + 180.f)); // show the face the correct way around
				cardBack.spin(sf::degrees(spinAngle));
			}

			// smoothly move "out" when spinning
			const float scale{ 1.f + (0.5f * std::sin(spinAngle * 3.14159f / 360.f)) };
			cardFace.setScale({ scale, scale });
			cardBack.setScale({ scale, scale });
		}

		window.clear();

		// side cards (must return for the central card)
		backSprite.move({ -120.f, 0.f });
		backSprite.rotate(sf::degrees(-2.f));
		window.draw(backSprite);
		backSprite.move({ 240.f, 0.f });
		backSprite.rotate(sf::degrees(4.f));
		window.draw(backSprite);
		backSprite.move({ -120.f, 0.f });
		backSprite.rotate(sf::degrees(-2.f));

		// central card
		if (!isSpinning)
			window.draw(backSprite);
		else
		{
			if ((spinAngle > 90.f) && (spinAngle < 270.f))
				window.draw(cardFace);
			else
				window.draw(cardBack);
		}

		window.display();
	}
}
