/*
 * File:   tilecoords.cc
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

#include "utile/tilecoords.h"

#include <cassert>
#include <algorithm>
#include <cstdint>

namespace stmg
{

TileCoords::const_iterator::const_iterator(const TileCoords& oTileCoords, Coords::const_iterator it) noexcept
: Coords::const_iterator(std::move(it))
, m_aPosTiles(oTileCoords.m_aPosTiles)
{
}

TileCoords::TileCoords() noexcept
: Coords(true)
{
}
TileCoords::TileCoords(int32_t nAtLeastSize) noexcept
: Coords(true, nAtLeastSize)
{
}
void TileCoords::reInit(int32_t nAtLeastSize) noexcept
{
	Coords::clearData(nAtLeastSize);
	m_aPosTiles.clear();
	m_aPosTiles.reserve(nAtLeastSize);
}
void TileCoords::add(int32_t nX, int32_t nY, const Tile& oTile) noexcept
{
	add(NPoint{nX, nY}, oTile);
}
std::pair<NPoint, int32_t> TileCoords::removeIndex(int32_t nIdx) noexcept
{
	const int32_t nLen = static_cast<int32_t>(m_aPosTiles.size());
//std::cout << "TileCoords::removeIndex nIdx=" << nIdx << "  nLen=" << nLen << '\n';
	assert((nIdx >= 0) && (nIdx < nLen));
	if (nIdx < nLen - 1) {
		PosTile& oRemove = m_aPosTiles[nIdx];
		oRemove = std::move(m_aPosTiles[nLen - 1]);
//		Coords::addDataOverwrite(m_aPosTiles[nIdx].m_nX, m_aPosTiles[nIdx].m_nY, nIdx);
		m_aPosTiles.pop_back();
		return std::make_pair(oRemove.m_oXY, nIdx);
	} else {
		m_aPosTiles.pop_back();
		return std::make_pair(NPoint{0,0}, -1);
	}
}
void TileCoords::add(NPoint oXY, const Tile& oTile) noexcept
{
	const bool bIsEmpty = oTile.isEmpty();
	if (bIsEmpty) {
//		const std::pair<bool, int32_t*> oPair = Coords::getData(nX, nY);
		int32_t& nCurIdx = getOrCreate(oXY);
		if (nCurIdx < 0) {
			// unchanged (already was empty tile) or just created
			return; //----------------------------------------------------------
		}
//std::cout << "TileCoords::add nX=" << oXY.m_nX << "  nY=" << oXY.m_nY << '\n';
//std::cout << "TileCoords::add to empty nCurIdx=" << nCurIdx << '\n';
		// was non empty, remove
		const auto oOtherPair = removeIndex(nCurIdx);
		const int32_t nNewOtherIdx = oOtherPair.second;
//std::cout << "TileCoords::add to empty nNewOtherIdx=" << nNewOtherIdx << '\n';
		if (nNewOtherIdx >= 0) {
			const NPoint& oOtherXY = oOtherPair.first;
			int32_t& nOtherIdx = getOrCreate(oOtherXY);
//std::cout << "TileCoords::add to empty nOtherIdx=" << nOtherIdx << '\n';
			nOtherIdx = nNewOtherIdx;
		}
		// set to empty
		nCurIdx = -1;
	} else {
		int32_t& nCurIdx = getOrCreate(oXY);
		if (nCurIdx < 0) {
			// newly created or was empty tile
			// get new index
			const int32_t nNewIdx = static_cast<int32_t>(m_aPosTiles.size());
			nCurIdx = nNewIdx;
			// add tile
			PosTile oPosTile;
			oPosTile.m_oXY = oXY;
			oPosTile.m_oTile = oTile;
			m_aPosTiles.push_back(std::move(oPosTile));
//std::cout << "TileCoords::add  nNewIdx=" << nNewIdx << "  nLen=" << m_aPosTiles.size() << '\n';
		} else {
			// overwrite tile
			m_aPosTiles[nCurIdx].m_oTile = oTile;
		}
	}
}
void TileCoords::addRect(NRect oRect, const Tile& oTile) noexcept
{
	addRect(oRect.m_nX, oRect.m_nY, oRect.m_nW, oRect.m_nH, oTile);
}
void TileCoords::addRect(int32_t nX, int32_t nY, int32_t nW, int32_t nH, const Tile& oTile) noexcept
{
	for (int32_t nCurX = nX; nCurX < nX + nW; ++nCurX) {
		for (int32_t nCurY = nY; nCurY < nY + nH; ++nCurY) {
			add(nCurX, nCurY, oTile);
		}
	}
}
std::pair<bool, Tile> TileCoords::getTile(NPoint oXY) const noexcept
{
	return getTile(oXY.m_nX, oXY.m_nY);
}
std::pair<bool, Tile> TileCoords::getTile(int32_t nX, int32_t nY) const noexcept
{
	auto itFind = Coords::find(nX, nY);
	if (itFind == Coords::end()) {
		return std::make_pair(false, Tile{}); //--------------------------------
	}
	const int32_t nIdx = itFind.get();
	if (nIdx < 0) {
		return std::make_pair(true, Tile{}); //--------------------------------
	}
	assert(nIdx < static_cast<int32_t>(m_aPosTiles.size()));
	return std::make_pair(true, m_aPosTiles[nIdx].m_oTile);
}
//bool TileCoords::remove(int32_t nX, int32_t nY) noexcept
//{
//	const std::pair<bool, int32_t> oPair = Coords::removeGetData(nX, nY);
//	if (! oPair.first) {
//		return false; //--------------------------------------------------------
//	}
//	const int32_t nIdx = oPair.second;
//	if (nIdx < 0) {
//		return true; //---------------------------------------------------------
//	}
//	removeIndex(nIdx);
//	return true;
//}
//bool TileCoords::remove(NPoint oXY) noexcept
//{
//	return remove(oXY.m_nX, oXY.m_nY);
//}
void TileCoords::add(const TileCoords& oTileCoords) noexcept
{
	if (&m_aPosTiles == &(oTileCoords.m_aPosTiles)) {
		return;
	}
	for (TileCoords::const_iterator it = oTileCoords.begin(); it != oTileCoords.end(); it.next()) {
		const int32_t nIdx = it.get();
		const Tile& oTile = ((nIdx < 0) ? Tile::s_oEmptyTile : oTileCoords.m_aPosTiles[nIdx].m_oTile);
		add(it.x(), it.y(), oTile);
	}
}
void TileCoords::remove(const Coords& oCoords) noexcept
{
	if (static_cast<Coords*>(this) == &oCoords) {
		Coords::reInit();
		m_aPosTiles.clear();
		return; //--------------------------------------------------------------
	}
	for (auto it = oCoords.begin(); it != oCoords.end(); it.next()) {
//std::cout << "TileCoords::remove it.x() = " << it.x() << "  it.y() = " << it.y() << '\n';
		remove(it.x(), it.y());
	}
//std::cout << "TileCoords::remove END" << '\n';
}

} // namespace stmg
