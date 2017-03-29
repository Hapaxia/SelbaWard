//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Elastic Sprite
//
// Copyright(c) 2017 M.J.Silk
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

#include "ElasticSprite.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Shader.hpp>
#include <SFML/Graphics/Transform.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Vertex.hpp>
#include <assert.h>

namespace
{

const sf::PrimitiveType primitiveType{ sf::PrimitiveType::Quads };
bool isShaderLoaded{ false };
sf::Shader shader;

const std::string shaderCode
{
	"uniform bool useTexture;\nuniform sampler2D texture;\nuniform int ren"
	"derTargetHeight;\nuniform vec2 v0;\nuniform vec2 v1;\nuniform vec2 v2"
	";\nuniform vec2 v3;\nuniform float textureRectLeftRatio;\nuniform flo"
	"at textureRectTopRatio;\nuniform float textureRectWidthRatio;\nunifor"
	"m float textureRectHeightRatio;\nuniform vec4 c0;\nuniform vec4 c1;\n"
	"uniform vec4 c2;\nuniform vec4 c3;\n\nvec2 linesIntersection(vec2 aSt"
	"art, vec2 aEnd, vec2 bStart, vec2 bEnd)\n{\nvec2 a = aEnd - aStart;"
	"\nvec2 b = bEnd - bStart;\nfloat aAngle = atan(a.y, a.x);\nfloat bA"
	"ngle = atan(b.y, b.x);\nif (abs(aAngle - bAngle) < 0.01)\n{\na = "
	"mix(aEnd, bEnd, 0.0001) - aStart;\nb = mix(bEnd, aEnd, 0.0001) - bS"
	"tart;\n}\nvec2 c = aStart - bStart;\nfloat alpha = ((b.x * c.y) - "
	"(b.y * c.x)) / ((b.y * a.x) - (b.x * a.y));\nreturn aStart + (a * al"
	"pha);\n}\n\nvoid main()\n{\nvec2 p = vec2(gl_FragCoord.x, (renderTar"
	"getHeight - gl_FragCoord.y));\nvec2 o = linesIntersection(v0, v3, v1"
	", v2);\nvec2 n = linesIntersection(v1, v0, v2, v3);\nvec2 l = lines"
	"Intersection(o, p, v0, v1);\nvec2 m = linesIntersection(o, p, v3, v2"
	");\nvec2 j = linesIntersection(n, p, v0, v3);\nvec2 k = linesInters"
	"ection(n, p, v2, v1);\nvec2 ratioCoord = vec2(distance(p, l) / dista"
	"nce(m, l), distance(p, j) / distance(k, j));\nvec4 color = mix(mix(c"
	"0, c3, ratioCoord.x), mix(c1, c2, ratioCoord.x), ratioCoord.y);\nif "
	"(useTexture)\n{\nvec2 texCoord = vec2(ratioCoord.x * textureRectWi"
	"dthRatio + textureRectLeftRatio, ratioCoord.y * textureRectHeightRati"
	"o + textureRectTopRatio);\nvec4 pixel = texture2D(texture, texCoord"
	");\ngl_FragColor = color * pixel;\n}\nelse\ngl_FragColor = colo"
	"r;\n}\n"
};

void loadShader()
{
	if (!isShaderLoaded && shader.loadFromMemory(shaderCode, sf::Shader::Fragment))
		isShaderLoaded = true;
}

inline bool isValidVertexIndex(const unsigned int vertexIndex)
{
	return (vertexIndex >= 0 && vertexIndex < 4);
}

} // namespace

