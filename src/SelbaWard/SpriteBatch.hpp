//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Sprite Batch
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

#ifndef SELBAWARD_SPRITEBATCH_HPP
#define SELBAWARD_SPRITEBATCH_HPP

#include "Common.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>

#include <functional>

namespace
{

const sf::Texture sfmlEmptyTexture{};

} // namespace

namespace selbaward
{

// Sprite Batch v1.1.0
class SpriteBatch : public sf::Drawable
{
public:
	SpriteBatch();

	void setTexture(const sf::Texture& texture);
	void setTexture();

	void setNumberOfSprites(std::size_t numberOfSprites);
	std::size_t getNumberOfSprites() const;

	std::size_t insertSprite(std::size_t insertIndex, std::size_t numberOfSprites = 1u, const sf::Sprite& sprite = sf::Sprite(sfmlEmptyTexture));
	std::size_t addSprite(std::size_t numberOfSprites = 1u, const sf::Sprite& sprite = sf::Sprite(sfmlEmptyTexture)); // to back

	std::size_t removeSprite(std::size_t removeIndex, std::size_t numberOfSprites = 1u);
	std::size_t removeSprite(std::size_t numberOfSprites = 1u); // from back

	void batchSprites(const std::vector<sf::Sprite>& sprites); // copy entire vector of sprites into batch and prepares it for entire update
	void batchSprites(const std::vector<sf::Sprite*>& sprites); // copy entire vector of sprites (from the pointers) into batch and prepares it for entire update

	void updateSprite(std::size_t index, const sf::Sprite& sprite);
	sf::Sprite getSprite(std::size_t index); // this sf::Sprite is a copy, not access to the internally stored one!
	sf::Sprite operator[](std::size_t index); // this sf::Sprite is a copy, not access to the internally stored one!

	void setOrderFunction(const std::function<bool(const sf::Sprite* a, const sf::Sprite* b)>& orderFunction); // sets order function and clears any manual order
	void setOrderFunction(); // clears order function and but does not clear any manual order

	void setOrder(const std::vector<std::size_t>& orderIndices); // sets manual order, which overrides order function (but doesn't clear the order function)
	void setOrder(); // clears manual order and reinstates order function, if available

	void clearAllOrdering(); // clears order function and also the manual order







	// standard SFML sprite methods

	// setters - absolute
	void setPosition(std::size_t index, sf::Vector2f position);
	void setOrigin(std::size_t index, sf::Vector2f origin);
	void setRotation(std::size_t index, float rotation);
	void setScale(std::size_t index, sf::Vector2f scale);
	void setScale(std::size_t index, float scale); // sets both x and y to the scale
	void setTextureRect(std::size_t index, sf::IntRect textureRect);
	void setColor(std::size_t index, const sf::Color& color);

	// setters - relative
	void move(std::size_t index, sf::Vector2f offset);
	void rotate(std::size_t index, sf::Angle angle);
	void scale(std::size_t index, sf::Vector2f factor);
	void scale(std::size_t index, float factor); // scales both x and y by the same factor

	// getters (that match the setters)
	sf::Vector2f getPosition(std::size_t index) const;
	sf::Vector2f getOrigin(std::size_t index) const;
	sf::Angle getRotation(std::size_t index) const;
	sf::Vector2f getScale(std::size_t index) const;
	sf::IntRect getTextureRect(std::size_t index) const;
	sf::Color getColor(std::size_t index) const;
	
	// getters (extra - no matching setter)
	sf::FloatRect getLocalBounds(std::size_t index) const;
	sf::FloatRect getGlobalBounds(std::size_t index) const;
	sf::Transform getTransform(std::size_t index) const;
	sf::Transform getInverseTransform(std::size_t index) const;



	// global sprite methods (affects all sprites) - relative
	void move(sf::Vector2f offset);
	void rotate(sf::Angle angle);
	void scale(sf::Vector2f factor);
	void scale(float factor); // scales both x and y by the same factor





















private:
	struct Sprite
	{
		bool isUpdateRequired{ false };
		sf::Sprite sprite{ sfmlEmptyTexture };
	};

	const sf::Texture* m_texture;

	std::function<bool(const sf::Sprite* a, const sf::Sprite* b)> m_orderFunction;
	std::vector<std::size_t> m_orderIndices;

	mutable std::vector<Sprite> m_sprites;
	mutable bool m_isGlobalUpdateRequired;
	mutable std::vector<sf::Vertex> m_vertices;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	void priv_testIsIndexValid(const std::size_t index) const;
	void priv_updateAll() const;
	void priv_updateRequired() const;
	void priv_updateQuad(const std::size_t quadIndex, const sf::Sprite* sprite) const;
};

} // namespace selbaward
#endif // SELBAWARD_SPRITEBATCH_HPP
