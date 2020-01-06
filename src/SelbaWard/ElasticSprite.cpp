//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Elastic Sprite
//
// Copyright(c) 2017-2020 M.J.Silk
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
#include <cmath>

namespace
{

const sf::PrimitiveType primitiveType{ sf::PrimitiveType::Quads };
bool areShadersLoaded{ false };
sf::Shader bilinearShader;
sf::Shader perspectiveShader;

const std::string bilinearFragmentShaderCode
{
	"#version 110\n\nuniform bool useTexture;\nuniform sampler2D texture;"
	"\nuniform int renderTargetHeight;\nuniform vec2 v0;\nuniform vec2 v1;"
	"\nuniform vec2 v2;\nuniform vec2 v3;\nuniform float textureRectLeftRat"
	"io;\nuniform float textureRectTopRatio;\nuniform float textureRectWid"
	"thRatio;\nuniform float textureRectHeightRatio;\nuniform vec4 c0;\nun"
	"iform vec4 c1;\nuniform vec4 c2;\nuniform vec4 c3;\n\nvec2 linesInter"
	"section(vec2 aStart, vec2 aEnd, vec2 bStart, vec2 bEnd)\n{\nvec2 a ="
	" aEnd - aStart;\nvec2 b = bEnd - bStart;\nfloat aAngle = atan(a.y, "
	"a.x);\nfloat bAngle = atan(b.y, b.x);\nif (abs(aAngle - bAngle) < 0"
	".01)\n{\na = mix(aEnd, bEnd, 0.0001) - aStart;\nb = mix(bEnd, aE"
	"nd, 0.0001) - bStart;\n}\nvec2 c = aStart - bStart;\nfloat alpha ="
	" ((b.x * c.y) - (b.y * c.x)) / ((b.y * a.x) - (b.x * a.y));\nreturn "
	"aStart + (a * alpha);\n}\n\nvoid main()\n{\nvec2 p = vec2(gl_FragCoo"
	"rd.x, (float(renderTargetHeight) - gl_FragCoord.y));\nvec2 o = lines"
	"Intersection(v0, v3, v1, v2);\nvec2 n = linesIntersection(v1, v0, v2"
	", v3);\nvec2 l = linesIntersection(o, p, v0, v1);\nvec2 m = linesIn"
	"tersection(o, p, v3, v2);\nvec2 j = linesIntersection(n, p, v0, v3);"
	"\nvec2 k = linesIntersection(n, p, v2, v1);\nvec2 ratioCoord = vec2"
	"(distance(p, l) / distance(m, l), distance(p, j) / distance(k, j));\n"
	"vec4 color = mix(mix(c0, c3, ratioCoord.x), mix(c1, c2, ratioCoord.x"
	"), ratioCoord.y);\nif (useTexture)\n{\nvec2 texCoord = vec2(ratio"
	"Coord.x * textureRectWidthRatio + textureRectLeftRatio, ratioCoord.y "
	"* textureRectHeightRatio + textureRectTopRatio);\nvec4 pixel = text"
	"ure2D(texture, texCoord);\ngl_FragColor = color * pixel;\n}\nelse"
	"\ngl_FragColor = color;\n}\n"
};

const std::string perspectiveVertexShaderCode
{
	"#version 110\n\nuniform vec4 c0;\nuniform vec4 c1;\nuniform vec4 c2;"
	"\nuniform vec4 c3;\nuniform float w0;\nuniform float w1;\nuniform floa"
	"t w2;\nuniform float w3;\n\nvoid main()\n{\nint vertexNumber = 0;\n"
	"if (gl_Color.r > 0.5)\nvertexNumber = 1;\nelse if (gl_Color.g > 0."
	"5)\nvertexNumber = 2;\nelse if (gl_Color.b > 0.5)\nvertexNumber "
	"= 3;\n\nvec4 color;\nfloat weight;\nif (vertexNumber == 0)\n{\n"
	"color = c0;\nweight = w0;\n}\nelse if (vertexNumber == 1)\n{\n"
	"color = c1;\nweight = w1;\n}\nelse if (vertexNumber == 2)\n{\n"
	"color = c2;\nweight = w2;\n}\nelse if (vertexNumber == 3)\n{\n"
	"color = c3;\nweight = w3;\n}\ngl_Position = gl_ModelViewProjecti"
	"onMatrix * gl_Vertex;\ngl_TexCoord[0] = gl_TextureMatrix[0] * gl_Mul"
	"tiTexCoord0;\ngl_TexCoord[0].z = weight;\ngl_FrontColor = color;\n}"
	"\n"
};

const std::string perspectiveFragmentShaderCode
{
	"#version 110\n\nuniform bool useTexture;\nuniform sampler2D texture;"
	"\n\nvoid main()\n{\nvec4 color = gl_Color;\nif (useTexture)\n{\nv"
	"ec2 texCoord = gl_TexCoord[0].xy / gl_TexCoord[0].z;\ngl_FragColor "
	"= color * texture2D(texture, texCoord);\n}\nelse\ngl_FragColor = "
	"color;\n}\n"
};

void loadShader()
{
	if (!areShadersLoaded &&
		bilinearShader.loadFromMemory(bilinearFragmentShaderCode, sf::Shader::Fragment) &&
		perspectiveShader.loadFromMemory(perspectiveVertexShaderCode, perspectiveFragmentShaderCode))
		areShadersLoaded = true;
}

inline bool isValidVertexIndex(const unsigned int vertexIndex)
{
	return (vertexIndex < 4);
}

inline sf::Vector2f linesIntersection(const sf::Vector2f aStart, const sf::Vector2f aEnd, const sf::Vector2f bStart, const sf::Vector2f bEnd)
{
	const sf::Vector2f a{ aEnd - aStart };
	const sf::Vector2f b{ bEnd - bStart };
	const sf::Vector2f c{ aStart - bStart };
	const float alpha{ ((b.x * c.y) - (b.y * c.x)) / ((b.y * a.x) - (b.x * a.y)) };
	return aStart + (a * alpha);
}

inline float distanceBetweenPoints(const sf::Vector2f a, const sf::Vector2f b)
{
	const sf::Vector2f c{ a - b };
	return std::sqrt(c.x * c.x + c.y * c.y);
}

inline sf::Color encodeFloatAsColor(const float f)
{
	return
	{
		static_cast<sf::Uint8>(static_cast<unsigned int>(f / 256) & 0xFF),
		static_cast<sf::Uint8>(static_cast<unsigned int>(f) & 0xFF),
		static_cast<sf::Uint8>(static_cast<unsigned int>(f * 256) & 0xFF),
		0u
	};
}

} // namespace

