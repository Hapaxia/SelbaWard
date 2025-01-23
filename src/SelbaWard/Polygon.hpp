//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Polygon
//
// Copyright(c) 2022-2025 M.J.Silk
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

#ifndef SELBAWARD_POLYGON_HPP
#define SELBAWARD_POLYGON_HPP

#include "Common.hpp"
#include <array>

namespace selbaward
{

// SW Polygon v1.4.0
class Polygon : public sf::Drawable, public sf::Transformable
{
public:
	enum class TriangulationMethod
	{
		BasicEarClip,
		EarClip,
	};
	enum class MeshRefinementMethod
	{
		None,
	};

	Polygon();
	Polygon(std::initializer_list<sf::Vector2f> list); // pass vertices' positions (sf::Vector2f) to the constructor (sets size automatically)
	Polygon(const Polygon& polygon);
	Polygon& operator=(const Polygon& polygon);

	void update();

	sf::Vertex& operator[] (std::size_t index); // direct access to the polygon's vertices (sf::Vertex) using the [] operator. no checks are performed. using with an invalid index results in undefined behaviour

	void setColor(sf::Color color);
	sf::Color getColor();

	void setTriangulationMethod(TriangulationMethod triangulationMethod);
	TriangulationMethod getTriangulationMethod() const;
	void setMeshRefinementMethod(MeshRefinementMethod meshRefinementMethod);
	MeshRefinementMethod getMeshRefinementMethod() const;

	void setReverseDirection(bool reverseDirection);
	bool getReverseDirection() const;

	void reserveVertices(std::size_t numberOfVertices);

	void setNumberOfVertices(std::size_t numberOfVertices);
	std::size_t getNumberOfVertices() const;

	void setVertexPosition(std::size_t index, sf::Vector2f position);
	sf::Vector2f getVertexPosition(std::size_t index) const;

	void setVertexColor(std::size_t index, sf::Color color);
	sf::Color getVertexColor(std::size_t index) const;

	void setVertexTexCoords(std::size_t index, sf::Vector2f texCoords);
	sf::Vector2f getVertexTexCoords(std::size_t index) const;

	void setTexture(const sf::Texture& texture); // activate texture (ignored for wireframe)
	void setTexture(); // de-activate/reset ("un-set") texture

	void setTriangleLimit(std::size_t triangleLimit);
	std::size_t getTriangleLimit() const;

	void setShowWireframe(bool showWireframe);
	bool getShowWireframe() const;

	void setWireframeColor(sf::Color wireframeColor);
	sf::Color getWireframeColor() const;

	float getPerimeter() const;
	float getArea() const;

	bool isPointInside(sf::Vector2f point) const;

	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;

	sf::Vector2f getCentroid() const; // ignores holes - gives decent representation (averaged points of outer)
	sf::Vector2f getCenterOfMass() const; // uses each point of each triangle, weighted by triangle area - represents actual "centre of mass"

	void reverseVertices();

	void importVertexPositions(const std::vector<sf::Vector2f>& position);
	std::vector<sf::Vector2f> exportVertexPositions() const;
	std::vector<sf::Vector2f> exportVertexPositionsOuterOnly() const;
	std::vector<sf::Vector2f> exportVertexPositionsHoleOnly(std::size_t holeIndex) const;
	std::vector<sf::Vector2f> exportTriangulatedPositions() const;
	std::vector<sf::Vector2f> exportWireframePositions() const;

	// holes must not overlap and must be specified in opposite direction to outer polygon
	void addHoleStartIndex(std::size_t index);
	void clearHoleStartIndices();
	void setHoleStartIndices(const std::vector<std::size_t>& indices);
	void setNumberOfHoles(std::size_t numberOfHoles);
	void setHoleStartIndex(std::size_t holeIndex, std::size_t holeStartIndex);
	std::size_t getNumberOfHoles() const;
	std::size_t getHoleStartIndex(std::size_t holeIndex) const; // hole index indentifies the hole. returned value is the vertex index of the start of that hole. "number of vertices" is returned if there are no holes or hole does not exist.







private:
	using TriangleIndices = std::array<std::size_t, 3u>;

	const sf::Texture* m_texture;

	std::vector<sf::Vertex> m_vertices;
	std::vector<TriangleIndices> m_triangles;
	std::vector<sf::Vertex> m_outputVertices;
	std::vector<std::size_t> m_holeStartIndices;
	sf::Color m_color;

	bool m_showWireframe;
	std::vector<sf::Vertex> m_wireframeVertices;
	sf::Color m_wireframeColor;

	TriangulationMethod m_triangulationMethod;
	MeshRefinementMethod m_meshRefinementMethod;

	std::size_t m_triangleLimit;

	bool m_reverseDirection;

	void draw(sf::RenderTarget&, sf::RenderStates) const override final;
	void priv_update();
	void priv_updateOutputVertices();
	void priv_triangulate();
	void priv_triangulateEarClip();
	void priv_triangulateBasicEarClip();
	bool priv_isValidVertexIndex(std::size_t vertexIndex) const;
	bool priv_isValidHoleIndex(std::size_t holeIndex) const;
	bool priv_testVertexIndex(std::size_t vertexIndex, const std::string& exceptionMessage) const;
	bool priv_testHoleIndex(std::size_t holeIndex, const std::string& exceptionMessage) const;
	void priv_buildWireframe();
};

inline sf::Vertex& Polygon::operator[] (const std::size_t index)
{
	return m_vertices[index];
}

} // namespace selbaward
#endif // SELBAWARD_POLYGON_HPP
