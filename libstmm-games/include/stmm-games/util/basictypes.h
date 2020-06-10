/*
 * File:   basictypes.h
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

#ifndef STMG_BASIC_TYPES_H
#define STMG_BASIC_TYPES_H

#include <functional>
#include <cstdint>
#include <iosfwd>

#include <stdint.h>

namespace stmg
{

////////////////////////////////////////////////////////////////////////////////
struct NPoint
{
	int32_t m_nX = 0; /**< Default is 0. */
	int32_t m_nY = 0; /**< Default is 0. */
};
bool operator==(const NPoint& oP1, const NPoint& oP2) noexcept;
struct NSize
{
	int32_t m_nW = 0; /**< Default is 0. */
	int32_t m_nH = 0; /**< Default is 0. */
};
bool operator==(const NSize& oS1, const NSize& oS2) noexcept;


////////////////////////////////////////////////////////////////////////////////
struct NRect
{
	int32_t m_nX = 0; /**< Default is 0. */
	int32_t m_nY = 0; /**< Default is 0. */
	int32_t m_nW = 0; /**< Default is 0. */
	int32_t m_nH = 0; /**< Default is 0. */
	bool containsPoint(const NPoint& oP) const noexcept
	{
		return ((oP.m_nX >= m_nX) && (oP.m_nX < m_nX + m_nW) && (oP.m_nY >= m_nY) && (oP.m_nY < m_nY + m_nH));
	}
	NSize getSize() const noexcept
	{
		return NSize{m_nW, m_nH};
	}
	/** Whether two rectangles intersect.
	 * @param oR1 Must have positive size.
	 * @param oR2 Must have positive size.
	 * @return Whether intersect.
	 */
	static bool doIntersect(const NRect& oR1, const NRect& oR2) noexcept;
	/** The maximal rectangle contained by two rectangles.
	 * @param oR1 Must have positive size.
	 * @param oR2 Must have positive size.
	 * @return Intersection rectangle or 0 size rectangle if rectangles don't intersect.
	 */
	static NRect intersectionRect(const NRect& oR1, const NRect& oR2) noexcept;
	/** The minimal rectangle containing two rectangles.
	 * @param oR1 Must have positive size.
	 * @param oR2 Must have positive size.
	 * @return Bounding rectangle.
	 */
	static NRect boundingRect(const NRect& oR1, const NRect& oR2) noexcept;
};
bool operator==(const NRect& oR1, const NRect& oR2) noexcept;

////////////////////////////////////////////////////////////////////////////////
struct FPoint
{
	double m_fX = 0; /**< Default is 0. */
	double m_fY = 0; /**< Default is 0. */
};
bool operator==(const FPoint& oP1, const FPoint& oP2) noexcept;
struct FSize
{
	double m_fW = 0; /**< Default is 0. */
	double m_fH = 0; /**< Default is 0. */
};
bool operator==(const FSize& oS1, const FSize& oS2) noexcept;


////////////////////////////////////////////////////////////////////////////////
struct FRect
{
	double m_fX = 0; /**< Default is 0. */
	double m_fY = 0; /**< Default is 0. */
	double m_fW = 0; /**< Default is 0. */
	double m_fH = 0; /**< Default is 0. */
	FSize getSize() const noexcept
	{
		return FSize{m_fW, m_fH};
	}
	static bool doIntersect(const FRect& oR1, const FRect& oR2) noexcept;
	static FRect boundingRect(const FRect& oR1, const FRect& oR2) noexcept;
};
bool operator==(const FRect& oR1, const FRect& oR2) noexcept;


////////////////////////////////////////////////////////////////////////////////
struct RgbColor
{
	uint8_t m_nR = 0;
	uint8_t m_nG = 0;
	uint8_t m_nB = 0;
};

////////////////////////////////////////////////////////////////////////////////
struct NRange
{
	int32_t m_nFrom = 0;
	int32_t m_nTo = 0;
};

////////////////////////////////////////////////////////////////////////////////
struct NTimeRange
{
	NRange m_oTicks; /**< The range in game ticks. Adds up to m_oMillisec. */
	NRange m_oMillisec; /**< The range in milliseconds. Adds up to m_oTicks. */
	/** The cumulated range in ticks.
	 * @param fMillisecPerTick The tick interval. Must be positive.
	 * @return The range in ticks.
	 */
	NRange getCumulatedTicksRange(double fMillisecPerTick) const noexcept
	{
		const int32_t nFrom = m_oTicks.m_nFrom + static_cast<double>(m_oMillisec.m_nFrom) / fMillisecPerTick;
		const int32_t nTo = m_oTicks.m_nTo + static_cast<double>(m_oMillisec.m_nTo) / fMillisecPerTick;
		return NRange{nFrom, nTo};
	}
};

} // namespace stmg

namespace std
{

////////////////////////////////////////////////////////////////////////////////
template<>
struct hash<stmg::NPoint>
{
	/**  0001020304050607
	 *   x0x1x2x3
	 *           y0y1y2y3
	 */
    std::size_t operator()(const stmg::NPoint& oXY) const noexcept
    {
		static_assert(sizeof(std::size_t) >= sizeof(uint64_t), "Must be 64bit architecture");
		const std::size_t nHash = ((static_cast<std::size_t>(oXY.m_nY) << 32) & 0xFFFFFFFF00000000) | ((static_cast<std::size_t>(oXY.m_nX)) & 0x00000000FFFFFFFF);
        return nHash;
    }
};

} // namespace std

#endif	/* STMG_BASIC_TYPES_H */