namespace selbaward
{

ElasticSprite::ElasticSprite()
	: m_requiresVerticesUpdate{ false }
	, m_vertices(4)
	, m_weights(0)
	, m_offsets(4)
	, m_pTexture{ nullptr }
	, m_textureRect()
	, m_useShader(sf::Shader::isAvailable())
	, m_usePerspectiveInterpolation{ false }
{
	if (m_useShader)
		loadShader();
}

ElasticSprite::ElasticSprite(const sf::Texture& texture)
	: ElasticSprite()
{
	setTexture(texture, true);
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
	m_requiresVerticesUpdate = true;
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
	m_requiresVerticesUpdate = true;
	return m_useShader = (useShader && sf::Shader::isAvailable());
}

bool ElasticSprite::getUseShader() const
{
	return m_useShader;
}

void ElasticSprite::activateBilinearInterpolation()
{
	m_usePerspectiveInterpolation = false;
	m_weights.clear();
	m_requiresVerticesUpdate = true;
}

bool ElasticSprite::isActiveBilinearInterpolation() const
{
	return !m_usePerspectiveInterpolation;
}

void ElasticSprite::activatePerspectiveInterpolation()
{
	m_usePerspectiveInterpolation = true;
	m_weights.resize(4);
	m_requiresVerticesUpdate = true;
}

bool ElasticSprite::isActivePerspectiveInterpolation() const
{
	return m_usePerspectiveInterpolation;
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
		priv_updateVertices(sf::Transform::Identity);

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
		priv_updateVertices(states.transform);
	states.transform = sf::Transform::Identity;

	if (!m_useShader)
	{
		states.texture = m_pTexture;
		target.draw(m_vertices.data(), 4u, primitiveType, states);
	}
	else
	{
		const bool isTextureAvailable{ (m_pTexture != nullptr) };
		if (m_usePerspectiveInterpolation)
		{
			std::vector<sf::Color> colors(4);
			for (std::size_t i{ 0u }; i < 4u; ++i)
				colors[i] = m_vertices[i].color;
			perspectiveShader.setUniform("useTexture", isTextureAvailable);
			if (isTextureAvailable)
				perspectiveShader.setUniform("texture", *m_pTexture);
			perspectiveShader.setUniform("c0", sf::Glsl::Vec4(m_vertices[0].color));
			perspectiveShader.setUniform("c1", sf::Glsl::Vec4(m_vertices[1].color));
			perspectiveShader.setUniform("c2", sf::Glsl::Vec4(m_vertices[2].color));
			perspectiveShader.setUniform("c3", sf::Glsl::Vec4(m_vertices[3].color));
			perspectiveShader.setUniform("w0", m_weights[0]);
			perspectiveShader.setUniform("w1", m_weights[1]);
			perspectiveShader.setUniform("w2", m_weights[2]);
			perspectiveShader.setUniform("w3", m_weights[3]);
			m_vertices[0].color = sf::Color::Black;
			m_vertices[1].color = sf::Color::Red;
			m_vertices[2].color = sf::Color::Green;
			m_vertices[3].color = sf::Color::Blue;
			states.shader = &perspectiveShader;
			target.draw(m_vertices.data(), 4u, primitiveType, states);
			for (std::size_t i{ 0u }; i < 4u; ++i)
				m_vertices[i].color = colors[i];
		}
		else
		{
			bilinearShader.setUniform("useTexture", isTextureAvailable);
			if (isTextureAvailable)
			{
				bilinearShader.setUniform("texture", *m_pTexture);
				const sf::Vector2f textureSize(m_pTexture->getSize());
				bilinearShader.setUniform("textureRectLeftRatio", m_textureRect.left / textureSize.x);
				bilinearShader.setUniform("textureRectTopRatio", m_textureRect.top / textureSize.y);
				bilinearShader.setUniform("textureRectWidthRatio", m_textureRect.width / textureSize.x);
				bilinearShader.setUniform("textureRectHeightRatio", m_textureRect.height / textureSize.y);
			}
			bilinearShader.setUniform("renderTargetHeight", static_cast<int>(target.getSize().y));
			bilinearShader.setUniform("v0", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[0].position)));
			bilinearShader.setUniform("v1", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[1].position)));
			bilinearShader.setUniform("v2", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[2].position)));
			bilinearShader.setUniform("v3", sf::Glsl::Vec2(target.mapCoordsToPixel(m_vertices[3].position)));
			bilinearShader.setUniform("c0", sf::Glsl::Vec4(m_vertices[0].color));
			bilinearShader.setUniform("c1", sf::Glsl::Vec4(m_vertices[1].color));
			bilinearShader.setUniform("c2", sf::Glsl::Vec4(m_vertices[2].color));
			bilinearShader.setUniform("c3", sf::Glsl::Vec4(m_vertices[3].color));
			states.shader = &bilinearShader;
			target.draw(m_vertices.data(), 4u, primitiveType, states);
		}
	}
}