namespace selbaward
{

ElasticSprite::ElasticSprite()
	: m_vertices(4)
	, m_offsets(4)
	, m_useShader(sf::Shader::isAvailable())
	, m_requiresVerticesUpdate(false)
	, m_textureRect()
	, m_pTexture(nullptr)
{
	if (m_useShader)
		loadShader();
}

ElasticSprite::ElasticSprite(const sf::Texture& texture)
	: ElasticSprite()
{
	setTexture(texture);
}

ElasticSprite::ElasticSprite(const sf::Texture& texture, const sf::FloatRect textureRect)
	: ElasticSprite()
{
	setTexture(texture);
	setTextureRect(textureRect);
}

void ElasticSprite::setTexture(const sf::Texture& texture, const bool resetTextureRect)
{
	if (resetTextureRect)
	{
		resetVertexOffsets();
		setTextureRect({ { 0.f, 0.f }, sf::Vector2f(texture.getSize()) });
	}
	m_pTexture = &texture;
}

void ElasticSprite::setTexture()
{
	m_pTexture = nullptr;
}

void ElasticSprite::setTextureRect(const sf::FloatRect textureRect)
{
	m_textureRect = textureRect;
	m_requiresVerticesUpdate = true;
}

const sf::Texture* ElasticSprite::getTexture() const
{
	return m_pTexture;
}

sf::FloatRect ElasticSprite::getTextureRect() const
{
	return m_textureRect;
}

bool ElasticSprite::setUseShader(const bool useShader)
{
	return m_useShader = (useShader && sf::Shader::isAvailable());
}

bool ElasticSprite::getUseShader() const
{
	return m_useShader;
}

void ElasticSprite::setVertexOffset(const unsigned int vertexIndex, const sf::Vector2f offset)
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	m_offsets[vertexIndex] = offset;
	m_requiresVerticesUpdate = true;
}

sf::Vector2f ElasticSprite::getVertexOffset(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return m_offsets[vertexIndex];
}

void ElasticSprite::setColor(const sf::Color color)
{
	for (auto& vertex : m_vertices)
		vertex.color = color;
	m_requiresVerticesUpdate = true;
}

void ElasticSprite::setVertexColor(const unsigned int vertexIndex, const sf::Color color)
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	m_vertices[vertexIndex].color = color;
	m_requiresVerticesUpdate = true;
}

sf::Color ElasticSprite::getColor() const
{
	const unsigned int totalR{ static_cast<unsigned int>(m_vertices[0].color.r) + m_vertices[1].color.r + m_vertices[2].color.r + m_vertices[3].color.r };
	const unsigned int totalG{ static_cast<unsigned int>(m_vertices[0].color.g) + m_vertices[1].color.g + m_vertices[2].color.g + m_vertices[3].color.g };
	const unsigned int totalB{ static_cast<unsigned int>(m_vertices[0].color.b) + m_vertices[1].color.b + m_vertices[2].color.b + m_vertices[3].color.b };
	const unsigned int totalA{ static_cast<unsigned int>(m_vertices[0].color.a) + m_vertices[1].color.a + m_vertices[2].color.a + m_vertices[3].color.a };
	return{ static_cast<sf::Uint8>(totalR / 4), static_cast<sf::Uint8>(totalG / 4), static_cast<sf::Uint8>(totalB / 4), static_cast<sf::Uint8>(totalA / 4) };
}

sf::Color ElasticSprite::getVertexColor(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return m_vertices[vertexIndex].color;
}

void ElasticSprite::resetVertexOffsets()
{
	setVertexOffset(0, { 0.f, 0.f });
	setVertexOffset(1, { 0.f, 0.f });
	setVertexOffset(2, { 0.f, 0.f });
	setVertexOffset(3, { 0.f, 0.f });
}

sf::Vector2f ElasticSprite::getVertexLocalPosition(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return getTransform().transformPoint(priv_getVertexBasePosition(vertexIndex) + m_offsets[vertexIndex]);
}

sf::Vector2f ElasticSprite::getVertexBaseLocalPosition(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return getTransform().transformPoint(priv_getVertexBasePosition(vertexIndex));
}

sf::Vector2f ElasticSprite::getVertexGlobalPosition(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return getTransform().transformPoint(priv_getVertexBasePosition(vertexIndex) + m_offsets[vertexIndex]);
}

sf::Vector2f ElasticSprite::getVertexBaseGlobalPosition(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	return getTransform().transformPoint(priv_getVertexBasePosition(vertexIndex));
}

sf::FloatRect ElasticSprite::getLocalBounds() const
{
	sf::Vector2f topLeft{ m_offsets[0] };
	sf::Vector2f bottomRight{ topLeft };

	for (unsigned int i{ 1u }; i < 4; ++i)
	{
		const sf::Vector2f vertex{ priv_getVertexBasePosition(i) + m_offsets[i] };
		if (vertex.x < topLeft.x)
			topLeft.x = vertex.x;
		else if (vertex.x > bottomRight.x)
			bottomRight.x = vertex.x;
		if (vertex.y < topLeft.y)
			topLeft.y = vertex.y;
		else if (vertex.y > bottomRight.y)
			bottomRight.y = vertex.y;
	}
	return{ topLeft, bottomRight - topLeft };
}

