/*
 * File:   tileanimator.h
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#ifndef STMG_TILE_ANIMATOR_H
#define STMG_TILE_ANIMATOR_H

#include <stdint.h>

namespace stmg
{

class LevelBlock;

/** Interface used to animate tiles of board and level blocks.
 */
class TileAnimator
{
public:
	virtual ~TileAnimator() noexcept = default;
	static constexpr double s_fInactiveElapsed = -1.0; /**< Unary elapsed meaning inactive tile animation. */
protected:
	friend class Level;
	friend class LevelBlock;
	/** Get unary elapsed of a board tile animation at a view tick.
	 * @param nHash The value passed to Level::boardSetTileAnimator().
	 * @param nX The x cell on the board.
	 * @param nY The y cell on the board.
	 * @param nAni Index into Named::tileAnis().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotTicks.
	 * @param nTotTicks The total number of view ticks.
	 * @return The elapsed (0.0 to 1.0) or s_fInactiveElapsed if inactive.
	 */
	virtual double getElapsed01(int32_t nHash, int32_t nX, int32_t nY, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept = 0;
	/** Get unary elapsed of a block brick tile animation at a view tick.
	 * @param nHash The value passed to LevelBlock::blockSetTileAnimator().
	 * @param oLevelBlock The level block.
	 * @param nBrickIdx The brick number. Must be positive.
	 * @param nAni Index into Named::tileAnis().
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotTicks.
	 * @param nTotTicks The total number of view ticks.
	 * @return The elapsed (0.0 to 1.0) or s_fInactiveElapsed if inactive.
	 */
	virtual double getElapsed01(int32_t nHash, const LevelBlock& oLevelBlock, int32_t nBrickIdx, int32_t nAni, int32_t nViewTick, int32_t nTotTicks) const noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_TILE_ANIMATOR_H */

