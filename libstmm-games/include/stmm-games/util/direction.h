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
 * File:   direction.h
 */

#ifndef STMG_DIRECTION_H
#define STMG_DIRECTION_H

#include <cassert>

#include <stdint.h>


namespace stmg
{

class Direction
{
public:
	/** The possible direction values. */
	enum VALUE {
		UP = 1, /**< Up (dX=0 dY=-1). */
		DOWN = 2, /**< Down (dX=0 dY=+1). */
		LEFT = 0, /**< Left (dX=-1 dY=0). */
		RIGHT = 3 /**< Right (dX=+1 dY=0). */
	};
	/** The direction from the unary delta of the coords.
	 *
	 * @param nDx The x delta. Must be either -1, 0 or +1.
	 * @param nDy The y delta. Must be either -1, 0 or +1.
	 * @return The direction or undefined if abs(nDx + nDy) not 1.
	 */
	static VALUE fromDelta(int32_t nDx, int32_t nDy) noexcept
	{
		assert(((nDx == 0) &&  ((nDy == -1) || (nDy == 1)))
				|| ((nDy == 0) &&  ((nDx == -1) || (nDx == 1))));
		return static_cast<VALUE>((3*nDx+nDy+3)/2);
	}
	/** The x delta of a given direction.
	 * @param eDir The direction.
	 * @return The x delta.
	 */
	inline static int32_t deltaX(VALUE eDir) noexcept
	{
		return (2 * eDir - 3) / 3;
	}
	/** The y delta of a given direction.
	 * @param eDir The direction.
	 * @return The y delta.
	 */
	inline static int32_t deltaY(VALUE eDir) noexcept
	{
		return ((3 - eDir) * eDir) / (4 * eDir - 6);
	}
	/** The clockwise rotated direction.
	 * Example: The clockwise rotation of `Direction::UP` is `Direction::RIGHT`.
	 * @param eDir The direction.
	 * @return The new direction.
	 */
	inline static VALUE rotateCW(VALUE eDir) noexcept
	{
		return static_cast<VALUE>((2 * eDir + 1) % 5);
	}
	/** The counter clockwise rotated direction.
	 * Example: The counter clockwise rotation of `Direction::LEFT` is `Direction::DOWN`.
	 * @param eDir The direction.
	 * @return The new direction.
	 */
	inline static VALUE rotateCCW(VALUE eDir) noexcept
	{
		return static_cast<VALUE>((7 - 2 * eDir) % 5);
	}
	/** The opposite direction.
	 * Example: The opposite direction of `Direction::DOWN` is `Direction::UP`.
	 * @param eDir The direction.
	 * @return The new direction.
	 */
	inline static VALUE opposite(VALUE eDir) noexcept
	{
		return static_cast<VALUE>(3 - eDir);
	}
};

} // namespace stmg

#endif	/* STMG_DIRECTION_H */