sf::FloatRect ElasticSprite::getBaseLocalBounds() const
{
	return{ { 0.f, 0.f }, priv_getVertexBasePosition(2) };
}

sf::FloatRect ElasticSprite::getGlobalBounds() const
{
	if (m_requiresVerticesUpdate)
		priv_updateVertices();

	sf::Vector2f topLeft{ m_vertices[0].position };
	sf::Vector2f bottomRight{ topLeft };

	for (unsigned int i{ 1u }; i < 4; ++i)
	{
		const sf::Vector2f transformedVertex{ m_vertices[i].position };
		if (transformedVertex.x < topLeft.x)
			topLeft.x = transformedVertex.x;
		else if (transformedVertex.x > bottomRight.x)
			bottomRight.x = transformedVertex.x;
		if (transformedVertex.y < topLeft.y)
			topLeft.y = transformedVertex.y;
		else if (transformedVertex.y > bottomRight.y)
			bottomRight.y = transformedVertex.y;
	}
	return{ topLeft, bottomRight - topLeft };
}

sf::FloatRect ElasticSprite::getBaseGlobalBounds() const
{
	return getTransform().transformRect(getBaseLocalBounds());
}



// PRIVATE

void ElasticSprite::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_requiresVerticesUpdate)
		priv_updateVertices();

	if (!m_useShader)
		states.texture = m_pTexture;
	else
	{
		if (m_pTexture == nullptr)
			shader.setUniform("useTexture", false);
		else
		{
			shader.setUniform("useTexture", true);
			shader.setUniform("texture", *m_pTexture);
			const sf::Vector2f textureSize(m_pTexture->getSize());
			shader.setUniform("textureRectLeftRatio", m_textureRect.left / textureSize.x);
			shader.setUniform("textureRectTopRatio", m_textureRect.top / textureSize.y);
			shader.setUniform("textureRectWidthRatio", m_textureRect.width / textureSize.x);
			shader.setUniform("textureRectHeightRatio", m_textureRect.height / textureSize.y);
		}
		shader.setUniform("renderTargetHeight", static_cast<int>(target.getSize().y));
		shader.setUniform("v0", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[0].position)));
		shader.setUniform("v1", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[1].position)));
		shader.setUniform("v2", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[2].position)));
		shader.setUniform("v3", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[3].position)));
		shader.setUniform("c0", sf::Glsl::Vec4(m_vertices[0].color));
		shader.setUniform("c1", sf::Glsl::Vec4(m_vertices[1].color));
		shader.setUniform("c2", sf::Glsl::Vec4(m_vertices[2].color));
		shader.setUniform("c3", sf::Glsl::Vec4(m_vertices[3].color));
		states.shader = &shader;
	}

	target.draw(m_vertices.data(), m_vertices.size(), primitiveType, states);
}

void ElasticSprite::priv_updateVertices() const
{
	m_requiresVerticesUpdate = false;

	const sf::Transform transform{ getTransform() };

	for (unsigned int i{ 0u }; i < 4; ++i)
		m_vertices[i].position = transform.transformPoint(m_offsets[i] + priv_getVertexBasePosition(i));

	m_vertices[0].texCoords = { m_textureRect.left, m_textureRect.top };
	m_vertices[2].texCoords = { m_textureRect.left + m_textureRect.width, m_textureRect.top + m_textureRect.height };
	m_vertices[1].texCoords = { m_vertices[0].texCoords.x, m_vertices[2].texCoords.y };
	m_vertices[3].texCoords = { m_vertices[2].texCoords.x, m_vertices[0].texCoords.y };
}

sf::Vector2f ElasticSprite::priv_getVertexBasePosition(const unsigned int vertexIndex) const
{
	// must be valid vertex index
	assert(isValidVertexIndex(vertexIndex));

	switch (vertexIndex)
	{
	case 1u:
		return sf::Vector2f(0.f, m_textureRect.height);
	case 2u:
		return sf::Vector2f(m_textureRect.width, m_textureRect.height);
	case 3u:
		return sf::Vector2f(m_textureRect.width, 0.f);
	case 0u:
	default:
		return { 0.f, 0.f };
	}
}

} // namespace selbaward
