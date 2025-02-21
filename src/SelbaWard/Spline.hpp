//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Spline
//
// Copyright(c) 2014-2025 M.J.Silk
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

#ifndef SELBAWARD_SPLINE_HPP
#define SELBAWARD_SPLINE_HPP

#include "Common.hpp"

namespace selbaward
{

// SW Spline v1.7.2
class Spline : public sf::Drawable
{
public:
	enum class ThickCornerType
	{
		Bevel,
		Point,
		PointLimit,
		PointClip,
		Round,
	};
	enum class ThickCapType
	{
		None,
		Extended,
		Round,
	};
	struct Vertex
	{
		sf::Vector2f position;
		sf::Vector2f frontHandle; // offset from position
		sf::Vector2f backHandle; // offset from position
		float thickness;
		sf::Color color;
		float randomNormalOffsetRange;
		Vertex() : thickness{ 1.f }, color{ sf::Color::White }, randomNormalOffsetRange{ 1.f } {}
		Vertex(sf::Vector2f newPosition) : position{ newPosition }, thickness{ 1.f }, color{ sf::Color::White }, randomNormalOffsetRange{ 1.f } {}
	};

	Spline(std::size_t vertexCount = 0u, sf::Vector2f initialPosition = { 0.f, 0.f });
	Spline(std::initializer_list<sf::Vector2f> list); // pass vertices' positions (sf::Vector2f) to the constructor (sets size automatically)
	Spline(const Spline& spline);
	Spline& operator=(const Spline& spline);

	void update();
	void updateOutputVertices();

	Vertex& operator[] (std::size_t index); // direct access to the spline's vertices (sw::Spline::Vertex) using the [] operator. no checks are performed. using with an invalid index results in undefined behaviour

	void connectFrontToFrontOf(const Spline& spline, bool rotateSpline = true, bool moveSpline = true);
	void connectFrontToBackOf(const Spline& spline, bool rotateSpline = true, bool moveSpline = true);
	void connectBackToFrontOf(const Spline& spline, bool rotateSpline = true, bool moveSpline = true);
	void connectBackToBackOf(const Spline& spline, bool rotateSpline = true, bool moveSpline = true);

	void addSplineConnectFrontToFront(Spline spline, bool rotateSpline = true, bool moveSpline = true);
	void addSplineConnectFrontToBack(Spline spline, bool rotateSpline = true, bool moveSpline = true);
	void addSplineConnectBackToFront(Spline spline, bool rotateSpline = true, bool moveSpline = true);
	void addSplineConnectBackToBack(Spline spline, bool rotateSpline = true, bool moveSpline = true);

	void addSplineToFront(const Spline& spline);
	void addSplineToBack(const Spline& spline);

	void setClosed(bool isClosed);
	bool getClosed() const;

	void rotate(float rotation, sf::Vector2f origin);
	void scale(float scale, sf::Vector2f origin, bool scaleThickness = true, bool scaleHandles = true);
	void move(sf::Vector2f offset);

	void setRandomNormalOffsetsActivated(bool randomNormalOffsetsActivated);
	bool getRandomNormalOffsetsActivated() const;

	void setThickCornerType(ThickCornerType cornerType);
	ThickCornerType getThickCornerType() const;
	void setRoundedThickCornerInterpolationLevel(std::size_t roundedCornerInterpolationLevel);
	std::size_t getRoundedThickCornerInterpolationLevel() const;

	void setThickStartCapType(ThickCapType thickStartCapType);
	ThickCapType getThickStartCapType() const;
	void setRoundedThickStartCapInterpolationLevel(std::size_t roundedThickStartCapInterpolationLevel);
	std::size_t getRoundedThickStartCapInterpolationLevel() const;

	void setThickEndCapType(ThickCapType thickEndCapType);
	ThickCapType getThickEndCapType() const;
	void setRoundedThickEndCapInterpolationLevel(std::size_t roundedThickEndCapInterpolationLevel);
	std::size_t getRoundedThickEndCapInterpolationLevel() const;

	void setMaxCornerPointLength(float maxPointCornerLength);
	float getMaxCornerPointLength() const;

	std::size_t getVertexCount() const;
	std::size_t getLastVertexIndex() const;
	float getLength() const; // returns the length of (linear) spline - the sum of the lengths of all of the lines between consecutive vertices
	float getInterpolatedLength() const; // returns the actual length of the spline - the sum of the lengths of all of the line segments drawn

	void reserveVertices(std::size_t numberOfVertices);

