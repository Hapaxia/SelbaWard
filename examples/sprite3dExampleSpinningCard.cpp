//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Sprite3d EXAMPLE (similar to SpinningCard example)
//
//  by Hapax (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape      Quit
//  Space       Start/restart spin animation
//
//  Please note that this example makes use of C++11 features
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard/Sprite3d.hpp>

int main()
{
	sf::RenderWindow window(sf::VideoMode(500, 500), "Sprite3d simplifying the SpinningCard example");
	window.setFramerateLimit(60);

	const std::string faceTextureFilename{ "resources/Card Face - SFML.png" };
	const std::string backTextureFilename{ "resources/Card Back - SFML.png" };

	sf::Texture faceTexture, backTexture;
	if (!faceTexture.loadFromFile(faceTextureFilename))
		return EXIT_FAILURE;
	if (!backTexture.loadFromFile(backTextureFilename))
		return EXIT_FAILURE;
	faceTexture.setSmooth(true);
	backTexture.setSmooth(true);

	sw::Sprite3d card(backTexture, faceTexture); // swap textures so that we can treat it as face up when face down. also getting a sprite from sprite3d has the first texture (and we want the other sprites to have the back texture)

	// set up and position main card
	const sf::Vector2f positionOfSpinningCard{ static_cast<float>(window.getSize().x) / 2, static_cast<float>(window.getSize().y) / 2 };
	card.setOrigin(card.getLocalBounds().width / 2, card.getLocalBounds().height / 2);
	card.setPosition(sf::Vector2f(window.getSize() / 2u));
	card.setRotation(10);
	card.setSubdivision(2);
	card.setDepth(30.f);
	card.setBackFlipEnabled(); // correct 'back' face as that it where the face of the card is
	//card.setColor(sf::Color::Green);

	// set up static cards
	std::vector<sf::Sprite> sideCards(2, card.getSprite());
	sideCards.front().rotate(-2);
	sideCards.back().rotate(2);
	sideCards.front().move(-120, 0);
	sideCards.back().move(120, 0);

	// spin control
	bool isSpinning{ false };
	const float anglePerSecond{ 521.21f };
	float spinAngle{ 0.f };
	const bool spinVertically{ true }; // spin horizontally (around the y axis) or vertically (around the x axis)
	sf::Clock clock;

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if ((event.type == sf::Event::Closed) || (event.type == sf::Event::KeyPressed) && (event.key.code == sf::Keyboard::Escape))
				window.close();
			if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::Space)
				{
					isSpinning = true;
					clock.restart();
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
		const float scale{ 1.f + (0.5f * sin(spinAngle * 3.14159f / 360)) }; // smoothly move "out" when spinning
		card.setScale(scale, scale);
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

	return 0;
}
