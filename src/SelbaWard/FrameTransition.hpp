//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Frame Transition
//
// Copyright(c) 2023-2025 M.J.Silk
//
// This software is provided 'as-is', without any express or implied
// warranty. In no event will the authors be held liable for any damages
// arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions :
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software.If you use this software
// in a product, an acknowledgment in the product documentation would be
// appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
// M.J.Silk
// MJSilk2@gmail.com
//
//////////////////////////////////////////////////////////////////////////////

#ifndef SELBAWARD_FRAMETRANSITION_HPP
#define SELBAWARD_FRAMETRANSITION_HPP

#include "Common.hpp"

#include <SFML/Graphics/Texture.hpp>

namespace selbaward
{

// SW Frame Transition v1.0.2
class FrameTransition : public sf::Drawable, public sf::Transformable
{
public:
	enum class FrameId
	{
		A, // "start" or "source" frame, shown fully (usually) at 0 ratio
		B, // "end" or "destination" frame, shown fully (usually) at 1 ratio
	};

	enum class T
	{
		// prefixes-only are bitmasks that cover all of that group
		// e.g. Type can be used to check types only

		None = 0u, // used as a general "none" (useful for non-overly-verbose conditionals)



		// "_" in names are separators that show groups and each group can be (bit)masked using the name before the "_".
		// e.g. TexCrop_A masks TexCrop_A_None, TexCrop_A_Start, TexCrop_A_End... 
		//   whereas TexCrop masks all of TexCrop_A as well as TexCrop_B

		// can combine a type with any - or all - of the groups it "uses"
		// e.g. with a TexCrop type, you can set the TexCrop group, the Direction group, both, or neither.
		//     and, within those groups, you can set A, B, both, or neither (if the group allows separate A and B settings e.g. Direction cannot be set separately for A and B)

		// Types
		// 
		// [] = global (available for all types)
		// "" = set outside of the Transition ID (T) (e.g. "parameters" are set via setParameter1 and setParameter2 methods)
		// 
		// TexCrop
		//   uses:
		//     TexCrop_
		//     Direction_
		//     [Fade_]
		// 
		// Zoom
		//   uses:
		//     Zoom_
		//     ZoomType_
		//     [Fade_]
		//     "parameters"

		Type_TexCrop  = 0u,
		Type_Zoom     = 1u << 0u,
		Type          = 1u << 0u, // 1 bit



		// global
		// applies to all types

		// fade
		// can be set separately for A and B
		// "none" : keeps colours exactly as they are - does not modify alpha for fades
		// "off" : sets alpha to zero, allowing that frame to be invisible for the entire time
		// "step" : for A, starts at normal colour and changes alpha to zero half way through; for B, start with alpha of zero and resets it half way through
		// "linear" : linearly interpolates alpha. for A, starts at normal colour and interpolates alpha to zero; for B, starts with alpha of zero and interpolates to normal colour's alpha

		Fade_A_None   = 0u,
		Fade_A_Off    = 1u << 1u,
		Fade_A_Step   = 2u << 1u,
		Fade_A_Linear = 3u << 1u,
		Fade_A        = 3u << 1u, // 2 bits
		Fade_B_None   = 0u,
		Fade_B_Off    = 1u << 3u,
		Fade_B_Step   = 2u << 3u,
		Fade_B_Linear = 3u << 3u,
		Fade_B        = 3u << 3u, // 2 bits
		Fade          = Fade_A | Fade_B, // 4 bits



		// tex crop
		// "none" scales the texture: the entire texture (rect) is fit inside the quad. (others keep the same size texture but crop it)
		// "start" and "end" are which side crops
		// "start" is the beginning side of the direction (e.g. left side when direction is "right")
		// "end" is the end side of the direction (e.g. right side when direction is "right")
		// "both" crops both opposite sides (start and end) equally and keeps the center in the center of the quad
		// "shuffle" is a special form that is designed to animate the shuffling of the frames (A slides "behind" during 2nd half, B slides "out" during 1st half)
		// front edge is left for horizontal directions (left/right) and top for vertical directions (up/down)
		// back edge is right for horizontal directions (left/right) and bottom for vertical directions (up/down)
		//
		// direction:
		// this is the direction of the separating edge (the line that separates the two quads - they don't overlap)
		// right: vertical separator starts from the left and moves to the right
		// left: vertical separator starts from the right and moves to the left
		// down: horizontal separator starts from the top and moves down to the bottom
		// up: horizontal separator starts from the bottom and moves up to the top