	void addVertices(const std::vector<sf::Vector2f>& positions);
	void addVertices(std::size_t index, const std::vector<sf::Vector2f>& positions); // inserts the new vertices before the vertex at the specified index
	void addVertices(std::size_t numberOfVertices, sf::Vector2f position = { 0.f, 0.f });
	void addVertices(std::size_t numberOfVertices, std::size_t index, sf::Vector2f position = { 0.f, 0.f }); // inserts the new vertices before the vertex at the specified index
	void addVertex(sf::Vector2f position = { 0.f, 0.f });
	void addVertex(std::size_t index, sf::Vector2f position = { 0.f, 0.f }); // inserts the new vertex before the vertex at the specified index
	void removeVertex(std::size_t index);
	void removeVertices(std::size_t index, std::size_t numberOfVertices = 0u); // if numberOfVertices is zero (the default), removes all vertices from specified index until the end
	void reverseVertices();

	void setPosition(std::size_t index, sf::Vector2f position = { 0.f, 0.f });
	void setPositions(std::size_t index, std::size_t numberOfVertices = 0u, sf::Vector2f position = { 0.f, 0.f }); // if numberOfVertices is zero (the default), sets positions of all vertices from specified index until the end
	void setPositions(const std::vector<sf::Vector2f>& positions, std::size_t index = 0u);
	sf::Vector2f getPosition(std::size_t index) const;

	void setFrontHandle(std::size_t index, sf::Vector2f offset);
	sf::Vector2f getFrontHandle(std::size_t index) const;

	void setBackHandle(std::size_t index, sf::Vector2f offset);
	sf::Vector2f getBackHandle(std::size_t index) const;

	void resetHandles(std::size_t index = 0u, std::size_t numberOfVertices = 0u); // if numberOfVertices is zero (the default), reset handles for all vertices from specified index until the end. if no index is specified, all handles are reset

	void smoothHandles();

	void setHandlesVisible(bool handlesVisible = true);
	bool getHandlesVisible() const;

	template <class T>
	void setThickness(T thickness);
	float getThickness() const;
	template <class T>
	void setThickness(std::size_t index, T thickness);
	float getThickness(std::size_t index) const;

	template<class T>
	void setRandomNormalOffsetRange(T randomNormalOffsetRange);
	float getRandomNormalOffsetRange() const;
	template<class T>
	void setRandomNormalOffsetRange(std::size_t index, T randomNormalOffsetRange);
	float getRandomNormalOffsetRange(std::size_t index) const;

	void setColor(sf::Color color);
	sf::Color getColor() const;
	void setColor(std::size_t index, sf::Color color);
	sf::Color getColor(std::size_t index) const;

	void setInterpolationSteps(std::size_t interpolationSteps);
	std::size_t getInterpolationSteps() const;

	void setHandleMirrorLock(bool handleMirrorLock = true);
	void setHandleAngleLock(bool handleAngleLock = true);

	void setBezierInterpolation(bool bezierInterpolation = true);
	bool getBezierInterpolation() const;

	void setPrimitiveType(sf::PrimitiveType primitiveType);
	sf::PrimitiveType getPrimitiveType() const;

	sf::Vector2f getInterpolatedPosition(std::size_t interpolationOffset, std::size_t index = 0u) const; // index is control vertex offset
	sf::Vector2f getInterpolatedPositionTangent(std::size_t interpolationOffset, std::size_t index = 0u) const; // index is control vertex offset
	sf::Vector2f getInterpolatedPositionNormal(std::size_t interpolationOffset, std::size_t index = 0u) const; // index is control vertex offset
	float getInterpolatedPositionThickness(std::size_t interpolationOffset, std::size_t index = 0u) const; // index is control vertex offset
	float getInterpolatedPositionThicknessCorrectionScale(std::size_t interpolationOffset, std::size_t index = 0u) const; // index is control vertex offset
	std::size_t getInterpolatedPositionCount() const;

	std::vector<sf::Vector2f> exportAllPositions() const;
	std::vector<sf::Vector2f> exportAllInterpolatedPositions() const;







private:
	bool m_throwExceptions;
	bool m_isClosed;
	bool m_isRandomNormalOffsetsActivated;
	ThickCornerType m_thickCornerType;
	ThickCapType m_thickStartCapType;
	ThickCapType m_thickEndCapType;
	std::size_t m_roundedThickCornerInterpolationLevel; // number of interpolations per corner (includes corners at interpolated points). does not include backward and forward line points - only the interpolation between them i.e. 0 interpolation looks equal to bevel.
	std::size_t m_roundedThickStartCapInterpolationLevel; // number of interpolations. 0 is flat (same as no cap), 1 is triangle, 2+ circular.
	std::size_t m_roundedThickEndCapInterpolationLevel; // number of interpolations. 0 is flat (same as no cap), 1 is triangle, 2+ circular.
	float m_maxPointLength; // maximum length of point when limited or clipped. ignored when using other corner types including Point.
	const bool m_automaticallyUpdateRandomNormalOffset;

