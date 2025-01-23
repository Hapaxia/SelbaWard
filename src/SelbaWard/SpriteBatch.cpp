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

#include "SpriteBatch.hpp"

#include <assert.h>

namespace
{

const std::string exceptionPrefix{ "Sprite Batch: " };

constexpr std::size_t numberOfVerticesPerQuad{ 6u };

} // namespace

namespace selbaward
{

SpriteBatch::SpriteBatch()
	: m_texture{ nullptr }
	, m_orderFunction{ nullptr }
	, m_orderIndices()
	, m_sprites()
	, m_isGlobalUpdateRequired { false }
	, m_vertices()
{

}

void SpriteBatch::setTexture(const sf::Texture& texture)
{
	m_texture = &texture;
}

void SpriteBatch::setTexture()
{
	m_texture = nullptr;
}

void SpriteBatch::setNumberOfSprites(const std::size_t numberOfSprites)
{
	if (numberOfSprites == m_sprites.size())
		return;

	m_sprites.resize(numberOfSprites);
	m_isGlobalUpdateRequired = true;
}

std::size_t SpriteBatch::getNumberOfSprites() const
{
	return m_sprites.size();
}

std::size_t SpriteBatch::insertSprite(std::size_t insertIndex, const std::size_t numberOfSprites, const sf::Sprite& sprite)
{
	if (numberOfSprites == 0u)
		return m_sprites.size();

	if (insertIndex > m_sprites.size())
		insertIndex = m_sprites.size();
	m_sprites.insert(m_sprites.begin() + insertIndex, numberOfSprites, Sprite{ false, sprite });
	m_isGlobalUpdateRequired = true;
	return m_sprites.size();
}

std::size_t SpriteBatch::addSprite(const std::size_t numberOfSprites, const sf::Sprite& sprite)
{
	return insertSprite(m_sprites.size(), numberOfSprites, sprite);
}

std::size_t SpriteBatch::removeSprite(const std::size_t removeIndex, const std::size_t numberOfSprites)
{
	if (numberOfSprites == 0u)
		return m_sprites.size();

	if (m_sprites.empty())
		throw Exception(exceptionPrefix + "Cannot remove sprite; no sprites available.");

	assert(removeIndex < m_sprites.size());
	if (removeIndex >= m_sprites.size())
		throw Exception(exceptionPrefix + "Cannot remove sprite; invalid sprite index.");

	std::size_t endIndex{ removeIndex + numberOfSprites };
	if (endIndex > m_sprites.size())
		endIndex = m_sprites.size();
	m_sprites.erase(m_sprites.begin() + removeIndex, m_sprites.begin() + endIndex);
	m_isGlobalUpdateRequired = true;
	return m_sprites.size();
}

std::size_t SpriteBatch::removeSprite(const std::size_t numberOfSprites)
{
	if (m_sprites.empty())
		throw Exception(exceptionPrefix + "Cannot remove sprite; no sprites available.");

	return removeSprite(m_sprites.size() - 1u, numberOfSprites);
}

void SpriteBatch::batchSprites(const std::vector<sf::Sprite>& sprites)
{
	const std::size_t numberOfSprites{ sprites.size() };
	m_sprites.resize(numberOfSprites);
	for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		m_sprites[i] = { false, sprites[i] };
	m_isGlobalUpdateRequired = true;
}

void SpriteBatch::batchSprites(const std::vector<sf::Sprite*>& sprites)
{
	const std::size_t numberOfSprites{ sprites.size() };
	m_sprites.resize(numberOfSprites);
	for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		m_sprites[i] = { false, *sprites[i] };
	m_isGlobalUpdateRequired = true;
}

void SpriteBatch::updateSprite(const std::size_t index, const sf::Sprite& sprite)
{
	priv_testIsIndexValid(index);

	m_sprites[index].isUpdateRequired = true;
	m_sprites[index].sprite = sprite;
}

sf::Sprite SpriteBatch::getSprite(const std::size_t index)
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite;
}

sf::Sprite SpriteBatch::operator[](const std::size_t index)
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite;
}

void SpriteBatch::setOrderFunction(const std::function<bool(const sf::Sprite* a, const sf::Sprite* b)>& orderFunction)
{
	m_orderFunction = orderFunction;
	m_orderIndices.clear();
}

void SpriteBatch::setOrderFunction()
{
	m_orderFunction = nullptr;
}