		TexCrop_A_None    = 0u,
		TexCrop_A_Start   = 1u << 5u,
		TexCrop_A_End     = 2u << 5u,
		TexCrop_A_Both    = TexCrop_A_Start | TexCrop_A_End,
		TexCrop_A_Shuffle = 4u << 5u,
		TexCrop_A         = 7u << 5u, // 3 bits
		TexCrop_B_None    = 0u,
		TexCrop_B_Start   = 1u << 8u,
		TexCrop_B_End     = 2u << 8u,
		TexCrop_B_Both    = TexCrop_B_Start | TexCrop_B_End,
		TexCrop_B_Shuffle = 4u << 8u,
		TexCrop_B         = 7u << 8u, // 3 bits
		TexCrop           = TexCrop_A | TexCrop_B, // 6 bits

		Direction_Right = 0u,
		Direction_Left  = 1u << 11u,
		Direction_Down  = 2u << 11u,
		Direction_Up    = 3u << 11u,
		Direction       = 3u << 11u, // 2 bits



		// zooming (can also be set separately for A and B):
		// zooming in means the image will be larger at the end
		// zooming in means the image will be smaller at the end
		// zooming in&out means the image will be larger in the middle and back to normal by the end
		// zooming out&in means the image will be smaller in the middle and back to normal by the end
		// 
		// zoomtype is whether is crops the texture rect or scales the quad to zoom in and/or out (can be set separately for A and B)
		// note that some combinations can cause an start or end state that isn't the full texture rect/quad of a frame (e.g. zoom out with scale ends with small, scaled B)
		//
		// parameters:
		// parameter1: "A scale" (scales the quad or texture rect for zooming in or out) for A. this should be in the range 0-1
		// parameter2: "B scale" (scales the quad or texture rect for zooming in or out) for B. this should be in the range 0-1
		// low values (under 0.1) are only recommended for scale zoom types (not for crop zoom types) but experimentation is encouraged


		Zoom_A_None  = 0u,
		Zoom_A_In    = 1u << 5u,
		Zoom_A_Out   = 2u << 5u,
		Zoom_A_InOut = 3u << 5u,
		Zoom_A_OutIn = 4u << 5u,
		Zoom_A       = 7u << 5u, // 3 bits
		Zoom_B_None  = 0u,
		Zoom_B_In    = 1u << 8u,
		Zoom_B_Out   = 2u << 8u,
		Zoom_B_InOut = 3u << 8u,
		Zoom_B_OutIn = 4u << 8u,
		Zoom_B       = 7u << 8u, // 3 bits
		Zoom         = Zoom_A | Zoom_B, // 6 bits

		ZoomType_A_Crop  = 0u,
		ZoomType_A_Scale = 1u << 11u,
		ZoomType_A       = 1u << 11u, // 1 bit
		ZoomType_B_Crop  = 0u,
		ZoomType_B_Scale = 1u << 12u,
		ZoomType_B       = 1u << 12u, // 1 bit
		ZoomType         = ZoomType_A | ZoomType_B, // 2 bits
	};

	// creation and size
	FrameTransition(sf::Vector2f size = { 64.f, 8.f });
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;

	// draw order
	void setDrawOrderToAOverB(); // this should be always be used except for specific reasons. this places A over B and can optimise draw calls if possible.
	void setDrawOrderToBOverA(); // this forces B to be drawn over A. always takes 2 separate draw calls (one for A and one for B)

	// transition ID (all transitions information; the T bitwise enum class)
	void setTransition(T transitionId);
	T getTransition() const;

	// transition ratio/alpha (0-1)
	void setRatio(float ratio);
	float getRatio() const;
	void setPercentage(float percentage); // (0-100 percent instead of 0-1 ratio/alpha)
	float getPercentage() const; // (0-100 percent instead of 0-1 ratio/alpha)
	template <class Type>
	void setFromValueInRange(const Type& value, const Type& min, const Type& max); // (min-max)
	template <class Type>
	void setFromValueInRange(const Type& value, const Type& range); // (0-range)

	// visual representation
	void setColors(sf::Color colorA, sf::Color colorB);
	void setColors(sf::Color color);
	void setColor(FrameId frameId, sf::Color color);
	sf::Color getColor(FrameId frameId) const;

