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
 * File:   tilerect.h
 */

#ifndef STMG_TILE_RECT_H
#define STMG_TILE_RECT_H

#include "tile.h"

#include <stdint.h>

namespace stmg { struct NPoint; }

namespace stmg
{

class TileRect
{
public:
	virtual ~TileRect() noexcept = default;
	/** The width in tiles.
	 * @return The width. Must be &gt;= 0.
	 */
	virtual int32_t getW() const noexcept = 0;
	/** The height in tiles.
	 * @return The height. Must be &gt;= 0.
	 */
	virtual int32_t getH() const noexcept = 0;
	/** The tile at a certain position.
	 * @param oXY The position. Must be &gt;= 0 and oXY.m_nX &lt; getW() and oXY.m_nY &lt; getH().
	 * @return The tile.
	 */
	virtual const Tile& get(NPoint oXY) const noexcept = 0;
	/** Whether an instance has all empty tiles.
	 * @param oTiles The tile rect.
	 * @return Whether all empty.
	 */
	static bool isAllEmptyTiles(const TileRect& oTiles) noexcept;
};

} // namespace stmg

#endif	/* STMG_TILE_RECT_H */

