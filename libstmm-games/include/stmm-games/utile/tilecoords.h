/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   tilecoords.h
 */

#ifndef STMG_TILE_COORDS_H
#define STMG_TILE_COORDS_H

#include "tile.h"

#include "util/basictypes.h"
#include "util/coords.h"

#include <vector>
#include <memory>
#include <utility>

#include <stdint.h>

namespace stmg
{

class TileCoords : public Coords
{
public:
	/** Constructor.
	 */
	TileCoords() noexcept;
	/** Constructor.
	 * @param nAtLeastSize The indicative minimum number of unique values.
	 */
	explicit TileCoords(int32_t nAtLeastSize) noexcept;

	using Coords::reInit;
	/** Clears the instance.
	 * All iterators become invalid.
	 * @param nAtLeastSize The indicative minimum number of unique values.
	 */
	void reInit(int32_t nAtLeastSize) noexcept;

	using Coords::size;
	using Coords::isEmpty;

	/** Add a tile coord.
	 * If the tile already exists at the position overwrites it.
	 *
	 * Might invalidate iterators.
	 * @param nX The x.
	 * @param nY The y.
	 * @param oTile The tile.
	 */
	void add(int32_t nX, int32_t nY, const Tile& oTile) noexcept;
	/** Add a tile coord.
	 * If the tile already exists at the position overwrites it.
	 *
	 * Might invalidate iterators.
	 * @param oXY The coord to add.
	 * @param oTile The tile.
	 */
	void add(NPoint oXY, const Tile& oTile) noexcept;
	/** Set tile for all coords in a rectangle.
	 * If tiles already exist at some position overwrites them.
	 *
	 * Might invalidate iterators.
	 * @param nX The x.
	 * @param nY The y.
	 * @param nW The width. Must be &gt; 0.
	 * @param nH The height. Must be &gt; 0.
	 * @param oTile The tile.
	 */
	void addRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH, const Tile& oTile) noexcept;
	/** Set tile for all coords in a rectangle.
	 * If tiles already exist at some position overwrites them.
	 *
	 * Might invalidate iterators.
	 * @param oRect The rectangle.
	 * @param oTile The tile.
	 */
	void addRect(NRect oRect, const Tile& oTile) noexcept;
	/** Whether the instance contains a tile at a position.
	 * @param nX The x.
	 * @param nY The y.
	 * @return Whether the instance contains the tile and the tile itself.
	 */
	std::pair<bool, Tile> getTile(int32_t nX, int32_t nY) const noexcept;
	/** Whether the instance contains a tile at a position.
	 * @param oXY The coord.
	 * @return Whether the instance contains the tile aand the tile itself.
	 */
	std::pair<bool, Tile> getTile(NPoint oXY) const noexcept;
//	/** Removes a coord if it exists.
//	 * @param nX The x.
//	 * @param nY The y.
//	 * @return Whether the coord existed.
//	 */
//	bool remove(int32_t nX, int32_t nY) noexcept;
//	/** Removes a coord if it exists.
//	 * @param oXY The coord.
//	 * @return Whether the coord existed.
//	 */
//	bool remove(NPoint oXY) noexcept;
	using Coords::remove;
	using Coords::removeInRect;
	using Coords::getMinMax;
	/** Add another coords to this instance.
	 * @param oTileCoords Thee coords to add.
	 */
	void add(const TileCoords& oTileCoords) noexcept;
	/** Remove all the coords of another instance,
	 * @param oCoords The coords to remove.
	 */
	void remove(const Coords& oCoords) noexcept;
private:
	struct PosTile
	{
		NPoint m_oXY;
		Tile m_oTile;
	};
	friend class const_iterator;
public:
	/** TileCoords iterator.
	 */
	class const_iterator : Coords::const_iterator
	{
	public:
		using Coords::const_iterator::x;
		using Coords::const_iterator::y;
		using Coords::const_iterator::point;
		using Coords::const_iterator::next;
		/** Tells whether the iterator point to the same position as another.
		 * @param it The other iterator.
		 * @return Whether same position within Coords.
		 */
		inline bool operator==(const const_iterator& it) const noexcept { return Coords::const_iterator::operator==(it); };
		/** Opposite of const_iterator::operator==().
		 */
		inline bool operator!=(const const_iterator& it) const noexcept { return Coords::const_iterator::operator!=(it); };
		/** Get the current tile.
		 * @return The tile.
		 */
		const Tile& getTile() const noexcept
		{
			// since when removing a coord the index of another might change
			// indexes are not cached on the iterator
			const int32_t nIdx = get();
			if (nIdx < 0) {
				return Tile::s_oEmptyTile;
			} else {
				return m_aPosTiles[nIdx].m_oTile;
			}
		}
	private:
		friend class TileCoords;
		const_iterator(const TileCoords& oTileCoords, Coords::const_iterator it) noexcept;
	private:
		const std::vector<PosTile>& m_aPosTiles;
	};

	inline TileCoords::const_iterator begin() noexcept
	{
		return TileCoords::const_iterator(*this, Coords::begin());
	}
	inline TileCoords::const_iterator begin() const noexcept
	{
		return TileCoords::const_iterator(*this, Coords::begin());
	}
	inline TileCoords::const_iterator end() noexcept
	{
		return TileCoords::const_iterator(*this, Coords::end());
	}
	inline TileCoords::const_iterator end() const noexcept
	{
		return TileCoords::const_iterator(*this, Coords::end());
	}
private:
//	std::unordered_map<int64_t, Tile>::iterator remove(const std::unordered_map<int64_t, Tile>::iterator itFind) noexcept;
	friend class Coords;
	// returns an additional coord that has changed index as a consequence
	// of the removal (if index is -1 none has changed)
	std::pair<NPoint, int32_t> removeIndex(int32_t nIdx) noexcept;

private:
	// This vector only contains non empty tiles
	std::vector<PosTile> m_aPosTiles; // Size: <= Coords::size()
};

} // namespace stmg

#endif	/* STMG_TILE_COORDS_H */