	std::vector<Vertex> m_vertices;
	sf::Color m_color;
	float m_thickness;
	float m_randomNormalOffsetRange;

	std::vector<sf::Vertex> m_interpolatedVertices;
	std::vector<sf::Vector2f> m_interpolatedVerticesUnitTangents;
	std::vector<sf::Vertex> m_outputVertices;
	sf::PrimitiveType m_primitiveType;
	std::size_t m_interpolationSteps;
	bool m_useBezier;

	std::vector<sf::Vertex> m_handlesVertices;
	bool m_showHandles;
	bool m_lockHandleMirror;
	bool m_lockHandleAngle;

	void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
	bool priv_isValidVertexIndex(std::size_t vertexIndex) const;
	bool priv_testVertexIndex(std::size_t vertexIndex, const std::string& exceptionMessage) const;
	bool priv_isThick() const;
	std::size_t priv_getNumberOfPointsPerVertex() const;
	std::size_t priv_getInterpolatedIndex(const std::size_t interpolationOffset, const std::size_t index) const;
	void priv_updateOutputVertices();
};

template <class T>
inline void Spline::setThickness(const T thickness)
{
	m_thickness = static_cast<float>(thickness);
}

inline float Spline::getThickness() const
{
	return m_thickness;
}

template <class T>
inline void Spline::setThickness(const std::size_t index, const T thickness)
{
	m_vertices[index].thickness = static_cast<float>(thickness);
}

inline float Spline::getThickness(const std::size_t index) const
{
	return m_vertices[index].thickness;
}

template<class T>
inline void Spline::setRandomNormalOffsetRange(const T randomNormalOffsetRange)
{
	m_randomNormalOffsetRange = static_cast<float>(randomNormalOffsetRange);
}

inline float Spline::getRandomNormalOffsetRange() const
{
	return m_randomNormalOffsetRange;
}

template <class T>
inline void Spline::setRandomNormalOffsetRange(const std::size_t index, const T randomNormalOffsetRange)
{
	m_vertices[index].randomNormalOffsetRange = static_cast<float>(randomNormalOffsetRange);
}

inline float Spline::getRandomNormalOffsetRange(const std::size_t index) const
{
	return m_vertices[index].randomNormalOffsetRange;
}

inline Spline::Vertex& Spline::operator[] (const std::size_t index)
{
	return m_vertices[index];
}

inline bool Spline::getClosed() const
{
	return m_isClosed;
}

inline bool Spline::getRandomNormalOffsetsActivated() const
{
	return m_isRandomNormalOffsetsActivated;
}

inline Spline::ThickCornerType Spline::getThickCornerType() const
{
	return m_thickCornerType;
}

inline std::size_t Spline::getRoundedThickCornerInterpolationLevel() const
{
	return m_roundedThickCornerInterpolationLevel;
}

inline Spline::ThickCapType Spline::getThickStartCapType() const
{
	return m_thickStartCapType;
}

inline std::size_t Spline::getRoundedThickStartCapInterpolationLevel() const
{
	return m_roundedThickStartCapInterpolationLevel;
}

inline Spline::ThickCapType Spline::getThickEndCapType() const
{
	return m_thickEndCapType;
}

inline std::size_t Spline::getRoundedThickEndCapInterpolationLevel() const
{
	return m_roundedThickEndCapInterpolationLevel;
}

inline float Spline::getMaxCornerPointLength() const
{
	return m_maxPointLength;
}

inline std::size_t Spline::getVertexCount() const
{
	return m_vertices.size();
}

inline std::size_t Spline::getLastVertexIndex() const
{
	return m_vertices.size() - 1u;
}

inline bool Spline::getHandlesVisible() const
{
	return m_showHandles;
}

inline sf::Color Spline::getColor() const
{
	return m_color;
}

inline sf::Color Spline::getColor(const std::size_t index) const
{
	return m_vertices[index].color;
}

inline std::size_t Spline::getInterpolationSteps() const
{
	return m_interpolationSteps;
}

inline sf::PrimitiveType Spline::getPrimitiveType() const
{
	return m_primitiveType;
}

inline bool Spline::getBezierInterpolation() const
{
	return m_useBezier;
}

} // namespace selbaward
#endif // SELBAWARD_SPLINE_HPP
