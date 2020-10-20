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
 * File:   extendedboard.h
 */

#ifndef STMG_EXTENDED_BOARD_H
#define STMG_EXTENDED_BOARD_H

#include <stdint.h>

namespace stmg { class Tile; }
namespace stmg { class TileAnimator; }

namespace stmg
{

class ExtendedBoard
{
public:
	virtual ~ExtendedBoard() noexcept = default;

	/** Extended board tile getter.
	 * @param nX The x. Can e outside the level's board.
	 * @param nY The y. Can e outside the level's board.
	 * @return The tile.
	 */
	virtual const Tile& boardGetTile(int32_t nX, int32_t nY) const noexcept = 0;
	/** Extended board tile animator getter.
	 * @param nX The x. Can e outside the level's board.
	 * @param nY The y. Can e outside the level's board.
	 * @param nIdxTileAni The tile animation. Must be &gt;= 0 and &lt; Level::getNrTileAniAttrs().
	 * @return The tile animator.
	 */
	virtual const TileAnimator* boardGetTileAnimator(int32_t nX, int32_t nY, int32_t nIdxTileAni) const noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_EXTENDED_BOARD_H */

