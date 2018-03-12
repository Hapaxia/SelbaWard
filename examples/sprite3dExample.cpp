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
//  F8              Resets mesh to minimal (two triangles) and turns off Dynamic Subdivision
//  F9              Toggle visibility of Sprite3d's bounds
//  F10             Toggle visibility of standard sprite's bounds
//  F11             Toggle visibility of Sprite3d
//  F12             Toggle visibility of standard sprite
//
//  Please note that this example makes use of C++11 features
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <iostream> // for std::cerr
#include <cmath>

#include <SelbaWard/Sprite3d.hpp>

int main()
{
	// set up window
	sf::RenderWindow window(sf::VideoMode(800, 600), "Sprite3d", sf::Style::Default);
	//window.setVerticalSyncEnabled(true);

	// load resources (textures and font)
	sf::Texture texture, frontTexture, backTexture;
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
	sf::Font font;
	if (!font.loadFromFile("resources/arial.ttf"))
		std::cerr << "Error loading font." << std::endl; // no need to exit if font fails; it will just not show any text

	// text headers
	sf::Text sprite3dText("Sprite3d", font, 48);
	sf::Text spriteText("Sprite", font, 48);
	sprite3dText.setOrigin(sprite3dText.getLocalBounds().left + sprite3dText.getLocalBounds().width / 2.f, 0.f);
	spriteText.setOrigin(spriteText.getLocalBounds().left + spriteText.getLocalBounds().width / 2.f, 0.f);
	sprite3dText.setPosition(sf::Vector2f(window.getSize().x * 0.25f, 0.f));
	spriteText.setPosition(sf::Vector2f(window.getSize().x * 0.75f, 0.f));

	// text feedback
	sf::Text feedbackText("FPS:\nVertices:\nSubdivision Level:\nSubdivided Mesh Density:\nMesh Density:\nDynamic Subdivision enabled:\nMost Extreme Angle:\nDepth:", font, 16);
	feedbackText.setOrigin(feedbackText.getLocalBounds().left, feedbackText.getLocalBounds().top + feedbackText.getLocalBounds().height);
	feedbackText.setPosition(sf::Vector2f(2.f, window.getSize().y - 1.f));

	// sprite3d
	sw::Sprite3d sprite3d(frontTexture, backTexture);
	sprite3d.setOrigin(sf::Vector2f(sprite3d.getLocalBounds().width, sprite3d.getLocalBounds().height) / 2.f);
	sprite3d.setPosition(sf::Vector2f(window.getSize().x * 0.25f, window.getSize().y / 2.f));
	//sprite3d.setFlipBack();
	//sprite3d.setMeshDensity(3); // = 5x5 points = 4x4 quads
	//sprite3d.setNumberOfPoints(25); // 5x5
	//sprite3d.setNumberOfQuads(16); // 4x4
	//sprite3d.setDynamicSubdivisionRange(3, 0); // max: 32x32 quads (4 subdivided 3 times is 32), min: 4x4 quads (zero subdivisions)

	// sprite
	//sf::Sprite sprite(sprite3d.getSprite());
	sf::Sprite sprite(texture);
	sprite.setOrigin(sf::Vector2f(sprite.getLocalBounds().width, sprite.getLocalBounds().height) / 2.f);
	sprite.setPosition(sf::Vector2f(window.getSize().x * 0.75f, window.getSize().y / 2.f));

	// bounds rectangles
	sf::RectangleShape boundsSprite3d;
	boundsSprite3d.setFillColor(sf::Color::Transparent);
	boundsSprite3d.setOutlineColor(sf::Color::Red);
	boundsSprite3d.setOutlineThickness(1.f);
	sf::RectangleShape boundsSprite = boundsSprite3d;

	// clock and pause
	sf::Clock clock;
	bool isPaused{ true };
	float time{ 0.f };

	// visibility flags
	struct
	{
		bool     sprite3d, sprite, sprite3dBounds, spriteBounds;
	} isVisible{ true, true, false, false };

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

				// control depth
				else if (event.key.code == sf::Keyboard::Equal)
					sprite3d.setDepth(sprite3d.getDepth() + 1.f);
				else if (event.key.code == sf::Keyboard::Dash)
					sprite3d.setDepth(sprite3d.getDepth() - 1.f);

				// control mesh density
				else if (event.key.code == sf::Keyboard::RBracket)
					sprite3d.setMeshDensity(sprite3d.getMeshDensity() + 1);
				else if (event.key.code == sf::Keyboard::LBracket)
				{
					if (sprite3d.getMeshDensity() > 0)
						sprite3d.setMeshDensity(sprite3d.getMeshDensity() - 1);
				}

				// control subdivision level
				else if (event.key.code == sf::Keyboard::Period)
					sprite3d.setSubdivision(sprite3d.getSubdivision() + 1);
				else if (event.key.code == sf::Keyboard::Comma)
				{
					if (sprite3d.getSubdivision() > 0)
						sprite3d.setSubdivision(sprite3d.getSubdivision() - 1);
				}

				// toggle dynamic subdivision
				else if (event.key.code == sf::Keyboard::Return)
					sprite3d.setDynamicSubdivision(!sprite3d.getDynamicSubdivision());

				// toggle back face is flipped state
				else if (event.key.code == sf::Keyboard::B)
					sprite3d.setFlipBack(!sprite3d.getFlipBack());

				// toggle bounds rectangles
				else if (event.key.code == sf::Keyboard::F9)
					isVisible.sprite3dBounds = !isVisible.sprite3dBounds;
				else if (event.key.code == sf::Keyboard::F10)
					isVisible.spriteBounds = !isVisible.spriteBounds;

				// toggle whether or not to draw the sprites
				else if (event.key.code == sf::Keyboard::F11)
					isVisible.sprite3d = !isVisible.sprite3d;
				else if (event.key.code == sf::Keyboard::F12)
					isVisible.sprite = !isVisible.sprite;

				// reset the mesh to minimal (also turns off dynamic subdivision)
				else if (event.key.code == sf::Keyboard::F8)
				{
					sprite3d.setDynamicSubdivision(false);
					sprite3d.minimalMesh();
				}

				// toggle texture usage
				else if (event.key.code == sf::Keyboard::Tab)
				{
					if (sprite3d.getTexture() != &frontTexture)
					{
						sprite3d.setTexture(frontTexture);
						sprite3d.setBackTexture(backTexture);
					}
					else
					{
						sprite3d.setTexture(sf::Texture());
						sprite3d.setBackTexture(sf::Texture());
					}
				}
			}
		}

		// update time
		float frameTime = 1.f;
		if (!isPaused)
		{
			frameTime = clock.restart().asSeconds();
			time += frameTime;
		}

		// update sprites
		sprite3d.setRotation({ time * 97.f, time * 42.f, time * 51.f });

		//sprite.setScale({ std::cos(time * 97.f * 3.141592653f / 180.f), std::cos(time * 42.f * 3.141592653f / 180.f) });
		sprite.setScale({ std::cos(time * 42 * 3.141592653f / 180.f), std::cos(time * 97 * 3.141592653f / 180.f) });
		sprite.setRotation(time * 51.f);
		// other sprite3d rotations for testing
		//sprite3d.setRotation(time * 60.f);
		/*
		sprite3d.setPitch(time * 60.f);
		sprite3d.setYaw(time * 40.f);
		*/
		//sprite3d.setYaw(time * 180.f);

		// update bounds rectangle
		boundsSprite3d.setPosition(sf::Vector2f(sprite3d.getGlobalBounds().left, sprite3d.getGlobalBounds().top));
		boundsSprite3d.setSize(sf::Vector2f(sprite3d.getGlobalBounds().width, sprite3d.getGlobalBounds().height));
		boundsSprite.setPosition(sf::Vector2f(sprite.getGlobalBounds().left, sprite.getGlobalBounds().top));
		boundsSprite.setSize(sf::Vector2f(sprite.getGlobalBounds().width, sprite.getGlobalBounds().height));

		// update depth text
		unsigned int subdividedMeshDensity = sprite3d.getMeshDensity();
		for (unsigned int i = 0; i < sprite3d.getSubdivision(); ++i)
			subdividedMeshDensity = subdividedMeshDensity * 2 + 1;
		unsigned int numberOfVerticesInSubdividedMesh = (subdividedMeshDensity * 2 + 5) * subdividedMeshDensity + 4;
		feedbackText.setString(
			"FPS: " + std::to_string(static_cast<unsigned int>(1.f / frameTime)) +
			"\nVertices: " + std::to_string(numberOfVerticesInSubdividedMesh) + " (" + std::to_string(numberOfVerticesInSubdividedMesh - 2) + " triangles)" +
			"\nSubdivision Level: " + std::to_string(sprite3d.getSubdivision()) +
			"\nSubdivided Mesh Density: " + std::to_string(sprite3d.getSubdividedMeshDensity()) + " (" + std::to_string(sprite3d.getSubdividedMeshDensity() + 2) + "x" + std::to_string(sprite3d.getSubdividedMeshDensity() + 2) + " = " + std::to_string((sprite3d.getSubdividedMeshDensity() + 2) * (sprite3d.getSubdividedMeshDensity() + 2)) + " points)" +
			"\nMesh Density: " + std::to_string(sprite3d.getMeshDensity()) + " (" + std::to_string(sprite3d.getMeshDensity() + 2) + "x" + std::to_string(sprite3d.getMeshDensity() + 2) + " = " + std::to_string((sprite3d.getMeshDensity() + 2) * (sprite3d.getMeshDensity() + 2)) + " points)" +
			"\nDynamic Subdivision enabled: " + (sprite3d.getDynamicSubdivision() ? "true" : "false") +
			"\nMost Extreme Angle: " + std::to_string(sprite3d.getMostExtremeAngle()) +
			"\nDepth: " + std::to_string(sprite3d.getDepth()));

		// update display
		window.clear(sf::Color(64, 64, 64));
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

	return EXIT_SUCCESS;
}
