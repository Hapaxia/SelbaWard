//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// NinePatch
//
// Copyright(c) 2015 M.J.Silk
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

#ifndef SELBAWARD_NINEPATCH_HPP
#define SELBAWARD_NINEPATCH_HPP

#include "Common.hpp"
#include <SFML/Graphics/Texture.hpp>

namespace selbaward
{

class NinePatch : public sf::Drawable
{
public:
	NinePatch();
	void setTexture(const sf::Texture& texture);
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize() const;

private:
	const sf::Vector2f m_trimAmount;
	const sf::PrimitiveType m_primitiveType;
	std::vector<sf::Vertex> m_vertices;
	const sf::Texture* m_texture;
	sf::Vector2f m_trimmedSize;
	sf::Vector2f m_size;
	sf::Vector2f m_scaleTopLeft;
	sf::Vector2f m_scaleBottomRight;

	virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void priv_updateVertices();
	void priv_updateVerticesPositions();
	void priv_updateVerticesTexCoords();
};

inline sf::Vector2f NinePatch::getSize() const
{
	return m_size;
}

} // namespace selbaward
#endif // SELBAWARD_NINEPATCH_HPP
