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
 * File:   coords.cc
 */

#include "util/coords.h"

#include "tile.h"
#include "util/basictypes.h"
#include "util/util.h"

#include "utile/tilecoords.h"

#include <cassert>
#include <algorithm>
//#include <iostream>
#include <limits>

namespace stmg
{

Coords::const_iterator::const_iterator(std::unordered_map<int64_t, int32_t>::const_iterator it) noexcept
: m_it(std::move(it))
{
}
bool Coords::remove(const const_iterator& it) noexcept
{
	if (it.m_it == m_oXY.end()) {
		return false;
	}
	removePriv(it.m_it);
	return true;
}
bool Coords::remove(int32_t nX, int32_t nY) noexcept
{
	const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
	const auto itFind = m_oXY.find(nXY);
	if (itFind == m_oXY.end()) {
		return false;
	}
	removePriv(itFind);
	return true;
}
std::unordered_map<int64_t, int32_t>::iterator Coords::removePriv(const std::unordered_map<int64_t, int32_t>::const_iterator& itFind) noexcept
{
	const int32_t nIdx = itFind->second;
	std::unordered_map<int64_t, int32_t>::iterator itNext = m_oXY.erase(itFind);
	if (m_bTileCoords && (nIdx >= 0)) {
		const auto oPairModified = static_cast<TileCoords*>(this)->removeIndex(nIdx);
		const int32_t nOtherIdx = oPairModified.second;
		if (nOtherIdx >= 0) {
			const int64_t nOtherXY = Util::packPointToInt64(oPairModified.first);
			const auto itFindOther = m_oXY.find(nOtherXY);
			assert(itFindOther != m_oXY.end());
			itFindOther->second = nOtherIdx;
		}
	}
	const int32_t nCurW = m_oBoundingRect.m_nW;
	if (nCurW > 0) {
		if (m_oXY.empty()) {
			m_oBoundingRect.m_nW = 0;
			m_oBoundingRect.m_nH = 0;
		} else {
			// mark bounding box as invalid
			m_oBoundingRect.m_nW = -1;
		}
	}
	return itNext;
}
void Coords::removeInRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept
{
	std::unordered_map<int64_t, int32_t>::iterator it = m_oXY.begin();
	while (it != m_oXY.end()) {
		const int64_t nXY = it->first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nCurX = oXY.m_nX;
		const int32_t nCurY = oXY.m_nY;
		if ((nCurX >= nX) && (nCurX < nX + nW) && (nCurY >= nY) && (nCurY < nY + nH)) {
			it = removePriv(it);
		} else {
			++it;
		}
	}
}


Coords::Coords(bool bTileCoords) noexcept
: m_bTileCoords(bTileCoords)
{
}
Coords::Coords(bool bTileCoords, int32_t nAtLeastSize) noexcept
: m_oXY(nAtLeastSize)
, m_bTileCoords(bTileCoords)
{
}

Coords::Coords() noexcept
: Coords(false)
{
}
Coords::Coords(int32_t nAtLeastSize) noexcept
: Coords(false, nAtLeastSize)
{
}
void Coords::reInit(int32_t nAtLeastSize) noexcept
{
	if (m_bTileCoords) {
		return static_cast<TileCoords*>(this)->reInit(nAtLeastSize); //---------
	}
	Coords::clearData(nAtLeastSize);
}
void Coords::reInit() noexcept
{
	reInit(0);
}
void Coords::clearData(int32_t nAtLeastSize) noexcept
{
	if (nAtLeastSize > static_cast<int32_t>(m_oXY.bucket_count())) {
		m_oXY.rehash(nAtLeastSize);
	}
	m_oXY.clear();
	m_oBoundingRect.m_nW = 0;
	m_oBoundingRect.m_nH = 0;
}
int32_t Coords::size() const noexcept
{
	return static_cast<int32_t>(m_oXY.size());
}
bool Coords::isEmpty() const noexcept
{
	return m_oXY.empty();
}
void Coords::add(int32_t nX, int32_t nY) noexcept
{
	add(NPoint{nX, nY});
}
void Coords::add(NPoint oXY) noexcept
{
	if (m_bTileCoords) {
		static_cast<TileCoords*>(this)->add(oXY, Tile::s_oEmptyTile);
	} else {
		getOrCreate(oXY);
	}
}
int32_t& Coords::getOrCreate(NPoint oXY) noexcept
{
	const int64_t nXY = Util::packPointToInt64(oXY);
//std::cout << "Coords::getOrCreate nX=" << oXY.m_nX << " nY=" << oXY.m_nY << '\n';
	auto oPair = m_oXY.insert(std::make_pair(nXY, -1));
	if (oPair.second) {
		// new element
		addData(oXY.m_nX, oXY.m_nY);
	}
	return oPair.first->second;
}
void Coords::addData(int32_t nX, int32_t nY) noexcept
{
	const int32_t nCurW = m_oBoundingRect.m_nW;
	if (nCurW < 0) {
		// a remove invalidated the bounding rect
		return; //--------------------------------------------------------------
	} else if (nCurW == 0) {
		m_oBoundingRect.m_nX = nX;
		m_oBoundingRect.m_nY = nY;
		m_oBoundingRect.m_nW = 1;
		m_oBoundingRect.m_nH = 1;
		return; //--------------------------------------------------------------
	}
	int32_t nDeltaW = m_oBoundingRect.m_nX - nX;
	if (nDeltaW > 0) {
		m_oBoundingRect.m_nX -= nDeltaW;
		m_oBoundingRect.m_nW += nDeltaW;
	} else {
		nDeltaW = (nX + 1) - (m_oBoundingRect.m_nX + nCurW);
		if (nDeltaW > 0) {
			m_oBoundingRect.m_nW += nDeltaW;
		}
	}
	int32_t nDeltaH = m_oBoundingRect.m_nY - nY;
	if (nDeltaH > 0) {
		m_oBoundingRect.m_nY -= nDeltaH;
		m_oBoundingRect.m_nH += nDeltaH;
	} else {
		nDeltaH = (nY + 1) - (m_oBoundingRect.m_nY + m_oBoundingRect.m_nH);
		if (nDeltaH > 0) {
			m_oBoundingRect.m_nH += nDeltaH;
		}
	}
}
void Coords::addRect(NRect oRect) noexcept
{
	addRect(oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH);
}
void Coords::addRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH) noexcept
{
	for (int32_t nCurX = nX; nCurX < nX + nW; ++nCurX) {
		for (int32_t nCurY = nY; nCurY < nY + nH; ++nCurY) {
			add(nCurX, nCurY);
		}
	}
}
bool Coords::contains(NPoint oXY) const noexcept
{
	const int64_t nXY = Util::packPointToInt64(oXY);
	return m_oXY.find(nXY) != m_oXY.end();
}
bool Coords::contains(int32_t nX, int32_t nY) const noexcept
{
	return contains(NPoint{nX, nY});
}
bool Coords::remove(NPoint oXY) noexcept
{
	return remove(oXY.m_nX, oXY.m_nY);
}
void Coords::removeInRect(NRect oRect) noexcept
{
	removeInRect(oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH);
}
NRect Coords::getMinMax() const noexcept
{
	const int32_t nCurW = m_oBoundingRect.m_nW;
	if (nCurW > 0) {
		return m_oBoundingRect; //----------------------------------------------
	}
	if (nCurW == 0) {
		assert(m_oXY.empty());
		return m_oBoundingRect; //----------------------------------------------
	}
	assert(! m_oXY.empty());
	m_oBoundingRect.m_nX = std::numeric_limits<int32_t>::max();
	int32_t nMaxX = std::numeric_limits<int32_t>::lowest();
	m_oBoundingRect.m_nY = std::numeric_limits<int32_t>::max();
	int32_t nMaxY = std::numeric_limits<int32_t>::lowest();
	for (const auto& oPair : m_oXY) {
		const int64_t nXY = oPair.first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nX = oXY.m_nX;
		const int32_t nY = oXY.m_nY;

		if (nX < m_oBoundingRect.m_nX) {
			m_oBoundingRect.m_nX = nX;
		}
		if (nX > nMaxX) {
			nMaxX = nX;
		}
		if (nY < m_oBoundingRect.m_nY) {
			m_oBoundingRect.m_nY = nY;
		}
		if (nY > nMaxY) {
			nMaxY = nY;
		}
	}
	if (m_oBoundingRect.m_nX <= nMaxX) {
		m_oBoundingRect.m_nW = nMaxX - m_oBoundingRect.m_nX + 1;
		m_oBoundingRect.m_nH = nMaxY - m_oBoundingRect.m_nY + 1;
	} else {
		m_oBoundingRect.m_nW = 0;
		m_oBoundingRect.m_nH = 0;
	}
	return m_oBoundingRect;
}

void Coords::add(const Coords& oCoords) noexcept
{
	if (&m_oXY == &(oCoords.m_oXY)) {
		return;
	}
	for (const auto& oPair : oCoords.m_oXY) {
		const int64_t nXY = oPair.first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		add(oXY);
	}
}
void Coords::remove(const Coords& oCoords) noexcept
{
	if (m_bTileCoords) {
		static_cast<TileCoords*>(this)->remove(oCoords);
		return;
	}
	if (&m_oXY == &(oCoords.m_oXY)) {
		reInit();
		return; //--------------------------------------------------------------
	}
	for (const auto& oPair : oCoords.m_oXY) {
		const int64_t nXY = oPair.first;
		const auto itFind = m_oXY.find(nXY);
		if (itFind != m_oXY.end()) {
			m_oXY.erase(itFind);
		}
	}
}
Coords::const_iterator Coords::find(int32_t nX, int32_t nY) const noexcept
{
	const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
	const auto itFind = m_oXY.find(nXY);
	return const_iterator(itFind);
}

} // namespace stmg
