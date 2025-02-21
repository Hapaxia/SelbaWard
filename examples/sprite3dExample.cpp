//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Sprite 3D *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape          Quit
//  Space           Pause/unpause
//  Backspace       Reset clock
//  - =             Adjust depth
//  [ ]             Adjust mesh density
//  , .             Adjust subdivision level
//  Tab             Toggle textures
//  Return          Toggle Dynamic Subdivision (default range is 1 to 3)
//  B               Toggle back face correction (flip)
//  F4              Resets mesh to minimal (two triangles) and turns off Dynamic Subdivision
//  F5              Toggle visibility of Sprite3d's bounds
//  F6              Toggle visibility of standard sprite's bounds
//  F7              Toggle visibility of Sprite3d
//  F8              Toggle visibility of standard sprite
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <iostream> // for std::cerr
#include <cmath>

#include <SelbaWard/Sprite3d.hpp>

int main()
{
	// set up window
	sf::RenderWindow window(sf::VideoMode({ 800u, 600u }), "Sprite3d");
	//window.setVerticalSyncEnabled(true);

	// load resources (textures and font)
	sf::Texture texture{}, frontTexture{}, backTexture{};
	if (!texture.loadFromFile("resources/uv map.jpg") ||
		!frontTexture.loadFromFile("resources/uv map front.jpg") ||
		!backTexture.loadFromFile("resources/uv map back.jpg"))
		/*
		if (!texture.loadFromFile("resources/Card Face - SFML.png") ||
		!frontTexture.loadFromFile("resources/Card Face - SFML.png") ||
		!backTexture.loadFromFile("resources/Card Back - SFML.png"))
		*/
	{
		std::cerr << "Error loading textures." << std::endl;
		return EXIT_FAILURE; // textures are required for this example. it's pointless carrying on without them
	}
	sf::Font font{};
	if (!font.openFromFile("resources/arial.ttf"))
		std::cerr << "Error loading font." << std::endl; // no need to exit if font fails; it will just not show any text

	// text headers
	sf::Text sprite3dText(font, "Sprite3d", 48u);
	sf::Text spriteText(font, "Sprite", 48u);
	sprite3dText.setOrigin({ sprite3dText.getLocalBounds().position.x + sprite3dText.getLocalBounds().size.x / 2.f, 0.f });
	spriteText.setOrigin({ spriteText.getLocalBounds().position.x + spriteText.getLocalBounds().size.x / 2.f, 0.f });
	sprite3dText.setPosition({ window.getSize().x * 0.25f, 0.f });
	spriteText.setPosition({ window.getSize().x * 0.75f, 0.f });

	// text feedback
	sf::Text feedbackText(font, "FPS:\nVertices:\nSubdivision Level:\nSubdivided Mesh Density:\nMesh Density:\nDynamic Subdivision enabled:\nMost Extreme Angle:\nDepth:", 16u);
	feedbackText.setOrigin({ feedbackText.getLocalBounds().position.x, feedbackText.getLocalBounds().position.y + feedbackText.getLocalBounds().size.y });
	feedbackText.setPosition({ 2.f, window.getSize().y - 1.f });

	// sprite3d
	sw::Sprite3d sprite3d(frontTexture, backTexture);
	sprite3d.setOrigin(sprite3d.getLocalBounds().size / 2.f);
	sprite3d.setPosition({ window.getSize().x * 0.25f, window.getSize().y / 2.f });
	//sprite3d.setFlipBack();
	//sprite3d.setMeshDensity(3u); // = 5x5 points = 4x4 quads
	//sprite3d.setNumberOfPoints(25u); // 5x5
	//sprite3d.setNumberOfQuads(16u); // 4x4
	//sprite3d.setDynamicSubdivisionRange(3u, 0u); // max: 32x32 quads (4 subdivided 3 times is 32), min: 4x4 quads (zero subdivisions)

	// sprite
	//sf::Sprite sprite(sprite3d.getSprite());
	sf::Sprite sprite(texture);
	sprite.setOrigin(sprite.getLocalBounds().size / 2.f);
	sprite.setPosition({ window.getSize().x * 0.75f, window.getSize().y / 2.f });

	// bounds rectangles
	sf::RectangleShape boundsSprite3d{};
	boundsSprite3d.setFillColor(sf::Color::Transparent);
	boundsSprite3d.setOutlineColor(sf::Color::Red);
	boundsSprite3d.setOutlineThickness(1.f);
	sf::RectangleShape boundsSprite{ boundsSprite3d };

	// clock and pause
	sf::Clock clock{};
	bool isPaused{ true };
	float time{ 0.f };

	// visibility flags
	struct
	{
		bool     sprite3d, sprite, sprite3dBounds, spriteBounds;
	} isVisible{ true, true, false, false };

	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
		{
			//if (event->is<sf::Event::Closed>() || event->is<sf::Event::KeyPressed>() && event->getIf<sf::Event::KeyPressed>()->code == sf::Keyboard::Key::Escape)
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

				// control depth
				case sf::Keyboard::Key::Equal:
					sprite3d.setDepth(sprite3d.getDepth() + 1.f);
					break;
				case sf::Keyboard::Key::Hyphen:
					sprite3d.setDepth(sprite3d.getDepth() - 1.f);
					break;

				// control mesh density
				case sf::Keyboard::Key::RBracket:
					sprite3d.setMeshDensity(sprite3d.getMeshDensity() + 1u);
					break;
				case sf::Keyboard::Key::LBracket:
					if (sprite3d.getMeshDensity() > 0u)
						sprite3d.setMeshDensity(sprite3d.getMeshDensity() - 1u);
					break;

				// control subdivision level
				case sf::Keyboard::Key::Period:
					sprite3d.setSubdivision(sprite3d.getSubdivision() + 1u);
					break;
				case sf::Keyboard::Key::Comma:
					if (sprite3d.getSubdivision() > 0u)
						sprite3d.setSubdivision(sprite3d.getSubdivision() - 1u);
					break;

				// toggle dynamic subdivision
				case sf::Keyboard::Key::Enter:
					sprite3d.setDynamicSubdivision(!sprite3d.getDynamicSubdivision());
					break;

				// toggle back face is flipped state
				case sf::Keyboard::Key::B:
					sprite3d.setFlipBack(!sprite3d.getFlipBack());
					break;

				// toggle bounds rectangles
				case sf::Keyboard::Key::F5:
					isVisible.sprite3dBounds = !isVisible.sprite3dBounds;
					break;
				case sf::Keyboard::Key::F6:
					isVisible.spriteBounds = !isVisible.spriteBounds;
					break;

				// toggle whether or not to draw the sprites
				case sf::Keyboard::Key::F7:
					isVisible.sprite3d = !isVisible.sprite3d;
					break;
				case sf::Keyboard::Key::F8:
					isVisible.sprite = !isVisible.sprite;
					break;

				// reset the mesh to minimal (also turns off dynamic subdivision)
				case sf::Keyboard::Key::F4:
					sprite3d.setDynamicSubdivision(false);
					sprite3d.minimalMesh();
					break;

				// toggle texture usage
				case sf::Keyboard::Key::Tab:
					if (sprite3d.getTexture() != &frontTexture)
					{
						sprite3d.setTexture(frontTexture);
						sprite3d.setBackTexture(backTexture);
					}
					else
					{
						sprite3d.setTexture();
						sprite3d.setBackTexture();
					}
					break;
				}
			}
		}

		// update time
		float frameTime{ 1.f };
		if (!isPaused)
		{
			frameTime = clock.restart().asSeconds();
			time += frameTime;
		}

		// update sprites
		sprite3d.setRotation({ time * 97.f, time * 42.f, time * 51.f });

		//sprite.setScale({ std::cos(time * 97.f * 3.141592653f / 180.f), std::cos(time * 42.f * 3.141592653f / 180.f) });
		sprite.setScale({ std::cos(time * 42.f * 3.141592653f / 180.f), std::cos(time * 97.f * 3.141592653f / 180.f) });
		sprite.setRotation(sf::degrees(time * 51.f));
		// other sprite3d rotations for testing
		//sprite3d.setRotation(time * 60.f);
		/*
		sprite3d.setPitch(time * 60.f);
		sprite3d.setYaw(time * 40.f);
		*/
		//sprite3d.setYaw(time * 180.f);

		// update bounds rectangle
		boundsSprite3d.setPosition(sprite3d.getGlobalBounds().position);
		boundsSprite3d.setSize(sprite3d.getGlobalBounds().size);
		boundsSprite.setPosition(sprite.getGlobalBounds().position);
		boundsSprite.setSize(sprite.getGlobalBounds().size);

		// update depth text
		std::size_t subdividedMeshDensity{ sprite3d.getMeshDensity() };
		for (std::size_t i{ 0u }; i < sprite3d.getSubdivision(); ++i)
			subdividedMeshDensity = (subdividedMeshDensity * 2u) + 1u;
		std::size_t numberOfVerticesInSubdividedMesh{ (((subdividedMeshDensity * 2u) + 5u) * subdividedMeshDensity) + 4u };
		feedbackText.setString(
			"FPS: " + std::to_string(static_cast<unsigned int>(1.f / frameTime)) +
			"\nVertices: " + std::to_string(numberOfVerticesInSubdividedMesh) +
				" (" + std::to_string(numberOfVerticesInSubdividedMesh - 2u) + " triangles)" +
			"\nSubdivision Level: " + std::to_string(sprite3d.getSubdivision()) +
			"\nSubdivided Mesh Density: " + std::to_string(sprite3d.getSubdividedMeshDensity()) +
				" (" + std::to_string(sprite3d.getSubdividedMeshDensity() + 2u) + "x" + std::to_string(sprite3d.getSubdividedMeshDensity() + 2u) +
				" = " + std::to_string((sprite3d.getSubdividedMeshDensity() + 2u) * (sprite3d.getSubdividedMeshDensity() + 2u)) + " points)" +
			"\nMesh Density: " + std::to_string(sprite3d.getMeshDensity()) +
				" (" + std::to_string(sprite3d.getMeshDensity() + 2u) + "x" + std::to_string(sprite3d.getMeshDensity() + 2u) +
				" = " + std::to_string((sprite3d.getMeshDensity() + 2u) * (sprite3d.getMeshDensity() + 2u)) + " points)" +
			"\nDynamic Subdivision enabled: " + (sprite3d.getDynamicSubdivision() ? "true" : "false") +
			"\nMost Extreme Angle: " + std::to_string(sprite3d.getMostExtremeAngle()) +
			"\nDepth: " + std::to_string(sprite3d.getDepth()));

		// update display
		window.clear(sf::Color(64u, 64u, 64u));
		if (isVisible.sprite)
			window.draw(sprite);
		if (isVisible.sprite3d)
			window.draw(sprite3d);
		if (isVisible.spriteBounds)
			window.draw(boundsSprite);
		if (isVisible.sprite3dBounds)
			window.draw(boundsSprite3d);
		window.draw(spriteText);
		window.draw(sprite3dText);
		window.draw(feedbackText);
		window.display();
	}
}
