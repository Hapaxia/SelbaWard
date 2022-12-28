//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Polygon
//
// Copyright(c) 2022 M.J.Silk
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

// SW Polygon v1.1
class Polygon : public sf::Drawable, public sf::Transformable
{
public:
	enum class TriangulationMethod
	{
		BasicEarClip,
	};
	enum class MeshRefinementMethod
	{
		None,
	};

	Polygon();
	void update();

	void setColor(sf::Color color);
	sf::Color getColor();

	void setTriangulationMethod(TriangulationMethod triangulationMethod);
	TriangulationMethod getTriangulationMethod() const;
	void setMeshRefinementMethod(MeshRefinementMethod meshRefinementMethod);
	MeshRefinementMethod getMeshRefinementMethod() const;

	void reserveVertices(std::size_t numberOfVertices);

	void setNumberOfVertices(std::size_t numberOfVertices);
	std::size_t getNumberOfVertices() const;

	void setVertexPosition(std::size_t index, sf::Vector2f position);
	sf::Vector2f getVertexPosition(std::size_t index) const;

	void reverseVertices();

	void importVertexPositions(const std::vector<sf::Vector2f>& position);
	std::vector<sf::Vector2f> exportTriangulatedPositions() const;




private:
	using TriangleIndices = std::array<std::size_t, 3u>;

	std::vector<sf::Vertex> m_vertices;
	std::vector<TriangleIndices> m_triangles;
	std::vector<sf::Vertex> m_outputVertices;
	sf::Color m_color;

	TriangulationMethod m_triangulationMethod;
	MeshRefinementMethod m_meshRefinementMethod;

	const bool m_throwExceptions;

	virtual void draw(sf::RenderTarget&, sf::RenderStates) const;
	void priv_update();
	void priv_updateOutputVertices();
	void priv_triangulate();
	void priv_triangulateBasicEarClip();
	bool priv_isValidVertexIndex(std::size_t vertexIndex) const;
	bool priv_testVertexIndex(std::size_t vertexIndex, const std::string& exceptionMessage) const;
};

} // namespace selbaward
#endif // SELBAWARD_POLYGON_HPP
