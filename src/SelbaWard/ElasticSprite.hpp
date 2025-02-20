//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Elastic Sprite
//
// Copyright(c) 2017-2025 M.J.Silk
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

#ifndef SELBAWARD_ELASTICSPRITE_HPP
#define SELBAWARD_ELASTICSPRITE_HPP

#include "Common.hpp"

namespace sf
{

class Texture;
template <class>
class Rect;
template <class>
class Vector2;

} // namespace sf

namespace selbaward
{

// SW Elastic Sprite v1.3.2
class ElasticSprite : public sf::Drawable, public sf::Transformable
{
public:
	ElasticSprite();
	explicit ElasticSprite(const sf::Texture& texture);
	ElasticSprite(const sf::Texture& texture, sf::FloatRect textureRect);

	void setTexture(const sf::Texture& texture, bool resetTextureRect = false);
	void setTexture();
	void setTextureRect(sf::FloatRect textureRect);
	const sf::Texture* getTexture() const;
	sf::FloatRect getTextureRect() const;

	void setTextureFlipX(bool textureFlipX);
	bool getTextureFlipX() const;
	void setTextureFlipY(bool textureFlipY);
	bool getTextureFlipY() const;

	bool setUseShader(bool useShader); // returns the actual state of "use shader" (may be different from parameter if setUseShader fails)
	bool getUseShader() const;

	void activateBilinearInterpolation();
	bool isActiveBilinearInterpolation() const;
	void activatePerspectiveInterpolation();
	bool isActivePerspectiveInterpolation() const;

	void setColor(sf::Color color);
	void setVertexColor(std::size_t vertexIndex, sf::Color color);
	sf::Color getColor() const; // returns the average of all four vertices' colours
	sf::Color getVertexColor(std::size_t vertexIndex) const;

	void resetVertexOffsets();
	void setVertexOffset(std::size_t vertexIndex, sf::Vector2f offset);
	sf::Vector2f getVertexOffset(std::size_t vertexIndex) const;

	sf::Vector2f getVertexLocalPosition(std::size_t vertexIndex) const;
	sf::Vector2f getVertexBaseLocalPosition(std::size_t vertexIndex) const;
	sf::Vector2f getVertexGlobalPosition(std::size_t vertexIndex) const;
	sf::Vector2f getVertexBaseGlobalPosition(std::size_t vertexIndex) const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getBaseLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	sf::FloatRect getBaseGlobalBounds() const;



private:
	mutable bool m_requiresVerticesUpdate;
	mutable std::vector<sf::Vertex> m_vertices;
	mutable std::vector<float> m_weights;
	mutable sf::FloatRect m_actualTextureRect;
	std::vector<sf::Vector2f> m_offsets;
	const sf::Texture* m_pTexture;
	sf::FloatRect m_baseTextureRect;
	bool m_useShader;
	bool m_usePerspectiveInterpolation;
	bool m_textureFlipX;
	bool m_textureFlipY;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void priv_updateVertices(sf::Transform Transform)  const;
	sf::Vector2f priv_getVertexBasePosition(const std::size_t vertexIndex) const;
};

} // namespace selbaward
#endif // SELBAWARD_ELASTICSPRITE_HPP