void SpriteBatch::setOrder(const std::vector<std::size_t>& orderIndices)
{
	m_orderIndices = orderIndices;
}

void SpriteBatch::setOrder()
{
	m_orderIndices.clear();
}

void SpriteBatch::clearAllOrdering()
{
	setOrderFunction();
	setOrder();
}

void SpriteBatch::setPosition(const std::size_t index, const sf::Vector2f position)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setPosition(position);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::setOrigin(const std::size_t index, const sf::Vector2f origin)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setOrigin(origin);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::setRotation(const std::size_t index, const float rotation)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setRotation(sf::degrees(rotation));
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::setScale(const std::size_t index, const sf::Vector2f scale)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setScale(scale);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::setScale(const std::size_t index, const float scale)
{
	setScale(index, { scale, scale });
}

void SpriteBatch::setTextureRect(const std::size_t index, const sf::IntRect textureRect)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setTextureRect(textureRect);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::setColor(const std::size_t index, const sf::Color& color)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.setColor(color);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::move(const std::size_t index, const sf::Vector2f offset)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.move(offset);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::rotate(const std::size_t index, const float angle)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.rotate(sf::degrees(angle));
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::scale(const std::size_t index, const sf::Vector2f factor)
{
	priv_testIsIndexValid(index);
	m_sprites[index].sprite.scale(factor);
	m_sprites[index].isUpdateRequired = true;
}

void SpriteBatch::scale(const std::size_t index, const float factor)
{
	scale(index, { factor, factor });
}

sf::Vector2f SpriteBatch::getPosition(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getPosition();
}

sf::Vector2f SpriteBatch::getOrigin(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getOrigin();
}

float SpriteBatch::getRotation(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getRotation().asDegrees();
}

sf::Vector2f SpriteBatch::getScale(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getScale();
}

sf::IntRect SpriteBatch::getTextureRect(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getTextureRect();
}

sf::Color SpriteBatch::getColor(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getColor();
}

sf::FloatRect SpriteBatch::getLocalBounds(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getLocalBounds();
}

sf::FloatRect SpriteBatch::getGlobalBounds(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getGlobalBounds();
}

sf::Transform SpriteBatch::getTransform(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getTransform();
}

sf::Transform SpriteBatch::getInverseTransform(const std::size_t index) const
{
	priv_testIsIndexValid(index);
	return m_sprites[index].sprite.getInverseTransform();
}

void SpriteBatch::move(const sf::Vector2f offset)
{
	for (auto& sprite : m_sprites)
		sprite.sprite.move(offset);
	m_isGlobalUpdateRequired = true;
}

void SpriteBatch::rotate(const float angle)
{
	for (auto& sprite : m_sprites)
		sprite.sprite.rotate(sf::degrees(angle));
	m_isGlobalUpdateRequired = true;
}

void SpriteBatch::scale(const sf::Vector2f factor)
{
	for (auto& sprite : m_sprites)
		sprite.sprite.scale(factor);
	m_isGlobalUpdateRequired = true;
}

void SpriteBatch::scale(const float factor)
{
	scale({ factor, factor });
	m_isGlobalUpdateRequired = true;
}


















// PRIVATE

void SpriteBatch::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if ((m_orderFunction != nullptr) || (m_isGlobalUpdateRequired) || (!m_orderIndices.empty()))
		priv_updateAll();
	else
		priv_updateRequired();

	states.texture = m_texture;
	target.draw(m_vertices.data(), m_vertices.size(), sf::PrimitiveType::Triangles, states);
}

void SpriteBatch::priv_testIsIndexValid(const std::size_t index) const
{
	const std::size_t numberOfSprites{ m_sprites.size() };
	assert(index < numberOfSprites);
	if (index >= numberOfSprites)
		throw Exception(exceptionPrefix + "Sprite index invalid.");
}

