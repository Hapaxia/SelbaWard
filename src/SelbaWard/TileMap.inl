//////////////////////////////////////////////////////////////////////////////
//
// Selba Ward (https://github.com/Hapaxia/SelbaWard)
// --
//
// Tile Map
//
// Copyright(c) 2016-2020 M.J.Silk
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

#ifndef SELBAWARD_TILEMAP_INL
#define SELBAWARD_TILEMAP_INL

namespace selbaward
{

template <class T>
TileMap<T>::TileMap()
	: m_do()
	, m_is()
	, m_levelContainerType(LevelContainerType::None)
	, m_levelWidth(0u)
	, m_levelSize(0u)
	, m_pLevel(nullptr)
	, m_gridSize({ 1u, 1u })
	, m_grid(1u)
	, m_outOfBoundsTile(0u)
	, m_camera({ 0.f, 0.f })
	, m_cameraTarget({ 0.f, 0.f })
	, m_color(sf::Color::White)
	, m_primitiveType(sf::PrimitiveType::Quads)
	, m_size()
	, m_texture(nullptr)
	, m_numberOfTextureTilesPerRow(16u)
	, m_textureOffset({ 0u, 0u })
	, m_textureTileSize({ 16u, 16u })
	, m_vertices()
	, m_redrawRequired(true)
	, m_renderTexture()
	, m_render(4)
{
	priv_recreateRenderTexture();
}

template <class T>
void TileMap<T>::update()
{
	if (m_levelWidth > m_levelSize)
		m_levelWidth = m_levelSize;

	// if width is zero, rather than not display anything, choose width automatically to create an approximate square shape from the level data (if level size is zero, width will still be zero)
	if (m_levelWidth == 0)
		m_levelWidth = static_cast<unsigned int>(std::sqrt(m_levelSize));

	const sf::Vector2f actualCamera{ priv_getActualCamera() };
	const sf::Vector2i levelOffset{ static_cast<int>(std::floor(actualCamera.x)), static_cast<int>(std::floor(actualCamera.y)) };
	const unsigned int height{ m_levelWidth > 0u ? m_levelSize / m_levelWidth : 0u };

	for (unsigned int y{ 0 }; y < m_gridSize.y; ++y)
	{
		for (unsigned int x{ 0 }; x < m_gridSize.x; ++x)
		{
			const unsigned int gridIndex{ y * m_gridSize.x + x };
			const unsigned int levelIndex{ (levelOffset.y + y) * m_levelWidth + levelOffset.x + x };
			if (levelOffset.x + x >= 0 && levelOffset.x + x < m_levelWidth &&
				levelOffset.y + y >= 0 && levelOffset.y + y < height)
			{
				switch (m_levelContainerType)
				{
				case LevelContainerType::Vector:
					m_grid[gridIndex] = (*static_cast<const std::vector<T>*>(m_pLevel))[levelIndex];
					break;
				case LevelContainerType::Deque:
					m_grid[gridIndex] = (*static_cast<const std::deque<T>*>(m_pLevel))[levelIndex];
					break;
				case LevelContainerType::Raw:
					m_grid[gridIndex] = static_cast<const T*>(m_pLevel)[levelIndex];
					break;
				case LevelContainerType::None:
				default:
					m_grid[gridIndex] = m_outOfBoundsTile;;
				}
			}
			else
				m_grid[gridIndex] = m_outOfBoundsTile;
		}
	}

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setLevel()
{
	m_pLevel = nullptr;
	m_levelSize = 0u;
	m_levelWidth = 0u;
	m_levelContainerType = LevelContainerType::None;
}

template <class T>
void TileMap<T>::setLevel(const std::vector<T>& level)
{
	m_pLevel = &level;
	m_levelSize = level.size();
	m_levelContainerType = LevelContainerType::Vector;
}

template <class T>
void TileMap<T>::setLevel(const std::deque<T>& level)
{
	m_pLevel = &level;
	m_levelSize = level.size();
	m_levelContainerType = LevelContainerType::Deque;
}

template <class T>
void TileMap<T>::setLevel(const T* level, const unsigned long int size)
{
	m_pLevel = level;
	m_levelSize = size;
	m_levelContainerType = LevelContainerType::Raw;
}

template <class T>
template <class U>
void TileMap<T>::setLevel(const U& level, const unsigned long int width)
{
	m_levelWidth = width;
	setLevel(level);
}

template <class T>
void TileMap<T>::setLevel(const T* level, const unsigned long int size, const unsigned long int width)
{
	m_levelWidth = width;
	setLevel(level, size);
}

template <class T>
void TileMap<T>::setLevelWidth(const unsigned long int width)
{
	m_levelWidth = width;
}

template <class T>
unsigned int TileMap<T>::getLevelWidth() const
{
	return m_levelWidth;
}

template <class T>
void TileMap<T>::setSize(const sf::Vector2f size)
{
	m_size = size;

	m_redrawRequired = true;
}

template <class T>
sf::Vector2f TileMap<T>::getSize() const
{
	return m_size;
}

template <class T>
void TileMap<T>::setGridSize(const sf::Vector2u gridSize)
{
	m_gridSize = { gridSize.x + 1, gridSize.y + 1 };
	m_grid.resize(m_gridSize.x * m_gridSize.y);

	priv_recreateRenderTexture();
}

template <class T>
sf::Vector2u TileMap<T>::getGridSize() const
{
	return{ m_gridSize.x - 1, m_gridSize.y - 1 };
}

template <class T>
unsigned int TileMap<T>::getTotalGridSize() const
{
	return priv_getTotalSizeFromSizeVector(getGridSize());
}

template <class T>
void TileMap<T>::setOutOfBoundsTile(const unsigned long int textureTileIndex)
{
	m_outOfBoundsTile = textureTileIndex;

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setTexture(const sf::Texture& texture)
{
	m_texture = &texture;

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setTexture()
{
	m_texture = nullptr;

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setNumberOfTextureTilesPerRow(const unsigned int numberOfTextureTilesPerRow)
{
	m_numberOfTextureTilesPerRow = numberOfTextureTilesPerRow;

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setTextureOffset(const sf::Vector2u textureOffset)
{
	m_textureOffset = textureOffset;

	m_redrawRequired = true;
}

template <class T>
void TileMap<T>::setTextureTileSize(const sf::Vector2u textureTileSize)
{
	m_textureTileSize = textureTileSize;

	priv_recreateRenderTexture();
}

template <class T>
sf::Vector2u TileMap<T>::getTextureTileSize() const
{
	return m_textureTileSize;
}

template <class T>
void TileMap<T>::setSmooth(const bool smooth)
{
	m_is.smooth = smooth;

	priv_updateRender();
}

template <class T>
bool TileMap<T>::getSmooth() const
{
	return m_is.smooth;
}

template <class T>
void TileMap<T>::setSmoothScroll(const bool smoothScroll)
{
	m_do.scrollSmoothly = smoothScroll;

	priv_updateRender();
}

template <class T>
bool TileMap<T>::getSmoothScroll() const
{
	return m_do.scrollSmoothly;
}

template <class T>
void TileMap<T>::setCameraTargetTile(const sf::Vector2f cameraTileTarget)
{
	m_cameraTarget = cameraTileTarget;;

	m_redrawRequired = true;
}

template <class T>
sf::Vector2f TileMap<T>::getCameraTargetTile() const
{
	return m_cameraTarget;
}

template <class T>
void TileMap<T>::setCamera(const sf::Vector2f camera)
{
	m_camera = priv_getTileOffsetFromVector(camera);

	m_redrawRequired = true;
}

template <class T>
sf::Vector2f TileMap<T>::getCamera() const
{
	return priv_getVectorFromTileOffset(m_camera);
}

template <class T>
void TileMap<T>::setColor(const sf::Color color)
{
	m_color = color;

	m_redrawRequired = true;
}

template <class T>
sf::Color TileMap<T>::getColor() const
{
	return m_color;
}

template <class T>
sf::Vector2i TileMap<T>::getLevelPositionAtCoord(sf::Vector2f coord) const
{
	coord = getInverseTransform().transformPoint(coord);
	const sf::Vector2f actualCamera{ priv_getActualCamera() };
	return{ static_cast<int>(std::floor(coord.x * (m_gridSize.x - 1) / m_size.x + actualCamera.x)), static_cast<int>(std::floor(coord.y * (m_gridSize.y - 1) / m_size.y + actualCamera.y)) };
}

template <class T>
unsigned long int TileMap<T>::getTileAtCoord(const sf::Vector2f coord) const
{
	return priv_getTileAtGridPosition(priv_getGridPositionAtCoord(coord));
}

template <class T>
sf::Vector2f TileMap<T>::getCoordAtLevelGridPosition(sf::Vector2f levelGridPosition) const
{
	const sf::Vector2f actualCamera{ priv_getActualCamera() };
	const sf::Vector2f local{ (levelGridPosition.x - actualCamera.x) * m_size.x / (m_gridSize.x - 1), (levelGridPosition.y - actualCamera.y) * m_size.y / (m_gridSize.y - 1) };
	return getTransform().transformPoint(local);
}

template <class T>
sf::Vector2f TileMap<T>::getTileSize() const
{
	return{ m_size.x / (m_gridSize.x - 1), m_size.y / (m_gridSize.y - 1) };
}



template <class T>
void TileMap<T>::redraw()
{
	priv_updateVertices();
	priv_updateRender();
}



// PRIVATE

template <class T>
void TileMap<T>::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	if (m_redrawRequired)
	{
		priv_updateVertices();
		priv_updateRender();
		m_redrawRequired = false;
	}

	states.texture = &m_renderTexture.getTexture();
	states.transform = getTransform();

	target.draw(&m_render.front(), 4, sf::PrimitiveType::Quads, states); // final render is always 4 vertices & quad
}

template <class T>
void TileMap<T>::priv_updateVertices() const
{
	m_vertices.resize(m_gridSize.x * m_gridSize.y * 4);
	if (m_gridSize.x == 0 || m_gridSize.y == 0)
		return;

	for (unsigned int y{ 0 }; y < m_gridSize.y; ++y)
	{
		for (unsigned int x{ 0 }; x < m_gridSize.x; ++x)
		{
			const unsigned int tileIndex{ y * m_gridSize.x + x };
			const unsigned long int tileValue{ m_grid[tileIndex] };
			const sf::Vector2u textureTilePosition{ static_cast<unsigned int>(tileValue % m_numberOfTextureTilesPerRow * m_textureTileSize.x), static_cast<unsigned int>(tileValue / m_numberOfTextureTilesPerRow * m_textureTileSize.y) };
			sf::Vertex* pVertex{ &m_vertices[tileIndex * 4] };

			// top-left
			pVertex->position = { static_cast<float>(m_textureTileSize.x * x), static_cast<float>(m_textureTileSize.y * y) };
			pVertex->texCoords = sf::Vector2f(m_textureOffset + textureTilePosition);
			pVertex++->color = m_color;

			// top-right
			pVertex->position = { static_cast<float>(m_textureTileSize.x * (x + 1)), static_cast<float>(m_textureTileSize.y * y) };
			pVertex->texCoords = { static_cast<float>(m_textureOffset.x + textureTilePosition.x + m_textureTileSize.x), static_cast<float>(m_textureOffset.y + textureTilePosition.y) };
			pVertex++->color = m_color;

			// bottom-right
			pVertex->position = { static_cast<float>(m_textureTileSize.x * (x + 1)), static_cast<float>(m_textureTileSize.y * (y + 1)) };
			pVertex->texCoords = sf::Vector2f(m_textureOffset + textureTilePosition + m_textureTileSize);
			pVertex++->color = m_color;

			// bottom-left
			pVertex->position = { static_cast<float>(m_textureTileSize.x * x), static_cast<float>(m_textureTileSize.y * (y + 1)) };
			pVertex->texCoords = { static_cast<float>(m_textureOffset.x + textureTilePosition.x), static_cast<float>(m_textureOffset.y + textureTilePosition.y + m_textureTileSize.y) };
			pVertex++->color = m_color;
		}
	}

	const sf::Vector2f actualCamera{ priv_getActualCamera() };
	const sf::Vector2f cameraOffset{ std::trunc((actualCamera.x - std::floor(actualCamera.x)) * m_textureTileSize.x), std::trunc((actualCamera.y - std::floor(actualCamera.y)) * m_textureTileSize.y) };
	for (auto& vertex : m_vertices)
		vertex.position -= cameraOffset;
}

template <class T>
void TileMap<T>::priv_updateRender() const
{
	m_render[0].position = { 0.f, 0.f };
	m_render[1].position = { m_size.x, 0.f };
	m_render[2].position = m_size;
	m_render[3].position = { 0.f, m_size.y };
	const sf::Vector2f size{ m_renderTexture.getSize() };
	m_render[0].texCoords = { 0.f, 0.f };
	m_render[1].texCoords = { size.x, 0.f };
	m_render[2].texCoords = size;
	m_render[3].texCoords = { 0.f, size.y };

	if (m_do.scrollSmoothly)
	{
		const sf::Vector2f tileSize{ m_size.x / m_gridSize.x, m_size.y / m_gridSize.y };
		const sf::Vector2f pixelSize{ tileSize.x / m_textureTileSize.x, tileSize.y / m_textureTileSize.y };
		const sf::Vector2f actualCamera{ priv_getActualCamera() };
		const sf::Vector2f cameraOffset{ ((actualCamera.x - std::floor(actualCamera.x)) * m_textureTileSize.x), ((actualCamera.y - std::floor(actualCamera.y)) * m_textureTileSize.y) };
		const sf::Vector2f fractionOffset{ std::round((cameraOffset.x - std::floor(cameraOffset.x)) * pixelSize.x), std::round((cameraOffset.y - std::floor(cameraOffset.y)) * pixelSize.y) };
		for (auto& corner : m_render)
			corner.position -= fractionOffset;
	}

	m_renderTexture.clear(sf::Color::Transparent);
	const unsigned int numberOfVertices{ static_cast<unsigned int>(m_vertices.size()) };
	if (numberOfVertices > 0)
		m_renderTexture.draw(&m_vertices.front(), numberOfVertices, m_primitiveType, m_texture);
	m_renderTexture.display();

	m_renderTexture.setSmooth(m_is.smooth);
}

template <class T>
void TileMap<T>::priv_recreateRenderTexture()
{
	if (m_gridSize.x < 2 || m_gridSize.y < 2)
		m_renderTexture.create(1, 1);
	else
		m_renderTexture.create((m_gridSize.x - 1) * m_textureTileSize.x, (m_gridSize.y - 1) * m_textureTileSize.y);

	m_redrawRequired = true;
}

template <class T>
sf::Vector2i TileMap<T>::priv_getGridPositionAtCoord(sf::Vector2f coord) const
{
	coord = getInverseTransform().transformPoint(coord);
	const sf::Vector2f actualCamera{ priv_getActualCamera() };
	const sf::Vector2f fraction{ actualCamera.x - std::floor(actualCamera.x), actualCamera.y - std::floor(actualCamera.y) };
	return{ static_cast<int>(std::floor(coord.x * (m_gridSize.x - 1) / m_size.x + fraction.x)), static_cast<int>(std::floor(coord.y * (m_gridSize.y - 1) / m_size.y + fraction.y)) };
}

template <class T>
unsigned int TileMap<T>::priv_getTileAtGridPosition(const sf::Vector2i gridPosition) const
{
	if (gridPosition.x < 0 ||
		gridPosition.y < 0 ||
		static_cast<unsigned int>(gridPosition.x) >= m_gridSize.x ||
		static_cast<unsigned int>(gridPosition.y) >= m_gridSize.y)
		return 0u;

	return m_grid[gridPosition.y * m_gridSize.x + gridPosition.x];
}

template <class T>
inline sf::Vector2f TileMap<T>::priv_getActualCamera() const
{
	return m_camera - m_cameraTarget;
}

template <class T>
sf::Vector2f TileMap<T>::priv_getTileOffsetFromVector(const sf::Vector2f vector) const
{
	return
	{
		vector.x * (m_gridSize.x - 1) / m_size.x,
		vector.y * (m_gridSize.y - 1) / m_size.y,
	};
}

template <class T>
sf::Vector2f TileMap<T>::priv_getVectorFromTileOffset(const sf::Vector2f offset) const
{
	return
	{
		offset.x * m_size.x / (m_gridSize.x - 1),
		offset.y * m_size.y / (m_gridSize.y - 1)
	};
}

template <class T>
template <class U>
U TileMap<T>::priv_getTotalSizeFromSizeVector(sf::Vector2<U> vector) const
{
	return vector.x * vector.y;
}

} // namespace selbaward
#endif // SELBAWARD_TILEMAP_INL
