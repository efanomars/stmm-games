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
 * File:   newrows.cc
 */

#include "utile/newrows.h"

#include "randomsource.h"
#include "util/basictypes.h"
#include "utile/randomtiles.h"
#include "utile/tilebuffer.h"

#include <memory>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <utility>


namespace stmg
{

NewRows::Distr::Distr(DISTR_TYPE eDistrType) noexcept
: m_eDistrType(eDistrType)
{
	assert((m_eDistrType >= DISTR_TYPE_FIRST) && (m_eDistrType <= DISTR_TYPE_LAST));
}

NewRows::NewRows(RandomSource& oRandomSource, Init&& oInit) noexcept
: m_oRandomSource(oRandomSource)
, m_aNewRowGens(std::move(oInit.m_aNewRowGens))
, m_nMinW(0)
{
	for (RandomTiles::ProbTileGen& oProbTileGen : oInit.m_aRandomTiles) {
		auto refRT = std::make_unique<RandomTiles>(oRandomSource, std::move(oProbTileGen));
		m_aRandomTiles.push_back(std::move(refRT));
	}
	#ifndef NDEBUG
	const int32_t nTotRandomTiles = static_cast<int32_t>(m_aRandomTiles.size());
	#endif //NDEBUG
	assert(m_aNewRowGens.size() > 0);
	for (NewRowGen& oNewRowGen : m_aNewRowGens) {
		for (unique_ptr<Distr>& refDistr : oNewRowGen.m_aDistrs) {
			assert(refDistr.get() != nullptr);
			switch (refDistr->getType())
			{
			case DISTR_FIXED:
				{
					DistrFixed& oTilesDistr = *(static_cast<DistrFixed*>(refDistr.get()));
					m_nMinW = std::max(m_nMinW, static_cast<int32_t>(oTilesDistr.m_aTiles.size()));
					#ifndef NDEBUG
					int32_t nNonEmpty = 0;
					for (const Tile& oTile : oTilesDistr.m_aTiles) {
						if (! oTile.isEmpty()) {
							++nNonEmpty;
						}
					}
					assert(nNonEmpty > 0);
					#endif //NDEBUG
				}
				break;
			case DISTR_RAND_SINGLE:
				{
					#ifndef NDEBUG
					DistrRandSingle& oTilesDistr = *(static_cast<DistrRandSingle*>(refDistr.get()));
					//m_nMinW = std::max(m_nMinW, oTilesDistr.m_nLeaveEmpty + oTilesDistr.m_nFill);
					assert(oTilesDistr.m_nLeaveEmpty >= 0);
					assert(oTilesDistr.m_nFill >= -1);
					assert(! oTilesDistr.m_oTile.isEmpty());
					#endif //NDEBUG
				}
				break;
			case DISTR_RAND_POS_SINGLE:
				{
					DistrRandPosSingle& oTilesDistr = *(static_cast<DistrRandPosSingle*>(refDistr.get()));
					assert(oTilesDistr.m_oPositions.size() > 0);
					const int32_t nTotPositions = oTilesDistr.m_oPositions.size();
					m_nMinW = std::max(m_nMinW, oTilesDistr.m_oPositions.getValueByIndex(nTotPositions - 1) + 1);
					assert((oTilesDistr.m_nLeaveEmpty >= 0) && (oTilesDistr.m_nLeaveEmpty < nTotPositions));
					assert(oTilesDistr.m_nFill >= -1);
					if ((oTilesDistr.m_nFill < 0) || (oTilesDistr.m_nFill > (nTotPositions - oTilesDistr.m_nLeaveEmpty))) {
						oTilesDistr.m_nFill = (nTotPositions - oTilesDistr.m_nLeaveEmpty);
					}
					assert(! oTilesDistr.m_oTile.isEmpty());
				}
				break;
			case DISTR_RAND_TILES:
				{
					#ifndef NDEBUG
					DistrRandTiles& oTilesDistr = *(static_cast<DistrRandTiles*>(refDistr.get()));
					assert(oTilesDistr.m_nLeaveEmpty >= 0);
					assert(oTilesDistr.m_nFill >= -1);
					assert((oTilesDistr.m_nRandomTilesIdx >= 0) && (oTilesDistr.m_nRandomTilesIdx < nTotRandomTiles));
					#endif //NDEBUG
				}
				break;
			case DISTR_RAND_POS_TILES:
				{
					DistrRandPosTiles& oTilesDistr = *(static_cast<DistrRandPosTiles*>(refDistr.get()));
					assert(oTilesDistr.m_oPositions.size() > 0);
					const int32_t nTotPositions = oTilesDistr.m_oPositions.size();
					m_nMinW = std::max(m_nMinW, oTilesDistr.m_oPositions.getValueByIndex(nTotPositions - 1) + 1);
					assert((oTilesDistr.m_nLeaveEmpty >= 0) && (oTilesDistr.m_nLeaveEmpty < nTotPositions));
					assert(oTilesDistr.m_nFill >= -1);
					if ((oTilesDistr.m_nFill < 0) || (oTilesDistr.m_nFill > (nTotPositions - oTilesDistr.m_nLeaveEmpty))) {
						oTilesDistr.m_nFill = (nTotPositions - oTilesDistr.m_nLeaveEmpty);
					}
					assert((oTilesDistr.m_nRandomTilesIdx >= 0) && (oTilesDistr.m_nRandomTilesIdx < nTotRandomTiles));
				}
				break;
			default:
				{
					assert(false);
				}
				break;
			}
		}
	}
}
int32_t NewRows::getTotNewRowGens() const noexcept
{
	return static_cast<int32_t>(m_aNewRowGens.size());
}
int32_t NewRows::getTotRandomTileGens() const noexcept
{
	return static_cast<int32_t>(m_aRandomTiles.size());
}
int32_t NewRows::getMinSize() const noexcept
{
	return m_nMinW;
}
void NewRows::createNewRow(int32_t nNewRowGenIdx, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	assert((nNewRowGenIdx >= 0) && (nNewRowGenIdx < getTotNewRowGens()));
	assert((nRow >= 0) && (nRow < oTileBuffer.getH()));
	assert(oTileBuffer.getW() > 0);
	assert(oTileBuffer.getW() >= m_nMinW);
	NewRowGen& oNewRowGen = m_aNewRowGens[nNewRowGenIdx];
	const int32_t nTotTilesDistrs = static_cast<int32_t>(oNewRowGen.m_aDistrs.size());

	std::vector<int32_t> aFreePositions;
	const int32_t nW = oTileBuffer.getW();
	for (int32_t nX = 0; nX < nW; ++nX) {
		const bool bIsEmpty = oTileBuffer.get(NPoint{nX, nRow}).isEmpty();
		if (bIsEmpty) {
			aFreePositions.push_back(nX);
		}
	}
	IntSet oFreePositions{aFreePositions, true};
	//
	for (int32_t nCutTileDistr = 0; nCutTileDistr < nTotTilesDistrs; ++nCutTileDistr) {
		const bool bLast = (nCutTileDistr == nTotTilesDistrs - 1);
		const auto& refDistr = oNewRowGen.m_aDistrs[nCutTileDistr];
		switch (refDistr->getType())
		{
		case DISTR_FIXED:
			{
				const DistrFixed& oTilesDistr = *(static_cast<DistrFixed*>(refDistr.get()));
				fillPositions(oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow);
			}
			break;
		case DISTR_RAND_SINGLE:
			{
				const DistrRandSingle& oTilesDistr = *(static_cast<DistrRandSingle*>(refDistr.get()));
				fillPositions(oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow);
			}
			break;
		case DISTR_RAND_POS_SINGLE:
			{
				const DistrRandPosSingle& oTilesDistr = *(static_cast<DistrRandPosSingle*>(refDistr.get()));
				fillPositions(oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow);
			}
			break;
		case DISTR_RAND_TILES:
			{
				const DistrRandTiles& oTilesDistr = *(static_cast<DistrRandTiles*>(refDistr.get()));
				fillPositions(oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow);
			}
			break;
		case DISTR_RAND_POS_TILES:
			{
				const DistrRandPosTiles& oTilesDistr = *(static_cast<DistrRandPosTiles*>(refDistr.get()));
				fillPositions(oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow);
			}
			break;
		default:
			{
				assert(false);
			}
			break;
		}
	}
}
template<class TDistr, class TTileSetter>
static void fillPositionsWS(RandomSource& oRandomSource, const IntSet& oWorkSet
							, const TDistr& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow, TTileSetter oTS) noexcept
{
//#ifndef NDEBUG
//std::cout << "fillPositionsWS() oWorkSet=";
//oWorkSet.dump(5, "?");
//std::cout << '\n';
//#endif //NDEBUG
	const int32_t nW = oTileBuffer.getW();
	const int32_t nFill = ((oTilesDistr.m_nFill >= 0) ? oTilesDistr.m_nFill : nW);
	const int32_t nTotFree = oWorkSet.size();
	const int32_t nToFill = std::min(nTotFree - oTilesDistr.m_nLeaveEmpty, nFill);
	if (nToFill <= 0) {
		return; //--------------------------------------------------------------
	}
	std::vector<int32_t> aFillPositions;
	const int32_t nSize = nTotFree;
	for (int32_t nCount = 0; nCount < nToFill; ++nCount) {
		// decide which empty tile should be filled
		int32_t nFreePos = oRandomSource.random(0, (nSize - nCount) - 1);
		int32_t nX = -1;
		int32_t nIdx = 0;
		for ( ; nIdx < nSize; ++nIdx) {
			nX = oWorkSet.getValueByIndex(nIdx);
			assert(nX < nW);
			if (oTileBuffer.get({nX, nRow}).isEmpty()) {
				if (nFreePos == 0) {
					break; // for nIdx ------
				}
				--nFreePos;
			}
		}
		assert(nIdx < nSize);
		Tile& oCurTile = oTileBuffer.get({nX, nRow});
		oCurTile = oTS(); //m_aRandomTiles[oTilesDistr.m_nRandomTilesIdx]->createTile();
		aFillPositions.push_back(nX);
	}
	if (! bLast) {
		oFreePositions = IntSet::makeDifference(oFreePositions, IntSet{std::move(aFillPositions), false});
	}
}
template<class TDistr, class TTileSetter>
static void fillPositionsPos(RandomSource& oRandomSource
							, const TDistr& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow, TTileSetter oTS) noexcept
{
	const IntSet oWorkSet = IntSet::makeIntersection(oFreePositions, oTilesDistr.m_oPositions);
	fillPositionsWS(oRandomSource, oWorkSet, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, oTS);
}
template<class TDistr, class TTileSetter>
static void fillPositionsAll(RandomSource& oRandomSource
							, const TDistr& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow, TTileSetter oTS) noexcept
{
//#ifndef NDEBUG
//std::cout << "fillPositionsAll() oFreePositions=";
//oFreePositions.dump(2, "?");
//std::cout << '\n';
//#endif //NDEBUG
	const IntSet& oWorkSet = oFreePositions;
	fillPositionsWS(oRandomSource, oWorkSet, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, oTS);
}
void NewRows::fillPositions(const DistrFixed& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	std::vector<int32_t> aFillPositions;

	const int32_t nSize = oFreePositions.size();
	const int32_t nW = static_cast<int32_t>(oTilesDistr.m_aTiles.size());
	for (int32_t nIdx = 0; nIdx < nSize; ++nIdx) {
		const int32_t nX = oFreePositions.getValueByIndex(nIdx);
		Tile& oCurTile = oTileBuffer.get({nX, nRow});
		if (! oCurTile.isEmpty()) {
			continue;
		}
		if (nX >= nW) {
			// we know that oFreePositions values are ordered, all following will also be >= nW
			break;
		}
		const Tile& oNewTile = oTilesDistr.m_aTiles[nX];
		if (oNewTile.isEmpty()) {
			continue;
		}
		oCurTile = oNewTile;
		aFillPositions.push_back(nX);
	}
	if (! bLast) {
		oFreePositions = IntSet::makeDifference(oFreePositions, IntSet{std::move(aFillPositions), true});
	}
}
void NewRows::fillPositions(const DistrRandSingle& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	stmg::fillPositionsAll(m_oRandomSource, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, [&]() -> const Tile&
	{
		return oTilesDistr.m_oTile;
	});
}
void NewRows::fillPositions(const DistrRandPosSingle& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	stmg::fillPositionsPos(m_oRandomSource, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, [&]() -> const Tile&
	{
		return oTilesDistr.m_oTile;
	});
}
void NewRows::fillPositions(const DistrRandTiles& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	stmg::fillPositionsAll(m_oRandomSource, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, [&]() -> Tile
	{
		return m_aRandomTiles[oTilesDistr.m_nRandomTilesIdx]->createTile();
	});
}
void NewRows::fillPositions(const DistrRandPosTiles& oTilesDistr, bool bLast, IntSet& oFreePositions, TileBuffer& oTileBuffer, int32_t nRow) noexcept
{
	stmg::fillPositionsPos(m_oRandomSource, oTilesDistr, bLast, oFreePositions, oTileBuffer, nRow, [&]() -> Tile
	{
		return m_aRandomTiles[oTilesDistr.m_nRandomTilesIdx]->createTile();
	});
}

} // namespace stmg
