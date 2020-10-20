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
 * File:   querytileremoval.h
 */

#ifndef STMG_QUERY_TILE_REMOVAL_H
#define STMG_QUERY_TILE_REMOVAL_H

#include <stdint.h>

namespace stmg { class ExtendedBoard; }
namespace stmg { struct NRect; }

namespace stmg
{

class QueryTileRemoval
{
public:
	virtual ~QueryTileRemoval() noexcept = default;

	/** Tells whether tiles would be removed in an extended board.
		* The extended area doesn't need to even intersect with the initialization area.
		* @param oBoard The extended board.
		* @param oExtArea The extended area that could also be partly outside the level's board. Size must be positive.
		* @return The number of tiles that would be removed.
		*/
	virtual int32_t wouldRemoveTiles(const ExtendedBoard& oBoard, const NRect& oExtArea) noexcept = 0;
};

} // namespace stmg

#endif	/* STMG_QUERY_TILE_REMOVAL_H */

