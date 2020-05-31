/*
 * File:   basictypes.cc
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

#include "util/basictypes.h"

#include <cassert>
#include <algorithm>

namespace stmg
{

bool operator==(const NPoint& oP1, const NPoint& oP2) noexcept
{
	return ((oP1.m_nX == oP2.m_nX) && (oP1.m_nY == oP2.m_nY));
}
bool operator==(const NSize& oS1, const NSize& oS2) noexcept
{
	return ((oS1.m_nW == oS2.m_nW) && (oS1.m_nH == oS2.m_nH));
}
bool operator==(const NRect& oR1, const NRect& oR2) noexcept
{
	return ((oR1.m_nX == oR2.m_nX) && (oR1.m_nY == oR2.m_nY) && (oR1.m_nW == oR2.m_nW) && (oR1.m_nH == oR2.m_nH));
}
bool operator==(const FPoint& oP1, const FPoint& oP2) noexcept
{
	return ((oP1.m_fX == oP2.m_fX) && (oP1.m_fY == oP2.m_fY));
}
bool operator==(const FSize& oS1, const FSize& oS2) noexcept
{
	return ((oS1.m_fW == oS2.m_fW) && (oS1.m_fH == oS2.m_fH));
}
bool operator==(const FRect& oR1, const FRect& oR2) noexcept
{
	return ((oR1.m_fX == oR2.m_fX) && (oR1.m_fY == oR2.m_fY) && (oR1.m_fW == oR2.m_fW) && (oR1.m_fH == oR2.m_fH));
}

bool NRect::doIntersect(const NRect& oR1, const NRect& oR2) noexcept
{
	assert((oR1.m_nW >= 0) && (oR1.m_nH >= 0) && (oR2.m_nW >= 0) && (oR2.m_nH >= 0));
	return !((oR1.m_nX + oR1.m_nW <= oR2.m_nX) || (oR2.m_nX + oR2.m_nW <= oR1.m_nX)
				|| (oR1.m_nY + oR1.m_nH <= oR2.m_nY) || (oR2.m_nY + oR2.m_nH <= oR1.m_nY));
}
NRect NRect::intersectionRect(const NRect& oR1, const NRect& oR2) noexcept
{
	assert((oR1.m_nW >= 0) && (oR1.m_nH >= 0) && (oR2.m_nW >= 0) && (oR2.m_nH >= 0));
	NRect oRr;
	oRr.m_nX = std::max(oR1.m_nX, oR2.m_nX);
	const int32_t nToX = std::min(oR1.m_nX + oR1.m_nW - 1, oR2.m_nX + oR2.m_nW - 1);
	if (oRr.m_nX > nToX) {
		return NRect{};
	}
	oRr.m_nY = std::max(oR1.m_nY, oR2.m_nY);
	const int32_t nToY = std::min(oR1.m_nY + oR1.m_nH - 1, oR2.m_nY + oR2.m_nH - 1);
	if (oRr.m_nY > nToY) {
		return NRect{};
	}
	oRr.m_nW = nToX - oRr.m_nX + 1;
	oRr.m_nH = nToY - oRr.m_nY + 1;
	return oRr;
}
NRect NRect::boundingRect(const NRect& oR1, const NRect& oR2) noexcept
{
	assert((oR1.m_nW >= 0) && (oR1.m_nH >= 0) && (oR2.m_nW >= 0) && (oR2.m_nH >= 0));
	NRect oRr;
	oRr.m_nX = std::min(oR1.m_nX, oR2.m_nX);
	oRr.m_nY = std::min(oR1.m_nY, oR2.m_nY);
	const int32_t nMaxX = std::max(oR1.m_nX + oR1.m_nW, oR2.m_nX + oR2.m_nW);
	const int32_t nMaxY = std::max(oR1.m_nY + oR1.m_nH, oR2.m_nY + oR2.m_nH);
	oRr.m_nW = nMaxX - oRr.m_nX;
	oRr.m_nH = nMaxY - oRr.m_nY;
	return oRr;
}

bool FRect::doIntersect(const FRect& oR1, const FRect& oR2) noexcept
{
	assert((oR1.m_fW >= 0) && (oR1.m_fH >= 0) && (oR2.m_fW >= 0) && (oR2.m_fH >= 0));
	return !((oR1.m_fX + oR1.m_fW <= oR2.m_fX) || (oR2.m_fX + oR2.m_fW <= oR1.m_fX)
				|| (oR1.m_fY + oR1.m_fH <= oR2.m_fY) || (oR2.m_fY + oR2.m_fH <= oR1.m_fY));
}
FRect FRect::boundingRect(const FRect& oR1, const FRect& oR2) noexcept
{
	assert((oR1.m_fW >= 0) && (oR1.m_fH >= 0) && (oR2.m_fW >= 0) && (oR2.m_fH >= 0));
	FRect oRr;
	oRr.m_fX = std::min(oR1.m_fX, oR2.m_fX);
	oRr.m_fY = std::min(oR1.m_fY, oR2.m_fY);
	const double fMaxX = std::max(oR1.m_fX + oR1.m_fW, oR2.m_fX + oR2.m_fW);
	const double fMaxY = std::max(oR1.m_fY + oR1.m_fH, oR2.m_fY + oR2.m_fH);
	oRr.m_fW = fMaxX - oRr.m_fX;
	oRr.m_fH = fMaxY - oRr.m_fY;
	return oRr;
}

} // namespace stmg
