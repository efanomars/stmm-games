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
 * File:   levelshow.h
 */

#ifndef STMG_LEVEL_SHOW_H
#define STMG_LEVEL_SHOW_H

#include "util/basictypes.h"

#include <memory>

#include <stdint.h>

namespace stmg
{

using std::unique_ptr;
using std::shared_ptr;

class Level;


/** This class is used for both Show and Subshows.
 * In the case of Show the position is relative to the level's Board origin.
 * In the case of a player Subshow the position is relative to the level's Show origin.
 */
class LevelShow
{
public:
	/** Interface to position the LevelShow during view ticks.
	 */
	class Positioner
	{
	public:
		virtual ~Positioner() noexcept = default;
		/** The position in tiles within the context of the LevelShow.
		 * The implementation can expect the view ticks to happen in order, but
		 * except for the first (nViewTick == 0) and the last (nTotViewTicks - 1),
		 * view ticks might be skipped.
		 *
		 * As all functions called within a view tick, it should do as little as possible.
		 * Implementations examples: move "smoothly" from a starting position
		 * to a destination or vibrating around the current position.
		 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
		 * @param nTotViewTicks The number of view ticks in the game interval. Must be &gt;= 1.
		 * @return The position in tiles.
		 */
		virtual FPoint getPos(int32_t nViewTick, int32_t nTotViewTicks) noexcept = 0;
	};
	/** The width of the Show or Subshow.
	 * @return The width in tiles.
	 */
	inline int32_t getW() const noexcept { return m_nShowW; }
	/** The height of the Show or Subshow.
	 * @return The height in tiles.
	 */
	inline int32_t getH() const noexcept { return m_nShowH; }
	/** Gets the position of the show or subshow during a view tick.
	 * This calls the positioner if one is installed.
	 * @param nViewTick The view tick. Must be &gt;= 0 and &lt; nTotViewTicks.
	 * @param nTotViewTicks The number of view ticks in the game interval. Must be &gt;= 1.
	 * @return The current position in tiles.
	 */
	FPoint getPos(int32_t nViewTick, int32_t nTotViewTicks) const noexcept;
	/** Gets the position of the show or subshow during the game tick.
	 * This gets the position of the last call to getPos(nViewTick, nTotViewTicks)
	 * or setPos().
	 * @return The current position in tiles.
	 */
	FPoint getPos() const noexcept;
	/** The maximum x coordinate of the level show.
	 * The minimum value is 0.
	 *
	 * The context can be the board (for the main LevelShow) or the
	 * main LevelShow (for the subshows).
	 * @return The maximum x position in tiles within its context.
	 */
	inline double getMaxX() const noexcept { return m_fShowMaxX; }
	/** The maximum y coordinate of the level show.
	 * The minimum value is 0.
	 *
	 * The context can be the board (for the main LevelShow) or the
	 * main LevelShow (for the subshows).
	 * @return The maximum y position in tiles within its context.
	 */
	inline double getMaxY() const noexcept { return m_fShowMaxY; }
	/** Tells whether a board position is visible.
	 * @param fX The x board coordinate in tiles.
	 * @param fY The y board coordinate in tiles.
	 * @return Whether board position visible in this instance.
	 */
	bool isBoardPosVisible(double fX, double fY) const noexcept;
	/** The board position given the position relative to this instance.
	 * @param fX The x coordinate in tiles.
	 * @param fY The y coordinate in tiles.
	 * @return The position within the board.
	 */
	FPoint getBoardPos(double fX, double fY) const noexcept;
	/** Sets the level show position within its context.
	 * This must be called within a game tick.
	 * @param oNewPos The new position.
	 */
	void setPos(const FPoint& oNewPos) noexcept;
	/** Install a positioner.
	 * The caller should ensure that the positioner isn't deleted while
	 * this LevelShow instance is using it.
	 * @param p0Positioner The positioner. Cannot be null.
	 */
	void setPositioner(Positioner* p0Positioner) noexcept;
	/** Reset the positioner.
	 * If no positioner is currently installed has no effect.
	 */
	void resetPositioner() noexcept;
private:
	friend class Level;
	LevelShow() noexcept
	: m_nShowW(-1)
	, m_nShowH(-1)
	, m_p0Positioner(nullptr)
	, m_fShowMaxX(0.0)
	, m_fShowMaxY(0.0)
	, m_nGameTickLastChanged(-1)
	, m_nViewTickLastChanged(-1)
	, m_p0Level(nullptr)
	{
	}
	bool m_bIsSubshow;
	int32_t m_nShowW, m_nShowH; // Set by Level
	Positioner* m_p0Positioner;
	FPoint m_oXY;
	double m_fShowMaxX, m_fShowMaxY; // Set by Level
	int32_t m_nGameTickLastChanged;
	int32_t m_nViewTickLastChanged;
	Level* m_p0Level;
};

} // namespace stmg

#endif	/* STMG_LEVEL_SHOW_H */