void ElasticSprite::priv_updateVertices(sf::Transform transform) const
{
	m_requiresVerticesUpdate = false;

	transform *= getTransform();

	for (unsigned int i{ 0u }; i < 4; ++i)
		m_vertices[i].position = transform.transformPoint(m_offsets[i] + priv_getVertexBasePosition(i));

	if (m_useShader && m_usePerspectiveInterpolation && m_pTexture != nullptr)
	{
		const sf::Vector2f intersection{ linesIntersection(m_vertices[0].position, m_vertices[2].position, m_vertices[1].position, m_vertices[3].position) };
		const float distanceToIntersection0{ distanceBetweenPoints(m_vertices[0].position, intersection) };
		const float distanceToIntersection1{ distanceBetweenPoints(m_vertices[1].position, intersection) };
		const float distanceToIntersection2{ distanceBetweenPoints(m_vertices[2].position, intersection) };
		const float distanceToIntersection3{ distanceBetweenPoints(m_vertices[3].position, intersection) };
		m_weights[0] = (distanceToIntersection0 + distanceToIntersection2) / distanceToIntersection2;
		m_weights[1] = (distanceToIntersection1 + distanceToIntersection3) / distanceToIntersection3;
		m_weights[2] = (distanceToIntersection2 + distanceToIntersection0) / distanceToIntersection0;
		m_weights[3] = (distanceToIntersection3 + distanceToIntersection1) / distanceToIntersection1;

		const sf::Vector2f textureSize(m_pTexture->getSize());
		m_vertices[0].texCoords = { m_weights[0] * (m_textureRect.left / textureSize.x), m_weights[0] * (m_textureRect.top / textureSize.y) };
		m_vertices[1].texCoords = { m_weights[1] * (m_textureRect.left / textureSize.x), m_weights[1] * ((m_textureRect.top + m_textureRect.height) / textureSize.y) };
		m_vertices[2].texCoords = { m_weights[2] * ((m_textureRect.left + m_textureRect.width) / textureSize.x), m_weights[2] * ((m_textureRect.top + m_textureRect.height) / textureSize.y) };
		m_vertices[3].texCoords = { m_weights[3] * ((m_textureRect.left + m_textureRect.width) / textureSize.x), m_weights[3] * (m_textureRect.top / textureSize.y) };
	}
	else
	{
		m_vertices[0].texCoords = { m_textureRect.left, m_textureRect.top };
		m_vertices[2].texCoords = { m_textureRect.left + m_textureRect.width, m_textureRect.top + m_textureRect.height };
		m_vertices[1].texCoords = { m_vertices[0].texCoords.x, m_vertices[2].texCoords.y };
		m_vertices[3].texCoords = { m_vertices[2].texCoords.x, m_vertices[0].texCoords.y };
	}
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
