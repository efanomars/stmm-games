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
 * File:   tilebuffer.h
 */

#ifndef STMG_TILE_BUFFER_H
#define STMG_TILE_BUFFER_H

#include "tile.h"

#include "utile/tilerect.h"

#include "util/xybuffer.h"
#include "util/basictypes.h"

#include <stdint.h>

namespace stmg
{

class TileBuffer : public TileRect
{
public:
	explicit TileBuffer(NSize oWH) noexcept
	: m_oBuf(oWH)
	{
	}
	TileBuffer(NSize oWH, const Tile& oTile) noexcept
	: m_oBuf(oWH, oTile)
	{
	}

	inline void reInit(NSize oWH) noexcept
	{
		m_oBuf.resize(oWH);
		m_oBuf.setAll(Tile::s_oEmptyTile);
	}
	inline void reInit(NSize oWH, const Tile& oTile) noexcept
	{
		m_oBuf.resize(oWH);
		m_oBuf.setAll(oTile);
	}

	int32_t getW() const noexcept override
	{
		return m_oBuf.getW();
	}
	int32_t getH() const noexcept override
	{
		return m_oBuf.getH();
	}
	const Tile& get(NPoint oXY) const noexcept override
	{
		return m_oBuf.get(oXY);
	}

	inline Tile& get(NPoint oXY) noexcept
	{
		return m_oBuf.get(oXY);
	}
	inline void set(NPoint oXY, const Tile& oTile) noexcept
	{
		m_oBuf.set(oXY, oTile);
	}
	inline void setAll(const Tile& oTile) noexcept
	{
		m_oBuf.setAll(oTile);
	}
	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces) const noexcept;
	void dump() const noexcept;
	#endif //NDEBUG
private:
	XYBuffer<Tile> m_oBuf;
};

} // namespace stmg

#endif	/* STMG_TILE_BUFFER_H */