	// texturing
	void setTextures(const sf::Texture& textureA, const sf::Texture& textureB);
	void setTextures(const sf::Texture& texture);
	void setTextures(); // clear/nullify textures
	void setTexture(FrameId frameId, const sf::Texture& texture, bool resetRect = false);
	void setTexture(FrameId frameId); // clear/nullify texture
	void setTextureRect(FrameId, const sf::IntRect& textureRectangle = sf::IntRect{});
	const sf::Texture& getTexture(FrameId frameId) const;

	// parameters
	void setParameter1(float parameterValue = 0.f);
	void setParameter2(float parameterValue = 0.f);
	float getParameter1() const;
	float getParameter2() const;
	void resetParameters();

	// bounds
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

private:
	float m_ratio;
	sf::Vector2f m_size;
	T m_transitionId;
	bool m_drawAOverB;
	float m_parameter1;
	float m_parameter2;

	struct Frame
	{
		const sf::Texture* pTexture{};
		sf::IntRect textureRect{};
		sf::Color color{ sf::Color::White };
		std::size_t numberOfVertices{};
	};
	Frame m_frameA;
	Frame m_frameB;

	struct Quad
	{
		sf::Vertex topLeft;
		sf::Vertex bottomLeft;
		sf::Vertex bottomRight;
		sf::Vertex topRight;

		void setColor(const sf::Color color)
		{
			topLeft.color = color;
			bottomLeft.color = color;
			bottomRight.color = color;
			topRight.color = color;
		}
	};

	mutable bool m_isUpdateRequired;
	mutable std::vector<sf::Vertex> m_vertices;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void priv_update() const;
	void priv_updateFromTexCrop() const;
	void priv_updateFromZoom() const;
	void priv_addQuad(
		const std::size_t startVertex,
		const sf::Vertex topLeft,
		const sf::Vertex bottomLeft,
		const sf::Vertex bottomRight,
		const sf::Vertex topRight) const;
	void priv_addQuad(const std::size_t startVertex, const Quad& quad) const;
	void priv_setQuadPositionsFromRect(Quad& quad, sf::FloatRect rect) const;
	void priv_setQuadTextureCoordsFromRect(Quad& quad, sf::FloatRect rect) const;
	const Frame& priv_getFrame(const FrameId frameId) const;
	Frame& priv_getFrame(const FrameId frameId);
};

inline FrameTransition::T operator|(FrameTransition::T a, FrameTransition::T b)
{
	return static_cast<FrameTransition::T>(static_cast<unsigned int>(a) | static_cast<unsigned int>(b));
}
inline FrameTransition::T operator^(FrameTransition::T a, FrameTransition::T b)
{
	return static_cast<FrameTransition::T>(static_cast<unsigned int>(a) ^ static_cast<unsigned int>(b));
}
inline FrameTransition::T operator&(FrameTransition::T a, FrameTransition::T b)
{
	return static_cast<FrameTransition::T>(static_cast<unsigned int>(a) & static_cast<unsigned int>(b));
}
inline FrameTransition::T operator~(FrameTransition::T a)
{
	return static_cast<FrameTransition::T>(~static_cast<unsigned int>(a));
}
inline FrameTransition::T operator>>(FrameTransition::T a, std::size_t bits)
{
	return static_cast<FrameTransition::T>(static_cast<unsigned int>(a) >> bits);
}
inline FrameTransition::T operator<<(FrameTransition::T a, std::size_t bits)
{
	return static_cast<FrameTransition::T>(static_cast<unsigned int>(a) << bits);
}

inline sf::Vector2f FrameTransition::getSize() const
{
	return m_size;
}
inline FrameTransition::T FrameTransition::getTransition() const
{
	return m_transitionId;
}
inline float FrameTransition::getRatio() const
{
	return m_ratio;
}
inline float FrameTransition::getPercentage() const
{
	return m_ratio * 100.f;
}

template <class Type>
void FrameTransition::setFromValueInRange(const Type& value, const Type& minimum, const Type& maximum)
{
	setRatio(static_cast<float>(value - minimum) / static_cast<float>(maximum - minimum));
}
template <class Type>
void FrameTransition::setFromValueInRange(const Type& value, const Type& range)
{
	setRatio(static_cast<float>(value) / static_cast<float>(range));
}

} // namespace selbaward
#endif // SELBAWARD_PROGRESSBAR_HPP
