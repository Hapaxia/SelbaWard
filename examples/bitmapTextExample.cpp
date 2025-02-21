//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Bitmap Text *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//    Keys:
//
//  Escape          Quit
//  Space           Swap text string
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>

#include <SelbaWard/BitmapFont.hpp> // not actually required as BitmapText includes it. we're just being transparently explicit here
#include <SelbaWard/BitmapText.hpp>

int main()
{
	sf::Texture fontSheetTexture{};
	if (!fontSheetTexture.loadFromFile("resources/Selba Ward Bitmap Font 0001.png"))
	{
		std::cerr << "Could not load font sheet texture." << std::endl;
		return EXIT_FAILURE;
	}

	sw::BitmapFont font{};
	sw::BitmapText text{};

	try
	{
		// standard setup for a font
		font.setExternalTexture(fontSheetTexture);
		font.setNumberOfTilesPerRow(16u);
		font.setDefaultTextureRect({ { 0, 0 }, { 8, 8 } });

		// customisation for the specific font ("Selba Ward Bitmap Font 0001.png")

		// customise texture rects for 'j' and 'z' as the glyph for j spills into z
		font.setTextureRect({ { 80, 48 }, { 8, 9 } }, 'j');
		font.setTextureRect({ { 80, 57 }, { 8, 7 } }, 'z');
		font.setBaseline(-1, 'z');

		// starting values
		//font.setBaselines(-1, 256); // this is the default (for all default texture rects)
		font.setWidths(4, 256u);
		font.setStartXs(2, 256u);

		// alpha numeric
		font.setBaseline(-4, "gypq");
		font.setBaseline(-3, 'j');

		font.setWidth(1, "ijl");
		font.setWidth(3, "1I");
		font.setWidth(5, "NQTVWXYZmvwxz");
		font.setWidth(6, "M");

		font.setStartX(1, "M");
		font.setStartX(3, "1I");
		font.setStartX(4, "ijl");

		// punctuation
		font.setBaseline(-3, ",;");

		font.setWidth(1, "!,.:;|");
		font.setWidth(2, "'()[]`");
		font.setWidth(3, "/<>\\");
		font.setWidth(5, "#$*+-=?^\"");
		font.setWidth(6, "&");
		font.setWidth(7, "%@");

		font.setStartX(1, "%&@");
		font.setStartX(3, "'()/<>[]`\\");
		font.setStartX(4, "!,.:;|");

		// used kerning
		font.setKerning(-1, "Iv");
		font.setKerning(-1, "fo");
	}
	catch (sw::Exception& e)
	{
		std::cerr << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	text.setBitmapFont(font);
	const std::string prefixString = "This is a bitmap font. ";
	const std::string defaultString = "012 Str;:zingy! qu,ic(k)jumps 57";
	const std::string xxxyyyString = "xxxyyyiijiizzJJIIvvvwwyyxxzzz";
	text.setString(prefixString + defaultString);
	text.setPosition({ 20.f, 50.f });
	text.setScale(2u); // scale so we can see the pixels (only accepts "std::size_t" - single std::size_t, two std::size_t, sf::Vector2<std::size_t>)
	//text.Transformable::setScale(3.5f, 9.75f); // it's still possible to scale by fractional amounts by explicitly calling the Transformable method
	//text.setTracking(2); // base spacing between characters. default is 1

	sf::RenderWindow window(sf::VideoMode({ 550u, 100u }), "Bitmap Text example", sf::Style::Default);
	window.setFramerateLimit(20u);
	while (window.isOpen())
	{
		while (const std::optional event{ window.pollEvent() })
		{
			if (event->is<sf::Event::Closed>())
				window.close();
			else if (const auto keyPressed{ event->getIf<sf::Event::KeyPressed>() })
			{
				if (keyPressed->code == sf::Keyboard::Key::Space)
				{
					if (text.getString() != prefixString + xxxyyyString)
						text.setString(prefixString + xxxyyyString);
					else
						text.setString(prefixString + defaultString);
				}
			}
		}
		window.clear(sf::Color(0u, 64u, 32u));
		window.draw(text);
		window.display();
	}
}
