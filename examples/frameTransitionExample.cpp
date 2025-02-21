//////////////////////////////////////////////////////////////////////////////////////////////
//
//  Selba Ward - Frame Transition *EXAMPLE*
//
//  by Hapaxia (https://github.com/Hapaxia)
//
//
//  Demonstates multiple variations of transitions.
//
//
//    Controls:
//
//  Escape key          Quit
//  Right               Assigns "right" direction to all TexCrop transitions
//  Left                Assigns "left" direction to all TexCrop transitions
//  Down                Assigns "down" direction to all TexCrop transitions
//  Up                  Assigns "up" direction to all TexCrop transitions
//
//////////////////////////////////////////////////////////////////////////////////////////////

#include <SFML/Graphics.hpp>
#include <SelbaWard.hpp>

using FT = sw::FrameTransition;

int main()
{
	sf::Texture textureA{};
	sf::Texture textureB{};
	if (!(textureA.loadFromFile("resources/uv map.jpg") && textureB.loadFromFile("resources/Simple Tileset.png")))
		return EXIT_FAILURE;

	std::vector<FT> frameTransitions(25u);

	// row 1, left: "slide in"
	frameTransitions[0u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_None | FT::T::TexCrop_B_Start);
	frameTransitions[1u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Start | FT::T::TexCrop_B_Start);
	frameTransitions[2u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_End | FT::T::TexCrop_B_Start);
	frameTransitions[3u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Both | FT::T::TexCrop_B_Start);
	// row 1, right: "reveal"
	frameTransitions[4u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_None | FT::T::TexCrop_B_End);
	frameTransitions[5u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Start | FT::T::TexCrop_B_End);
	frameTransitions[6u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_End | FT::T::TexCrop_B_End);
	frameTransitions[7u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Both | FT::T::TexCrop_B_End);
	// row 2, left: "stretch in"
	frameTransitions[8u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_None | FT::T::TexCrop_B_None);
	frameTransitions[9u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Start | FT::T::TexCrop_B_None);
	frameTransitions[10u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_End | FT::T::TexCrop_B_None);
	frameTransitions[11u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Both | FT::T::TexCrop_B_None);
	// row 2, right: "slow slide during reveal"
	frameTransitions[12u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_None | FT::T::TexCrop_B_Both);
	frameTransitions[13u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Start | FT::T::TexCrop_B_Both);
	frameTransitions[14u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_End | FT::T::TexCrop_B_Both);
	frameTransitions[15u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Both | FT::T::TexCrop_B_Both);
	// row 3: "zooms"
	frameTransitions[16u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_None | FT::T::Zoom_B_None);
	frameTransitions[17u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_In | FT::T::Zoom_B_Out);
	frameTransitions[18u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_In | FT::T::Zoom_B_None);
	frameTransitions[19u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_None | FT::T::Zoom_B_Out);
	frameTransitions[20u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_InOut | FT::T::Zoom_B_InOut);
	frameTransitions[21u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_OutIn | FT::T::Zoom_B_OutIn | FT::T::ZoomType_A_Scale | FT::T::ZoomType_B_Scale);
	frameTransitions[22u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_In | FT::T::Zoom_B_In | FT::T::ZoomType_A_Crop | FT::T::ZoomType_B_Scale);
	frameTransitions[23u].setTransition(FT::T::Type_Zoom | FT::T::Zoom_A_Out | FT::T::Zoom_B_Out | FT::T::ZoomType_A_Scale | FT::T::ZoomType_B_Crop);
	// row 4: "shuffle"
	frameTransitions[24u].setTransition(FT::T::Type_TexCrop | FT::T::TexCrop_A_Shuffle | FT::T::TexCrop_B_Shuffle);

	for (std::size_t i{ 0u }; i < frameTransitions.size(); ++i)
	{
		frameTransitions[i].setPosition({ 12.f + 140.f * (i % 8u), 12.f + 140.f * (i / 8u) });
		frameTransitions[i].setSize({ 128.f, 128.f });
		if (i > 15u && i < 24u) // for zoom type transitions, we set the parameters and apply the linear fade
		{
			frameTransitions[i].setParameter1(0.1f); // A frame scale (for Zoom)
			frameTransitions[i].setParameter2(0.1f); // B frame scale (for Zoom)
			frameTransitions[i].setTransition(frameTransitions[i].getTransition() | FT::T::Fade_A_Linear | FT::T::Fade_B_Linear);
		}
	}

	sf::RenderWindow window(sf::VideoMode({ 1132u, 572u }), "Selba Ward - Frame Transition example");
	sf::Clock clock;
	while (window.isOpen())
	{
		const float current{ clock.getElapsedTime().asSeconds() / 2.f }; // 1 transition per 2 seconds
		const float currentTrunc{ std::trunc(current) };
		const float ratio{ current - currentTrunc };
		const bool reverse{ (std::lround(currentTrunc) % 2) > 0 }; // this allows us transition back in the same way
		for (auto& frameTransition : frameTransitions)
		{
			frameTransition.setTexture(FT::FrameId::A, reverse ? textureB : textureA, true);
			frameTransition.setTexture(FT::FrameId::B, reverse ? textureA : textureB, true);
			frameTransition.setRatio(ratio);
		}

		window.clear();
		for (auto& frameTransition : frameTransitions)
			window.draw(frameTransition);
		window.display();

		while (const auto event{ window.pollEvent() })
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
				case sf::Keyboard::Key::Right:
					for (auto& frameTransition : frameTransitions)
					{
						if ((frameTransition.getTransition() & FT::T::Type) == FT::T::Type_TexCrop)
							frameTransition.setTransition((frameTransition.getTransition() & ~FT::T::Direction) | FT::T::Direction_Right);
					}
					break;
				case sf::Keyboard::Key::Left:
					for (auto& frameTransition : frameTransitions)
					{
						if ((frameTransition.getTransition() & FT::T::Type) == FT::T::Type_TexCrop)
							frameTransition.setTransition((frameTransition.getTransition() & ~FT::T::Direction) | FT::T::Direction_Left);
					}
					break;
				case sf::Keyboard::Key::Down:
					for (auto& frameTransition : frameTransitions)
					{
						if ((frameTransition.getTransition() & FT::T::Type) == FT::T::Type_TexCrop)
							frameTransition.setTransition((frameTransition.getTransition() & ~FT::T::Direction) | FT::T::Direction_Down);
					}
					break;
				case sf::Keyboard::Key::Up:
					for (auto& frameTransition : frameTransitions)
					{
						if ((frameTransition.getTransition() & FT::T::Type) == FT::T::Type_TexCrop)
							frameTransition.setTransition((frameTransition.getTransition() & ~FT::T::Direction) | FT::T::Direction_Up);
					}
					break;
				}
			}
		}
	}
}
