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
 * File:   tilesizing.h
 */

#ifndef STMG_TILE_SIZING_H
#define STMG_TILE_SIZING_H

#include <stmm-games/util/basictypes.h>

#include <stdint.h>

namespace stmg
{

/** Constraints to tile size and width to height ratio.
 */
class TileSizing
{
public:
	int32_t m_nMinW = 5; /**< Minimum width in pixels. Must be positive. Default is 5. */
	int32_t m_nMaxW = 1000; /**< Maximum width in pixels. Cannot be smaller than m_nMinW. Default is 1000. */
	int32_t m_nMinH = 5; /**< Minimum height in pixels. Must be positive. Default is 5. */
	int32_t m_nMaxH = 1000; /**< Maximum height in pixels. Cannot be smaller than m_nMinH. Default is 1000. */
	double m_fWHRatio = 1.0; /**< The fixed width to height ratio. Minimum is `0.001`. Maximum is `1000.0`. Default is 1. */
public:
	bool isValid() const noexcept;
	/** Calculates the best tile size given a requested width.
	 * The resulting size might not satisfy the constraints because they can't.
	 * @param nReqW The requested width.
	 * @return The tile size.
	 */
	NSize getBest(int32_t nReqW) const noexcept;
};


} // namespace stmg

#endif	/* STMG_TILE_SIZING_H */