void SpriteBatch::priv_updateAll() const
{
	if (m_sprites.empty())
	{
		m_vertices.clear();
		return;
	}

	const std::size_t numberOfSprites{ m_sprites.size() };
	m_vertices.resize(numberOfSprites * numberOfVerticesPerQuad);

	if (!m_orderIndices.empty())
	{
		// create a vector of orig(ordered) indices
		std::vector<std::size_t> origIndices(numberOfSprites);
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
			origIndices[i] = i;

		// copy order indices in reverse
		std::vector<std::size_t> orderIndices{ m_orderIndices };
		std::sort(orderIndices.rbegin(), orderIndices.rend());

		// remove all indices in order indices from orig(ordered) indices
		for (auto& orderIndex : orderIndices)
			origIndices.erase(origIndices.begin() + orderIndex);

		// rebuild order indices (copy) to contain order indices followed by remaining orig(ordered) indices
		orderIndices.resize(numberOfSprites);
		const std::size_t sizeOfOrderIndices{ m_orderIndices.size() };
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		{
			if (i < sizeOfOrderIndices)
				orderIndices[i] = m_orderIndices[i];
			else
				orderIndices[i] = origIndices[i - sizeOfOrderIndices];
		}

		// update quads
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		{
			priv_updateQuad(i, &(m_sprites[orderIndices[i]].sprite));
			m_sprites[i].isUpdateRequired = false; // this "i" doesn't need to match as we're clearing the update for all anyway
		}
	}
	else if (m_orderFunction != nullptr)
	{
		// create vector of pointers to sort
		std::vector<sf::Sprite*> pointers(numberOfSprites);
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
			pointers[i] = &(m_sprites[i].sprite);

		// sort pointers using custom order function
		std::sort(pointers.begin(), pointers.end(), m_orderFunction);

		// update quads
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		{
			priv_updateQuad(i, pointers[i]);
			m_sprites[i].isUpdateRequired = false;
		}
	}
	else
	{
		// update quads
		for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
		{
			priv_updateQuad(i, &(m_sprites[i].sprite));
			m_sprites[i].isUpdateRequired = false;
		}
	}

	m_isGlobalUpdateRequired = false;
}

void SpriteBatch::priv_updateRequired() const
{
	const std::size_t numberOfSprites{ m_sprites.size() };
	for (std::size_t i{ 0u }; i < numberOfSprites; ++i)
	{
		if (m_sprites[i].isUpdateRequired)
		{
			priv_updateQuad(i, &(m_sprites[i].sprite));
			m_sprites[i].isUpdateRequired = false;
		}
	}
}

void SpriteBatch::priv_updateQuad(const std::size_t quadIndex, const sf::Sprite* sprite) const
{
	const std::size_t startVertex{ quadIndex * numberOfVerticesPerQuad };

	const sf::Transform transform{ sprite->getTransform() };
	const sf::Color color{ sprite->getColor() };
	const sf::IntRect rect{ sprite->getTextureRect() };

	sf::Vector2f shapeTopLeft{ 0.f, 0.f };
	sf::Vector2f shapeBottomRight(rect.size);
	sf::Vector2f shapeTopRight{ shapeBottomRight.x, shapeTopLeft.y };
	sf::Vector2f shapeBottomLeft{ shapeTopLeft.x, shapeBottomRight.y };
	sf::Vector2f textureTopLeft(rect.position);
	sf::Vector2f textureBottomRight{ textureTopLeft + shapeBottomRight };
	sf::Vector2f textureTopRight{ textureBottomRight.x, textureTopLeft.y };
	sf::Vector2f textureBottomLeft{ textureTopLeft.x, textureBottomRight.y };


	shapeTopLeft = transform.transformPoint(shapeTopLeft);
	shapeBottomRight = transform.transformPoint(shapeBottomRight);
	shapeTopRight = transform.transformPoint(shapeTopRight);
	shapeBottomLeft = transform.transformPoint(shapeBottomLeft);

	m_vertices[startVertex + 0u].position = shapeTopLeft;
	m_vertices[startVertex + 0u].texCoords = textureTopLeft;
	m_vertices[startVertex + 0u].color = color;
	m_vertices[startVertex + 1u].position = shapeBottomLeft;
	m_vertices[startVertex + 1u].texCoords = textureBottomLeft;
	m_vertices[startVertex + 1u].color = color;
	m_vertices[startVertex + 2u].position = shapeBottomRight;
	m_vertices[startVertex + 2u].texCoords = textureBottomRight;
	m_vertices[startVertex + 2u].color = color;
	m_vertices[startVertex + 5u].position = shapeTopRight;
	m_vertices[startVertex + 5u].texCoords = textureTopRight;
	m_vertices[startVertex + 5u].color = color;

	m_vertices[startVertex + 3u] = m_vertices[startVertex + 0u];
	m_vertices[startVertex + 4u] = m_vertices[startVertex + 2u];
}

} // namespace selbaward
