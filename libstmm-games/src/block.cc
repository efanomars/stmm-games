/*
 * File:   block.cc
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

#include "block.h"

#include "util/basictypes.h"
#include "util/util.h"

#include <vector>
#include <cassert>
#include <algorithm>
#include <limits>
#include <iostream>
#include <string>
#include <utility>


namespace stmg
{

const Tile Block::s_oNonEmptyTile = Block::getNonEmptyTile();


Block::Block() noexcept
: m_nTotBricks(0)
, m_nTotShapes(0)
, m_nMaxWidth(-1)
, m_nMaxHeight(-1)
, m_nWidestShapeId(-1)
, m_nHighestShapeId(-1)
{
}
void Block::assignBricks(const std::vector<Tile>& aBrick) noexcept
{
	m_aBrickId.resize(m_nTotBricks);
	m_aBrickIdx.resize(m_nTotBricks);
	m_aTile.resize(m_nTotBricks);
	const int32_t nAssBricks = std::min(static_cast<int32_t>(aBrick.size()), m_nTotBricks);
	for (int32_t nBrick = 0; nBrick < nAssBricks; ++nBrick) {
		m_aBrickId[nBrick] = nBrick;
		m_aBrickIdx[nBrick] = nBrick;
		const Tile& oTile = aBrick[nBrick];
		const bool bEmptyTile = oTile.isEmpty();
		m_aTile[nBrick] = (bEmptyTile ? s_oNonEmptyTile : oTile);
	}
	for (int32_t nBrick = nAssBricks; nBrick < m_nTotBricks; ++nBrick) {
		m_aBrickId[nBrick] = nBrick;
		m_aBrickIdx[nBrick] = nBrick;
		m_aTile[nBrick] = s_oNonEmptyTile;
	}
}
void Block::assignShape(Shape& oShape, const std::vector< std::tuple<bool, int32_t, int32_t> >& aBrickPos) noexcept
{
	oShape.m_aBrickPosX.resize(m_nTotBricks);
	oShape.m_aBrickPosY.resize(m_nTotBricks);
	oShape.m_aBrickVisible.resize(m_nTotBricks);
	const int32_t nAssShapeBricks = std::min(static_cast<int32_t>(aBrickPos.size()), m_nTotBricks);
	for (int32_t nBrick = 0; nBrick < nAssShapeBricks; ++nBrick) {
		bool bVisible;
		int32_t nPosX, nPosY;
		std::tie(bVisible, nPosX, nPosY) = aBrickPos[nBrick];
		if (bVisible) {
			const int64_t nXY = Util::packPointToInt64(NPoint{nPosX, nPosY});
			auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
			if (itFind != oShape.m_oPosOfVisibleBrick.end()) {
				// There already is a visible brick at the position
				bVisible = false;
			} else {
				// Add to unique positions of visible bricks
				oShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrick));
			}
		}
		oShape.m_aBrickPosX[nBrick] = nPosX;
		oShape.m_aBrickPosY[nBrick] = nPosY;
		oShape.m_aBrickVisible[nBrick] = bVisible;
//std::cout << "Block::assignShape  nBrick=" << nBrick << "  nPosX=" << nPosX << "  nPosY=" << nPosY << "  bVisible=" << bVisible << '\n';
	}
	for (int32_t nBrick = nAssShapeBricks; nBrick < m_nTotBricks; ++nBrick) {
		oShape.m_aBrickPosX[nBrick] = 0;
		oShape.m_aBrickPosY[nBrick] = 0;
		oShape.m_aBrickVisible[nBrick] = false;
	}
}
Block::Block(int32_t nTotBricks, const std::vector<Tile>& aBrick
			, int32_t nTotShapes
			, const std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos) noexcept
: m_nTotBricks(nTotBricks)
, m_nTotShapes(nTotShapes)
, m_nMaxWidth(-1)
, m_nMaxHeight(-1)
, m_nWidestShapeId(-1)
, m_nHighestShapeId(-1)
{
	assert(nTotBricks > 0);
	assert(nTotShapes > 0);

	assignBricks(aBrick);

	m_aIsShapeId.resize(nTotShapes, true);
	m_aShapeIdIterator.resize(nTotShapes);
	m_aShape.resize(nTotShapes);
	const int32_t nAssShapes = std::min(static_cast<int32_t>(aShapeBrickPos.size()), nTotShapes);
	for (int32_t nShape = 0; nShape < nAssShapes; ++nShape) {
		auto itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShape);
		m_aShapeIdIterator[nShape] = itShapeId;
//std::cout << "Block::Block  nShape=" << nShape << '\n';
		Shape& oShape = m_aShape[nShape];
		assignShape(oShape, aShapeBrickPos[nShape]);
	}
	for (int32_t nShape = nAssShapes; nShape < nTotShapes; ++nShape) {
		auto itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShape);
		m_aShapeIdIterator[nShape] = itShapeId;
		Shape& oShape = m_aShape[nShape];
		oShape.m_aBrickPosX.resize(nTotBricks);
		oShape.m_aBrickPosY.resize(nTotBricks);
		oShape.m_aBrickVisible.resize(nTotBricks);
		for (int32_t nBrick = 0; nBrick < nTotBricks; ++nBrick) {
			oShape.m_aBrickPosX[nBrick] = 0;
			oShape.m_aBrickPosY[nBrick] = 0;
			oShape.m_aBrickVisible[nBrick] = false;
		}
	}
	for (int32_t nShape = 0; nShape < m_nTotShapes; ++nShape) {
		calcExtraInfo(nShape);
	}
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
}
void Block::init3Shapes(int32_t nWH) noexcept
{
	assert(m_nTotShapes == 4);

	for (int32_t nShapeId = 1; nShapeId < m_nTotShapes; ++nShapeId) {
		assert(isShapeId(nShapeId));
		Shape& oShape = m_aShape[nShapeId];
		assert(oShape.m_oPosOfVisibleBrick.empty());
		Shape& oPrecShape = m_aShape[nShapeId - 1];
		for (int32_t nBrickId = 0; nBrickId < m_nTotBricks; ++nBrickId) {
			assert(isBrickId(nBrickId));
			// rotate 90 degrees!
			const int32_t nCurPosX = oPrecShape.m_aBrickPosX[nBrickId];
			const int32_t nCurPosY = oPrecShape.m_aBrickPosY[nBrickId];
			const bool bVisible = oPrecShape.m_aBrickVisible[nBrickId];
			const int32_t nPosX = (nWH - 1) - nCurPosY;
			const int32_t nPosY = nCurPosX;
			oShape.m_aBrickPosX[nBrickId] = nPosX;
			oShape.m_aBrickPosY[nBrickId] = nPosY;
			oShape.m_aBrickVisible[nBrickId] = bVisible;
			if (bVisible) {
				const int64_t nXY = Util::packPointToInt64(NPoint{nPosX, nPosY});
				#ifndef NDEBUG
				auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
				#endif //NDEBUG
				assert(itFind == oShape.m_oPosOfVisibleBrick.end());
				// Add to unique positions of visible bricks
				oShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickId));
			}
		}
		calcExtraInfo(nShapeId);
	}
}

Block::Block(int32_t nTotBricks, const std::vector<Tile>& aBrick
			, const std::vector< std::tuple<bool, int32_t, int32_t> >& aBrickPos
			, int32_t nWH) noexcept
: m_nTotBricks(nTotBricks)
, m_nTotShapes(4)
, m_nMaxWidth(-1)
, m_nMaxHeight(-1)
, m_nWidestShapeId(-1)
, m_nHighestShapeId(-1)
{
	assert(nTotBricks >= 0);

	assignBricks(aBrick);

	m_aIsShapeId.resize(m_nTotShapes, true);
	m_aShapeIdIterator.resize(m_nTotShapes);
	m_aShape.resize(m_nTotShapes);
//std::cout << "Block::Block rotate   itShapeId is valid=" << (isIteratorOfListShapeIds(itShapeId) ? "true" : "FALSE") << "   " << *itShapeId << '\n';
	{
		const int32_t nShape = 0;
		auto itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShape);
		m_aShapeIdIterator[nShape] = itShapeId;
		Shape& oShape = m_aShape[0];
		assignShape(oShape, aBrickPos);
		calcExtraInfo(0);
	}
	for (int32_t nShape = 1; nShape < m_nTotShapes; ++nShape) {
		auto itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShape);
		m_aShapeIdIterator[nShape] = itShapeId;
//std::cout << "Block::Block rotate   itShapeId is valid=" << (isIteratorOfListShapeIds(itShapeId) ? "true" : "FALSE") << "   " << *itShapeId << '\n';
		Shape& oShape = m_aShape[nShape];
		oShape.m_aBrickPosX.resize(nTotBricks);
		oShape.m_aBrickPosY.resize(nTotBricks);
		oShape.m_aBrickVisible.resize(nTotBricks);
	}
	init3Shapes(nWH);
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
}
Block::Block(int32_t nFirstShapeId, const Block& oFirst, int32_t nSecondShapeId, const Block& oSecond
			, int32_t nRelX, int32_t nRelY
			, int32_t& nFirstPosXDelta, int32_t& nFirstPosYDelta
			, std::unordered_map<int32_t, int32_t>& oFirstBrickIds, std::unordered_map<int32_t, int32_t>& oSecondBrickIds) noexcept
: m_nTotBricks(-1)
, m_nTotShapes(4)
, m_nMaxWidth(-1)
, m_nMaxHeight(-1)
, m_nWidestShapeId(-1)
, m_nHighestShapeId(-1)
{
	//  |012345678901234|  |012345678901234|
	//  |               |  |               |
	//  |               |  |               |
	//  |      .....    |  |               |
	//  |      .....    |  |               |
	//  |      ...=.    |  |       ,.=.,   |
	//  |      ...=.    |  |       ,.=.,   |
	//  |      .;;==    |  |       ,.==,   |
	//  |       ,**,    |  |       ,==.,   |
	//  |       ,**,    |  |       ,==.,   |
	//  |       ,,,,    |  |               |
	//  |               |  |               |
	//
	//  Shapes before fusion:
	//    .  empty space of first
	//    ,  empty space of second
	//    ;  overlap empty space
	//    =  first block
	//    *  second block
	//
	//  Result shape after fusion:
	//    =  result block
	//    .  empty space
	//    ,  extra empty space for rotation (to make it a square)
	//
	//  nFirstPosXDelta == 2, nFirstPosYDelta == 1

	assert(oFirst.isShapeId(nFirstShapeId));
	assert(oSecond.isShapeId(nSecondShapeId));
	const Shape& oFirstShape = oFirst.m_aShape[nFirstShapeId];
	const Shape& oSecondShape = oSecond.m_aShape[nSecondShapeId];

	const int32_t nFirstVisibles = static_cast<int32_t>(oFirstShape.m_oPosOfVisibleBrick.size());
	const int32_t nSecondVisibles = static_cast<int32_t>(oSecondShape.m_oPosOfVisibleBrick.size());

	m_nTotBricks = nFirstVisibles + nSecondVisibles;

	m_aBrickId.resize(m_nTotBricks);
	m_aBrickIdx.resize(m_nTotBricks);
	m_aTile.resize(m_nTotBricks);

	m_aIsShapeId.resize(m_nTotShapes, true);
	m_aShapeIdIterator.resize(m_nTotShapes);
	m_aShape.resize(m_nTotShapes);
	for (int32_t nShape = 0; nShape < m_nTotShapes; ++nShape) {
		auto itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShape);
		m_aShapeIdIterator[nShape] = itShapeId;
//std::cout << "Block::Block fusion   itShapeId is valid=" << (isIteratorOfListShapeIds(itShapeId) ? "true" : "FALSE") << "   " << *itShapeId << '\n';
	}

	Shape& oResShape = m_aShape[0];
	oResShape.m_aBrickPosX.reserve(m_nTotBricks);
	oResShape.m_aBrickPosY.reserve(m_nTotBricks);
	oResShape.m_aBrickVisible.reserve(m_nTotBricks);

	// baricenter
	int32_t nBx = 0;
	int32_t nBy = 0;

	{
	int32_t nBrickIdx = 0;
	for (auto& oBrickPos : oFirstShape.m_oPosOfVisibleBrick) {
		m_aBrickId[nBrickIdx] = nBrickIdx;
		m_aBrickIdx[nBrickIdx] = nBrickIdx;
		const int32_t nFirstBrickId = oBrickPos.second;
		oFirstBrickIds.insert(std::make_pair(nFirstBrickId, nBrickIdx));
		m_aTile[nBrickIdx] = oFirst.m_aTile[nFirstBrickId];
		const int64_t nXY = oBrickPos.first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nX = oXY.m_nX;
		const int32_t nY = oXY.m_nY;
		assert(oResShape.m_oPosOfVisibleBrick.find(nXY) == oResShape.m_oPosOfVisibleBrick.end());
		oResShape.m_aBrickPosX.push_back(nX);
		oResShape.m_aBrickPosY.push_back(nY);
		oResShape.m_aBrickVisible.push_back(true);
		oResShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickIdx));
		nBx += nX;
		nBy += nY;

		++nBrickIdx;
	}
	for (auto& oBrickPos : oSecondShape.m_oPosOfVisibleBrick) {
		m_aBrickId[nBrickIdx] = nBrickIdx;
		m_aBrickIdx[nBrickIdx] = nBrickIdx;
		const int32_t nSecondBrickId = oBrickPos.second;
		oSecondBrickIds.insert(std::make_pair(nSecondBrickId, nBrickIdx));
		m_aTile[nBrickIdx] = oSecond.m_aTile[nSecondBrickId];
		const int64_t nSecondXY = oBrickPos.first;
		const NPoint oXY = Util::unpackPointFromInt64(nSecondXY);
		const int32_t nX = oXY.m_nX + nRelX;
		const int32_t nY = oXY.m_nY + nRelY;
		const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
		const bool bPosFree = (oResShape.m_oPosOfVisibleBrick.find(nXY) == oResShape.m_oPosOfVisibleBrick.end());
		oResShape.m_aBrickPosX.push_back(nX);
		oResShape.m_aBrickPosY.push_back(nY);
		oResShape.m_aBrickVisible.push_back(bPosFree);
		if (bPosFree) {
			oResShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickIdx));
			nBx += nX;
			nBy += nY;
		}

		++nBrickIdx;
	}
	assert(nBrickIdx == m_nTotBricks);
	}
	calcExtraInfo(0);

	const int32_t nTotVisibleBricks = static_cast<int32_t>(oResShape.m_oPosOfVisibleBrick.size());

	const int32_t nCorr = (nTotVisibleBricks - 1) / 2;

	nBx = (nBx + nCorr) / nTotVisibleBricks - oResShape.m_nMinVisiblePosX;
	nBy = (nBy + nCorr) / nTotVisibleBricks - oResShape.m_nMinVisiblePosY;

	const int32_t nResW = oResShape.m_nMaxVisiblePosX - oResShape.m_nMinVisiblePosX + 1;
	const int32_t nResH = oResShape.m_nMaxVisiblePosY - oResShape.m_nMinVisiblePosY + 1;

	const int32_t nWH = std::max(nResW, nResH);
	const int32_t nFreeW = nWH - nResW;
	const int32_t nFreeH = nWH - nResH;

	// geometric center
	const int32_t nFx = (nResW - 1) / 2;
	const int32_t nFy = (nResH - 1) / 2;

	const int32_t nDeltaX = nBx - nFx;
	const int32_t nDeltaY = nBy - nFy;

	int32_t nBaseX = nFreeW / 2;
	int32_t nBaseY = nFreeH / 2;

	const int32_t nFreeCols = nFreeW - nBaseX * 2;
	const int32_t nFreeRows = nFreeH - nBaseY * 2;

	if ((nFreeCols > 0) && (nDeltaX < 0)) {
		++nBaseX;
	}

	if ((nFreeRows > 0) && (nDeltaY < 0)) {
		++nBaseY;
	}

	// correction
	const int32_t nCorrX = nBaseX - oResShape.m_nMinVisiblePosX;
	const int32_t nCorrY = nBaseY - oResShape.m_nMinVisiblePosY;
	oResShape.m_oPosOfVisibleBrick.clear();
	for (auto& nBrickId : m_aBrickId) {
		oResShape.m_aBrickPosX[nBrickId] += nCorrX;
		oResShape.m_aBrickPosY[nBrickId] += nCorrY;
		const int32_t nX = oResShape.m_aBrickPosX[nBrickId];
		const int32_t nY = oResShape.m_aBrickPosY[nBrickId];
		const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
		if (oResShape.m_aBrickVisible[nBrickId]) {
			oResShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickId));
		}
	}
	calcExtraInfo(0);

	nFirstPosXDelta = - nCorrX;
	nFirstPosYDelta = - nCorrY;

	for (int32_t nShapeIdx = 1; nShapeIdx < m_nTotShapes; ++nShapeIdx) {
		Shape& oRotShape = m_aShape[nShapeIdx];
		oRotShape.m_aBrickPosX.resize(m_nTotBricks);
		oRotShape.m_aBrickPosY.resize(m_nTotBricks);
		oRotShape.m_aBrickVisible.resize(m_nTotBricks);
	}
	init3Shapes(nWH);
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
}
/* generated automatically from fusion of two shapes
 */
/*
void Block::reInitFusion(int32_t nFirstShape, const Block& oFirst
			, int32_t nSecondShape, const Block& oSecond
			, int32_t nRelX, int32_t nRelY
			, int32_t& nFirstPosXDelta, int32_t& nFirstPosYDelta)
{
	//  |012345678901234|  |012345678901234|
	//  |               |  |               |
	//  |               |  |               |
	//  |      .....    |  |               |
	//  |      .....    |  |               |
	//  |      ...=.    |  |       ,.=.,   |
	//  |      ...=.    |  |       ,.=.,   |
	//  |      .;;==    |  |       ,.==,   |
	//  |       ,**,    |  |       ,==.,   |
	//  |       ,**,    |  |       ,==.,   |
	//  |       ,,,,    |  |               |
	//  |               |  |               |
	//  |               |  |               |
	//  |               |  |               |
	//  |               |  |               |
	//  |               |  |               |
	//  |               |  |               |
	//

	assert(oFirst.isValid());
	assert(oSecond.isValid());
	assert(&oFirst != this);
	assert(&oSecond != this);
	assert(&oSecond != &oFirst);

	const int32_t nFirstTotBricks = oFirst.m_nTotBricks;
	const int32_t nSecondTotBricks = oSecond.m_nTotBricks;
	const int32_t nMaxBricks = nFirstTotBricks + nSecondTotBricks;

	const int32_t nMinX = std::min(oFirst.m_aMinPosX[nFirstShape]
				, oSecond.m_aMinPosX[nSecondShape] + nRelX);
	const int32_t nMinY = std::min(oFirst.m_aMinPosY[nFirstShape]
				, oSecond.m_aMinPosY[nSecondShape] + nRelY);

	const int32_t nMaxX = std::max(oFirst.m_aMaxPosX[nFirstShape]
				, oSecond.m_aMaxPosX[nSecondShape] + nRelX);
	const int32_t nMaxY = std::max(oFirst.m_aMaxPosY[nFirstShape]
				, oSecond.m_aMaxPosY[nSecondShape] + nRelY);

	const int32_t nTheW = nMaxX - nMinX + 1;
	const int32_t nTheH = nMaxY - nMinY + 1;

	const int32_t nWH = std::max(nTheW, nTheH);
	const int32_t nFreeW = nWH - nTheW;
	const int32_t nFreeH = nWH - nTheH;

	m_nTotBricks = nMaxBricks;
	m_aBrick.resize(m_nTotBricks);
	m_nTotShapes = 4;
	m_aPosX.resize(m_nTotBricks * m_nTotShapes);
	m_aPosY.resize(m_nTotBricks * m_nTotShapes);
	m_aHidden.resize(m_nTotBricks * m_nTotShapes);

	// baricenter
	int32_t nBx = 0;
	int32_t nBy = 0;

	int32_t nTotBricks = 0;

	const int32_t nFirstShapeBase = nFirstShape * oFirst.m_nTotBricks;
	for (int32_t nFirstBrick = 0; nFirstBrick < nFirstTotBricks; ++nFirstBrick) {
		const int32_t nFirstIdx = nFirstShapeBase + nFirstBrick;
		if (!oFirst.m_aHidden[nFirstIdx]) {
			m_aBrick[nTotBricks] = oFirst.m_aBrick[nFirstBrick];
			m_aPosX[nTotBricks] = oFirst.m_aPosX[nFirstIdx] - nMinX;
			m_aPosY[nTotBricks] = oFirst.m_aPosY[nFirstIdx] - nMinY;
			m_aHidden[nTotBricks] = false;

			nBx += m_aPosX[nTotBricks];
			nBy += m_aPosY[nTotBricks];
			++nTotBricks;
		}
	}

	const int32_t nSecondShapeBase = nSecondShape * oSecond.m_nTotBricks;
	for (int32_t nSecondBrick = 0; nSecondBrick < nSecondTotBricks; ++nSecondBrick) {
		const int32_t nSecondIdx = nSecondShapeBase + nSecondBrick;
		if (!oSecond.m_aHidden[nSecondIdx]) {
			m_aBrick[nTotBricks] = oSecond.m_aBrick[nSecondBrick];
			m_aPosX[nTotBricks] = oSecond.m_aPosX[nSecondIdx] - nMinX + nRelX;
			m_aPosY[nTotBricks] = oSecond.m_aPosY[nSecondIdx] - nMinY + nRelY;
			m_aHidden[nTotBricks] = false;

			nBx += m_aPosX[nTotBricks];
			nBy += m_aPosY[nTotBricks];
			++nTotBricks;
		}
	}

	// geometric center
	const int32_t nFx = (nTheW - 1) / 2;
	const int32_t nFy = (nTheH - 1) / 2;

	const int32_t nCorr = (nTotBricks - 1) / 2;

	nBx = (nBx + nCorr) / nTotBricks;
	nBy = (nBy + nCorr) / nTotBricks;

	const int32_t nDeltaX = nBx - nFx;
	const int32_t nDeltaY = nBy - nFy;

	int32_t nBaseX = nFreeW / 2;
	int32_t nBaseY = nFreeH / 2;

	const int32_t nFreeCols = nFreeW - nBaseX * 2;
	const int32_t nFreeRows = nFreeH - nBaseY * 2;

	if ((nFreeCols > 0) && (nDeltaX < 0))
	{
		++nBaseX;
	}

	if ((nFreeRows > 0) && (nDeltaY < 0))
	{
		++nBaseY;
	}

	// correction
	for (int32_t nIdx = 0; nIdx < nMaxBricks; ++nIdx) {
		m_aPosX[nIdx] += nBaseX;
		m_aPosY[nIdx] += nBaseY;
	}

	nFirstPosXDelta = nMinX - nBaseX;
	nFirstPosYDelta = nMinY - nBaseY;

	m_nTotBricks = nTotBricks;
	init3Shapes(nWH);
}
*/
Block::Block(const Block& oSource) noexcept
: m_nTotBricks(oSource.m_nTotBricks)
, m_nTotShapes(oSource.m_nTotShapes)
, m_aBrickId(oSource.m_aBrickId)
, m_aBrickIdx(oSource.m_aBrickIdx)
, m_aFreeBrickIds(oSource.m_aFreeBrickIds)
, m_aTile(oSource.m_aTile)
, m_oShapeIds(oSource.m_oShapeIds)
, m_aIsShapeId(oSource.m_aIsShapeId)
, m_aFreeShapeId(oSource.m_aFreeShapeId)
, m_aShape(oSource.m_aShape)
, m_nMaxWidth(oSource.m_nMaxWidth)
, m_nMaxHeight(oSource.m_nMaxHeight)
, m_nWidestShapeId(oSource.m_nWidestShapeId)
, m_nHighestShapeId(oSource.m_nHighestShapeId)
{
	m_aShapeIdIterator.resize(oSource.m_aShapeIdIterator.size());
	for (auto itShapeId = m_oShapeIds.begin(); itShapeId != m_oShapeIds.end(); ++itShapeId) {
		const int32_t nShapeId = *itShapeId;
		m_aShapeIdIterator[nShapeId] = itShapeId;
	}
}
Block& Block::operator=(const Block& oSource) noexcept
{
	m_nTotBricks = oSource.m_nTotBricks;
	m_nTotShapes = oSource.m_nTotShapes;
	m_aBrickId = oSource.m_aBrickId;
	m_aBrickIdx = oSource.m_aBrickIdx;
	m_aFreeBrickIds = oSource.m_aFreeBrickIds;
	m_aTile = oSource.m_aTile;
	m_oShapeIds = oSource.m_oShapeIds;
	m_aIsShapeId = oSource.m_aIsShapeId;
	m_aFreeShapeId = oSource.m_aFreeShapeId;
	m_aShape = oSource.m_aShape;
	m_nMaxWidth = oSource.m_nMaxWidth;
	m_nMaxHeight = oSource.m_nMaxHeight;
	m_nWidestShapeId = oSource.m_nWidestShapeId;
	m_nHighestShapeId = oSource.m_nHighestShapeId;
	m_aShapeIdIterator.resize(oSource.m_aShapeIdIterator.size());
	for (auto itShapeId = m_oShapeIds.begin(); itShapeId != m_oShapeIds.end(); ++itShapeId) {
		const int32_t nShapeId = *itShapeId;
		m_aShapeIdIterator[nShapeId] = itShapeId;
	}
	return *this;
}
void Block::swap(Block& oOtherBlock) noexcept
{
//std::cout << "Block(" << (int64_t)this << ")::swap(" << (int64_t)&oOtherBlock << ")" << '\n';
	std::swap(m_nTotBricks, oOtherBlock.m_nTotBricks);
	std::swap(m_nTotShapes, oOtherBlock.m_nTotShapes);
	m_aBrickId.swap(oOtherBlock.m_aBrickId);
	m_aBrickIdx.swap(oOtherBlock.m_aBrickIdx);
	m_aFreeBrickIds.swap(oOtherBlock.m_aFreeBrickIds);
	m_aTile.swap(oOtherBlock.m_aTile);
	m_oShapeIds.swap(oOtherBlock.m_oShapeIds);
	m_aShapeIdIterator.swap(oOtherBlock.m_aShapeIdIterator);
	m_aIsShapeId.swap(oOtherBlock.m_aIsShapeId);
	m_aFreeShapeId.swap(oOtherBlock.m_aFreeShapeId);
	m_aShape.swap(oOtherBlock.m_aShape);
	std::swap(m_nMaxWidth, oOtherBlock.m_nMaxWidth);
	std::swap(m_nMaxHeight, oOtherBlock.m_nMaxHeight);
	std::swap(m_nWidestShapeId, oOtherBlock.m_nWidestShapeId);
	std::swap(m_nHighestShapeId, oOtherBlock.m_nHighestShapeId);
}
int32_t Block::brickAdd(const Tile& oTile, int32_t nX, int32_t nY, bool bVisible) noexcept
{
	if (m_oShapeIds.empty()) {
		shapeInsert(-1);
	}
	int32_t nBrickId;
	if (m_aFreeBrickIds.empty()) {
		const int32_t nTotBrickCapacity = static_cast<int32_t>(m_aBrickIdx.size());
		assert(nTotBrickCapacity == m_nTotBricks); // full
		nBrickId = nTotBrickCapacity;
		m_aBrickId.push_back(nBrickId);
		m_aBrickIdx.push_back(nTotBrickCapacity);
		m_aTile.push_back(oTile);
		for (auto& nShapeId : m_oShapeIds) {
			Shape& oShape = m_aShape[nShapeId];
			oShape.m_aBrickPosX.resize(nTotBrickCapacity + 1);
			oShape.m_aBrickPosY.resize(nTotBrickCapacity + 1);
			oShape.m_aBrickVisible.resize(nTotBrickCapacity + 1);
		}
	} else {
		nBrickId = m_aFreeBrickIds.back();
		m_aFreeBrickIds.pop_back();
		const int32_t nBrickIdx = static_cast<int32_t>(m_aBrickId.size());
		m_aBrickId.push_back(nBrickId);
		m_aBrickIdx[nBrickId] = nBrickIdx;
		m_aTile[nBrickId] = oTile;
	}
	const bool bEmptyTile = oTile.isEmpty();
	m_aTile[nBrickId] = (bEmptyTile ? s_oNonEmptyTile : oTile);
	++m_nTotBricks;
	for (auto& nShapeId : m_oShapeIds) {
		Shape& oShape = m_aShape[nShapeId];
		oShape.m_aBrickPosX[nBrickId] = nX;
		oShape.m_aBrickPosY[nBrickId] = nY;
		if (bVisible) {
			const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
			auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
			if (itFind != oShape.m_oPosOfVisibleBrick.end()) {
				#ifndef NDEBUG
				const int32_t nOccupierBrickId = itFind->second;
				#endif //NDEBUG
//std::cout << "Block::brickAdd  nShapeId=" << nShapeId << "  nBrickId=" << nBrickId << "   nX=" << nX << "  nY=" << nY << "  already occupied: nOccupierId=" << nOccupierBrickId << '\n';
				assert(shapeBrickVisible(nShapeId, nOccupierBrickId));
				// already occupied in this shape
				bVisible = false;
			} else {
				oShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickId));
//std::cout << "Block::brickAdd  nShapeId=" << nShapeId << "  nBrickId=" << nBrickId << "   nX=" << nX << "  nY=" << nY << "  inserted visible" << '\n';
			}
		}
		oShape.m_aBrickVisible[nBrickId] = bVisible;
		calcExtraInfo(nShapeId);
	}
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
	return nBrickId;
}
void Block::brickModify(int32_t nBrickId, const Tile& oTile) noexcept
{
	assert(isBrickId(nBrickId));
	const bool bEmptyTile = oTile.isEmpty();
	m_aTile[nBrickId] = (bEmptyTile ? s_oNonEmptyTile : oTile);
}
void Block::brickRemove(int32_t nBrickId) noexcept
{
//std::cout << "Block::brickRemove  nBrickId=" << nBrickId << '\n';
	// remove 7
	//                 0   1   2   3   4   5   6   7
	//               ---------------------------------
	// m_aBrickId    | 1 | 7 | 4 | 5 |
	// m_aBrickIdx   |-1 | 0 |-1 |-1 | 2 | 3 |-1 | 1 |
	// m_aFreeBrickIds 0,2,3,6
	//
	// m_aBrickId    | 1 | 5 | 4 |
	// m_aBrickIdx   |-1 | 0 |-1 |-1 | 2 | 1 |-1 |-1 |
	// m_aFreeBrickIds 0,2,3,6,7
	assert(isBrickId(nBrickId));
	const int32_t nBrickIdx = m_aBrickIdx[nBrickId];
	m_aBrickIdx[nBrickId] = -1;
	const int32_t nMoveBrickId = m_aBrickId[m_nTotBricks - 1];
	m_aBrickId[nBrickIdx] = nMoveBrickId;
	m_aBrickId.pop_back();
	m_aBrickIdx[nMoveBrickId] = nBrickIdx;
	m_aFreeBrickIds.push_back(nBrickId);
	{
		// some cleanup
		//m_aTile[nBrickId].clear();
	}
	--m_nTotBricks;
	assert(m_nTotBricks >= 0);
	for (auto& nShapeId : m_oShapeIds) {
		Shape& oShape = m_aShape[nShapeId];
		if (oShape.m_aBrickVisible[nBrickId]) {
			const int32_t nX = oShape.m_aBrickPosX[nBrickId];
			const int32_t nY = oShape.m_aBrickPosY[nBrickId];
			const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
			auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
			assert(itFind != oShape.m_oPosOfVisibleBrick.end());
			oShape.m_oPosOfVisibleBrick.erase(itFind);
		}
		calcExtraInfo(nShapeId);
	}
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
}
int32_t Block::shapeInsert(int32_t nBeforeShapeId) noexcept
{
	return shapeInsert(nBeforeShapeId, -1);
}
int32_t Block::shapeInsert(int32_t nBeforeShapeId, int32_t nCopyOfShapeId) noexcept
{
	assert((nBeforeShapeId == -1) || (isShapeId(nBeforeShapeId)));
	assert((nCopyOfShapeId == -1) || (isShapeId(nCopyOfShapeId)));
	int32_t nShapeId;
	if (m_aFreeShapeId.empty()) {
		nShapeId = static_cast<int32_t>(m_aIsShapeId.size());
		assert(nShapeId == m_nTotShapes);
		m_aIsShapeId.push_back(true);
		std::list<int32_t>::iterator itShapeId;
		if (nBeforeShapeId < 0) {
			itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShapeId);
		} else {
			itShapeId = m_oShapeIds.insert(m_aShapeIdIterator[nBeforeShapeId], nShapeId);
		}
		m_aShapeIdIterator.push_back(itShapeId);
		m_aShape.resize(m_nTotShapes + 1);
	} else {
		nShapeId = m_aFreeShapeId.back();
		m_aFreeShapeId.pop_back();
		assert(!m_aIsShapeId[nShapeId]);
		m_aIsShapeId[nShapeId] = true;
		std::list<int32_t>::iterator itShapeId;
		if (nBeforeShapeId < 0) {
			itShapeId = m_oShapeIds.insert(m_oShapeIds.end(), nShapeId);
		} else {
			itShapeId = m_oShapeIds.insert(m_aShapeIdIterator[nBeforeShapeId], nShapeId);
		}
		m_aShapeIdIterator[nShapeId] = itShapeId;
	}
	++m_nTotShapes;
	Shape& oShape = m_aShape[nShapeId];
	if (nCopyOfShapeId < 0) {
		const int32_t nBrickIdCapacity = static_cast<int32_t>(m_aBrickIdx.size());
		oShape.m_aBrickPosX.resize(nBrickIdCapacity);
		oShape.m_aBrickPosY.resize(nBrickIdCapacity);
		oShape.m_aBrickVisible.resize(nBrickIdCapacity);
		for (auto& nBrickId : m_aBrickId) {
			oShape.m_aBrickPosX[nBrickId] = 0;
			oShape.m_aBrickPosY[nBrickId] = 0;
			oShape.m_aBrickVisible[nBrickId] = false;
		}
		calcExtraInfo(nShapeId);
	} else {
		oShape = m_aShape[nCopyOfShapeId];
	}
	//m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	//m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
	return nShapeId;
}
void Block::shapeRemove(int32_t nShapeId) noexcept
{
	assert(isShapeId(nShapeId));
	auto& itShapeId = m_aShapeIdIterator[nShapeId];
	shapeRemove(nShapeId, itShapeId);
}
void Block::shapeRemove(int32_t nShapeId, std::list<int32_t>::iterator& itShapeId) noexcept
{
	m_aIsShapeId[nShapeId] = false;
	itShapeId = m_oShapeIds.erase(itShapeId);
	m_aFreeShapeId.push_back(nShapeId);
	{
		// some cleanup
		Shape& oShape = m_aShape[nShapeId];
		oShape.m_oPosOfVisibleBrick.clear();
		for (int32_t nDir = 0; nDir < 3; ++nDir) {
			oShape.m_aDirContactPositions[nDir].clear();
		}
	}
	--m_nTotShapes;
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
}
int32_t Block::shapeRemoveAllInvisible() noexcept
{
	int32_t nRemovedShapes = 0;
	auto itShapeId = m_oShapeIds.begin();
	while (itShapeId != m_oShapeIds.end()) {
		const int32_t nShapeId = *itShapeId;
		if (shapeTotVisibleBricks(nShapeId) == 0) {
			shapeRemove(nShapeId, itShapeId);
			++nRemovedShapes;
		} else {
			++itShapeId;
		}
	}
	return nRemovedShapes;
}
bool Block::shapeBrickSetPosVisible(int32_t nShapeId, int32_t nBrickId, int32_t nX, int32_t nY, bool bVisible) noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	Shape& oShape = m_aShape[nShapeId];
	const int32_t nOldX = oShape.m_aBrickPosX[nBrickId];
	const int32_t nOldY = oShape.m_aBrickPosY[nBrickId];
	const bool bOldVisible = oShape.m_aBrickVisible[nBrickId];
	if (bOldVisible) {
		const int64_t nOldXY = Util::packPointToInt64(NPoint{nOldX, nOldY});
		auto itFind = oShape.m_oPosOfVisibleBrick.find(nOldXY);
		assert(itFind != oShape.m_oPosOfVisibleBrick.end());
		oShape.m_oPosOfVisibleBrick.erase(itFind);
	}
	oShape.m_aBrickPosX[nBrickId] = nX;
	oShape.m_aBrickPosY[nBrickId] = nY;
	oShape.m_aBrickVisible[nBrickId] = false;
	return shapeBrickSetVisible(nShapeId, nBrickId, bVisible);
}
bool Block::shapeBrickSetVisible(int32_t nShapeId, int32_t nBrickId, bool bVisible) noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	Shape& oShape = m_aShape[nShapeId];
	const bool bWasVilsible = oShape.m_aBrickVisible[nBrickId];
	if (bWasVilsible == bVisible) {
		return true;
	}
	const int32_t nX = oShape.m_aBrickPosX[nBrickId];
	const int32_t nY = oShape.m_aBrickPosY[nBrickId];
	const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
	auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
	if (bWasVilsible) {
		assert(itFind != oShape.m_oPosOfVisibleBrick.end());
		assert(itFind->second == nBrickId);
		oShape.m_oPosOfVisibleBrick.erase(itFind);
	} else {
		if (itFind != oShape.m_oPosOfVisibleBrick.end()) {
			// already occupied
			return false;
		}
		oShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickId));
	}
	oShape.m_aBrickVisible[nBrickId] = bVisible;
	//TODO this could be done way more efficiently
	calcExtraInfo(nShapeId);
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
	return true;
}
bool Block::shapeModifyBricks(int32_t nShapeId, const std::vector< std::tuple<int32_t, int32_t, int32_t, bool> >& aBrickPosition) noexcept
{
	assert(isShapeId(nShapeId));
	Shape& oShape = m_aShape[nShapeId];
	for (auto& oBrickPos : aBrickPosition) {
		int32_t nBrickId, nX, nY;
		bool bVisible;
		std::tie(nBrickId, nX, nY, bVisible) = oBrickPos;
		assert(isBrickId(nBrickId));
		const bool bOldVisible = oShape.m_aBrickVisible[nBrickId];
		if (bOldVisible) {
			const int32_t nOldX = oShape.m_aBrickPosX[nBrickId];
			const int32_t nOldY = oShape.m_aBrickPosX[nBrickId];
			const int64_t nOldXY = Util::packPointToInt64(NPoint{nOldX, nOldY});
			auto itFind = oShape.m_oPosOfVisibleBrick.find(nOldXY);
			assert(itFind != oShape.m_oPosOfVisibleBrick.end());
			assert(itFind->second == nBrickId);
			oShape.m_oPosOfVisibleBrick.erase(itFind);

			oShape.m_aBrickVisible[nBrickId] = false;
		}
		oShape.m_aBrickPosX[nBrickId] = nX;
		oShape.m_aBrickPosY[nBrickId] = nY;
	}
	bool bCanAllToVisible = true;
	for (auto& oBrickPos : aBrickPosition) {
		int32_t nBrickId, nX, nY;
		bool bVisible;
		std::tie(nBrickId, nX, nY, bVisible) = oBrickPos;
		if (bVisible) {
//std::cout << "Block::shapeModifyBricks  nBrickId=" << nBrickId << "  nX=" << nX << "  nY=" << nY << "  bVisible=" << (bVisible ? "true" : "false") << '\n';
			const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
			auto itFind = oShape.m_oPosOfVisibleBrick.find(nXY);
			if (itFind != oShape.m_oPosOfVisibleBrick.end()) {
				bCanAllToVisible = false;
			} else {
				oShape.m_aBrickVisible[nBrickId] = true;
				oShape.m_oPosOfVisibleBrick.insert(std::make_pair(nXY, nBrickId));
			}
		}
	}
	calcExtraInfo(nShapeId);
	m_nWidestShapeId = calcWidestShape(m_nMaxWidth);
	m_nHighestShapeId = calcHighestShape(m_nMaxHeight);
	return bCanAllToVisible;
}
int32_t Block::shapeFirst() const noexcept
{
	if (m_nTotShapes <= 0) {
		return -1;
	}
	const int32_t nShapeId = *m_oShapeIds.begin();
	return nShapeId;
}
int32_t Block::shapeLast() const noexcept
{
	if (m_nTotShapes <= 0) {
		return -1;
	}
	std::list<int32_t>::const_iterator itShapeId = m_oShapeIds.end();
	--itShapeId;
	const int32_t nShapeId = *itShapeId;
	return nShapeId;
}
#ifndef NDEBUG
bool Block::isIteratorOfListShapeIds(const std::list<int32_t>::const_iterator& itShapeId) const noexcept
{
	for (std::list<int32_t>::const_iterator itTest = m_oShapeIds.cbegin(); itTest != m_oShapeIds.cend(); ++itTest) {
		if (itTest == itShapeId) {
			return true;
		}
	}
	return false;
}
bool Block::isIteratorOfListShapeIds(const std::list<int32_t>::iterator& itShapeId) const noexcept
{
	for (std::list<int32_t>::const_iterator itTest = m_oShapeIds.cbegin(); itTest != m_oShapeIds.cend(); ++itTest) {
		if (itTest == itShapeId) {
			return true;
		}
	}
	return false;
}
#endif //NDEBUG
int32_t Block::shapePrec(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	std::list<int32_t>::const_iterator itShapeId = m_aShapeIdIterator[nShapeId];
	if (itShapeId == m_oShapeIds.begin()) {
		return -1;
	}
	--itShapeId;
	const int32_t nPrecShapeId = *itShapeId;
	return nPrecShapeId;
}
int32_t Block::shapeNext(int32_t nShapeId) const noexcept
{
//std::cout << "Block(" << (int64_t)this << ")::shapeNext    nShapeId=" << nShapeId << "  m_nTotShapes=" << m_nTotShapes << '\n';
	assert(isShapeId(nShapeId));
	std::list<int32_t>::const_iterator itShapeId = m_aShapeIdIterator[nShapeId];
//std::cout << "       shapeNext    itShapeId is valid=" << (isIteratorOfListShapeIds(itShapeId) ? "true" : "FALSE") << "   " << *itShapeId << '\n';
	++itShapeId;
	if (itShapeId == m_oShapeIds.cend()) {
		return -1;
	}
	const int32_t nNextShapeId = *itShapeId;
	return nNextShapeId;
}

bool Block::isBrickId(int32_t nBrickId) const noexcept
{
	if ((nBrickId < 0) || (nBrickId >= static_cast<int32_t>(m_aBrickIdx.size()))) {
		return false;
	}
	return (m_aBrickIdx[nBrickId] >= 0);
}
bool Block::isShapeId(int32_t nShapeId) const noexcept
{
	if ((nShapeId < 0) || (nShapeId >= static_cast<int32_t>(m_aIsShapeId.size()))) {
		return false;
	}
	return m_aIsShapeId[nShapeId];
}
const std::vector<int32_t> Block::shapeIdsAsVector() const noexcept
{
	std::vector<int32_t> aShapeId;
	aShapeId.reserve(m_oShapeIds.size());
	for (auto& nShapeId : m_oShapeIds) {
		aShapeId.push_back(nShapeId);
	}
	return aShapeId;
}
const Tile& Block::brick(int32_t nBrickId) const noexcept
{
	assert(isBrickId(nBrickId));
	return m_aTile[nBrickId];
}
int32_t Block::shapeBrickPosX(int32_t nShapeId, int32_t nBrickId) const noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_aBrickPosX[nBrickId];
}
int32_t Block::shapeBrickPosY(int32_t nShapeId, int32_t nBrickId) const noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_aBrickPosY[nBrickId];
}
NPoint Block::shapeBrickPos(int32_t nShapeId, int32_t nBrickId) const noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return NPoint{oShape.m_aBrickPosX[nBrickId], oShape.m_aBrickPosY[nBrickId]};
}
bool Block::shapeBrickVisible(int32_t nShapeId, int32_t nBrickId) const noexcept
{
	assert(isBrickId(nBrickId));
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_aBrickVisible[nBrickId];
}
int32_t Block::shapeBrickVisiblePosition(int32_t nShapeId, int32_t nX, int32_t nY) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	const int64_t nXY = Util::packPointToInt64(NPoint{nX, nY});
	auto itBrickPos = oShape.m_oPosOfVisibleBrick.find(nXY);
	if (itBrickPos == oShape.m_oPosOfVisibleBrick.end()) {
		return -1; //-----------------------------------------------------------
	}
	const int32_t nBrickId = itBrickPos->second;
	assert(shapeBrickVisible(nShapeId, nBrickId));
	return nBrickId;
}
int32_t Block::shapeTotVisibleBricks(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return static_cast<int32_t>(oShape.m_oPosOfVisibleBrick.size());
}
	//const std::vector< Block::BrickInfo > Block::shapeGet(int32_t nShapeId) const
	//{
	//	assert(isShapeId(nShapeId));
	//	const Shape& oShape = m_aShape[nShapeId];
	//	std::vector< std::tuple<int32_t, int32_t, int32_t, bool> > aBricks(m_nTotBricks);
	//	int32_t nCurIdx = 0;
	//	for (auto& nBrickId : m_aBrickId) {
	//		const int32_t nX = oShape.m_aBrickPosX[nBrickId];
	//		const int32_t nY = oShape.m_aBrickPosY[nBrickId];
	//		const bool bVisible = oShape.m_aBrickVisible[nBrickId];
	//		aBricks[nCurIdx] = std::make_tuple(nBrickId, nX, nY, bVisible);
	//		++nCurIdx;
	//	}
	//	return aBricks;
	//}
const std::vector< Block::Contact > Block::shapeContacts(int32_t nShapeId, Direction::VALUE eDir) const noexcept
{
//std::cout << "Block::shapeContacts  nShapeId=" << nShapeId << '\n';
//dump();
	assert(isShapeId(nShapeId));
	assert((eDir >= 0) && (eDir <= 3));
	const Shape& oShape = m_aShape[nShapeId];
	const std::unordered_map<int64_t, int32_t>& oBrickContacts = oShape.m_aDirContactPositions[eDir];
	std::vector< Contact > aBrickContacts;
	aBrickContacts.reserve(oBrickContacts.size());
	for (auto& oBrickContact : oBrickContacts) {
		const int64_t nXY = oBrickContact.first;
		Contact oCt;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		oCt.m_nRelX = oXY.m_nX;
		oCt.m_nRelY = oXY.m_nY;
		oCt.m_nBrickId = oBrickContact.second;
		aBrickContacts.push_back(std::move(oCt));
	}
	return aBrickContacts;
}
/*
void Block::reInitHide(const Block& oSource, int32_t nWithoutBrick)
{
	assert(oSource.isValid());
	if (this != &oSource) {
		operator=(oSource);
	}

	for (int32_t nShape = m_nTotShapes - 1; nShape >= 0; --nShape) {
		const int32_t nIdx = nShape * m_nTotBricks + nWithoutBrick;
		int32_t nVisible = m_aTotVisibleBricks[nShape];
		assert(nVisible > 0);
		if (!m_aHidden[nIdx]) {
			m_aHidden[nIdx] = true;
			--nVisible;
		}
		if (nVisible == 0) {
			// remove shape
			--m_nTotShapes;
			m_aPosX.erase(m_aPosX.begin() + nShape * m_nTotBricks
					, m_aPosX.begin() + (nShape + 1) * m_nTotBricks);
			m_aPosY.erase(m_aPosY.begin() + nShape * m_nTotBricks
					, m_aPosY.begin() + (nShape + 1) * m_nTotBricks);
			m_aHidden.erase(m_aHidden.begin() + nShape * m_nTotBricks
					, m_aHidden.begin() + (nShape + 1) * m_nTotBricks);
		}
	}
	assert(m_nTotShapes > 0);
	for (int32_t nShape = 0; nShape < m_nTotShapes; ++nShape) {
		calcExtraInfo(nShape);
	}
	m_nWidestShape = calcWidestShape(m_nMaxWidth);
	m_nHighestShape = calcHighestShape(m_nMaxHeight);
}
*/

void Block::calcExtraInfo(int32_t nShapeId) noexcept
{
	calcMinMaxVisible(nShapeId);
	calcContacts(nShapeId);
}
void Block::calcMinMaxVisible(int32_t nShapeId) noexcept
{
	Shape& oShape = m_aShape[nShapeId];

	int32_t& nMinPosX = oShape.m_nMinVisiblePosX;
	int32_t& nMinPosY = oShape.m_nMinVisiblePosY;
	int32_t& nMaxPosX = oShape.m_nMaxVisiblePosX;
	int32_t& nMaxPosY = oShape.m_nMaxVisiblePosY;

	nMinPosX = std::numeric_limits<int32_t>::max();
	nMinPosY = std::numeric_limits<int32_t>::max();
	nMaxPosX = std::numeric_limits<int32_t>::min();
	nMaxPosY = std::numeric_limits<int32_t>::min();

	const std::unordered_map<int64_t, int32_t>& oBricks = oShape.m_oPosOfVisibleBrick;

	for (auto& oBrickPos : oBricks) {
		const int64_t nXY = oBrickPos.first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nX = oXY.m_nX;
		const int32_t nY = oXY.m_nY;
		if (nX < nMinPosX)
		{
			nMinPosX = nX;
		}
		if (nX > nMaxPosX)
		{
			nMaxPosX = nX;
		}
		if (nY < nMinPosY)
		{
			nMinPosY = nY;
		}
		if (nY > nMaxPosY)
		{
			nMaxPosY = nY;
		}
	}
}

void Block::calcContactsCardinal(int32_t nShapeId, int32_t nDx, int32_t nDy) noexcept
{
//std::cout << "Block::calcContactsCardinal nShapeId=" << nShapeId << "  nDx=" << nDx << "  nDy=" << nDy << '\n';
	Shape& oShape = m_aShape[nShapeId];

	const Direction::VALUE eCardi = Direction::fromDelta(nDx,nDy);
	assert((eCardi >= 0) && (eCardi <= 3));

	std::unordered_map<int64_t, int32_t>& oContacts = oShape.m_aDirContactPositions[eCardi];
	oContacts.clear();
	const std::unordered_map<int64_t, int32_t>& oBricks = oShape.m_oPosOfVisibleBrick;

	for (auto& oBrickPos : oBricks) {
		const int64_t nXY = oBrickPos.first;
		const NPoint oXY = Util::unpackPointFromInt64(nXY);
		const int32_t nX = oXY.m_nX;
		const int32_t nY = oXY.m_nY;
		const int32_t nBrickId = oBrickPos.second;
//std::cout << "                            nX=" << nX << "  nY=" << nY << "  nBrickId=" << nBrickId << '\n';
		const int32_t nContactX = nX + nDx;
		const int32_t nContactY = nY + nDy;
		const int64_t nContactXY = Util::packPointToInt64(NPoint{nContactX, nContactY});
		assert(oContacts.find(nContactXY) == oContacts.end());
		auto itFindPos = oBricks.find(nContactXY);
		if (itFindPos == oBricks.end()) {
			// position not occupied by another brick
			oContacts.insert(std::make_pair(nContactXY, nBrickId));
		}
	}
}
void Block::calcContacts(int32_t nShapeId) noexcept
{
	calcContactsCardinal(nShapeId, -1, 0);
	calcContactsCardinal(nShapeId, +1, 0);
	calcContactsCardinal(nShapeId, 0, -1);
	calcContactsCardinal(nShapeId, 0, +1);
}

int32_t Block::calcWidestShape(int32_t& nMaxW) const noexcept
{
	int32_t nMaxShapeId = -1;

	nMaxW = 0;

	for (auto& nShapeId : m_oShapeIds) {
		const int32_t nShapeW = shapeWidth(nShapeId);
		if (nShapeW > nMaxW)
		{
			nMaxW = nShapeW;
			nMaxShapeId = nShapeId;
		}
	}
	return nMaxShapeId;
}

int32_t Block::calcHighestShape(int32_t& nMaxH) const noexcept
{
	int32_t nMaxShapeId = -1;

	nMaxH = 0;

	for (auto& nShapeId : m_oShapeIds) {
		const int32_t nShapeH = shapeHeight(nShapeId);
		if (nShapeH > nMaxH)
		{
			nMaxH = nShapeH;
			nMaxShapeId = nShapeId;
		}
	}
	return nMaxShapeId;
}

int32_t Block::shapeWidth(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	const bool bHasntVisibleBricks = oShape.m_oPosOfVisibleBrick.empty();
	return (bHasntVisibleBricks ? 0 : oShape.m_nMaxVisiblePosX - oShape.m_nMinVisiblePosX + 1);
}

int32_t Block::shapeHeight(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	const bool bHasntVisibleBricks = oShape.m_oPosOfVisibleBrick.empty();
	return (bHasntVisibleBricks ? 0 : oShape.m_nMaxVisiblePosY - oShape.m_nMinVisiblePosY + 1);
}
NSize Block::shapeSize(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	const bool bHasntVisibleBricks = oShape.m_oPosOfVisibleBrick.empty();
	if (bHasntVisibleBricks) {
		return NSize{0,0};
	}
	return NSize{oShape.m_nMaxVisiblePosX - oShape.m_nMinVisiblePosX + 1, oShape.m_nMaxVisiblePosY - oShape.m_nMinVisiblePosY + 1};
}
int32_t Block::widestShapeId() const noexcept
{
	return m_nWidestShapeId;
}
int32_t Block::highestShapeId() const noexcept
{
	return m_nHighestShapeId;
}

int32_t Block::maxWidth() const noexcept
{
	return m_nMaxWidth;
}
int32_t Block::maxHeight() const noexcept
{
	return m_nMaxHeight;
}

int32_t Block::shapeMinX(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_nMinVisiblePosX;
}
int32_t Block::shapeMinY(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_nMinVisiblePosY;
}
NPoint Block::shapeMinPos(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return NPoint{oShape.m_nMinVisiblePosX, oShape.m_nMinVisiblePosY};
}
int32_t Block::shapeMaxX(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_nMaxVisiblePosX;
}
int32_t Block::shapeMaxY(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return oShape.m_nMaxVisiblePosY;
}
NPoint Block::shapeMaxPos(int32_t nShapeId) const noexcept
{
	assert(isShapeId(nShapeId));
	const Shape& oShape = m_aShape[nShapeId];
	return NPoint{oShape.m_nMaxVisiblePosX, oShape.m_nMaxVisiblePosY};
}

#ifndef NDEBUG
void Block::dump() const noexcept
{
	dump(0);
}
void Block::dump(int32_t nIndent) const noexcept
{
	auto sIndent = std::string(nIndent, ' ');

	std::string aDirString[4];
	aDirString[Direction::UP] = "UP   ";
	aDirString[Direction::DOWN] = "DOWN ";
	aDirString[Direction::LEFT] = "LEFT ";
	aDirString[Direction::RIGHT] = "RIGHT";

	std::cout << sIndent << "Block::dump()  nTotBricks=" << m_nTotBricks << "  aBrickIds=";
	for (auto& nBrickId : m_aBrickId) {
		std::cout << " " << nBrickId;
	}
	std::cout << '\n';
	std::cout << sIndent << "               nTotShapes=" << m_nTotShapes << "  oShapeIds=";
	for (auto& nShapeId : m_oShapeIds) {
		std::cout << " " << nShapeId;
	}
	std::cout << '\n';
	for (auto& nShapeId : m_oShapeIds) {
		const Shape& oShape = m_aShape[nShapeId];
		std::cout << sIndent << "        ShapeId=" << nShapeId << " -------" << '\n';
		std::cout << sIndent << "             BrickPos (X,Y,visible,brickId)=";
		for (auto& nBrickId : m_aBrickId) {
			std::cout << " (" << oShape.m_aBrickPosX[nBrickId] << "," << oShape.m_aBrickPosY[nBrickId] << "," << (oShape.m_aBrickVisible[nBrickId] ? "true" : "false") << "," << nBrickId << ")";
		}
		std::cout << '\n';
		std::cout << sIndent << "             BrickPos only visible (X,Y,brickId)=";
		for (auto& oPos : oShape.m_oPosOfVisibleBrick) {
			const int64_t nXY = oPos.first;
			const int32_t nBrickId = oPos.second;
			const NPoint oXY = Util::unpackPointFromInt64(nXY);
			const int32_t nX = oXY.m_nX;
			const int32_t nY = oXY.m_nY;
			std::cout << " (" << nX << "," << nY << "," << nBrickId << ")";
		}
		std::cout << '\n';
		for (int32_t nDir = 0; nDir < 4; ++nDir) {
			std::cout << sIndent << "             Contacts: " << aDirString[nDir] << " (X,Y,brickId)=";
			for (auto& oPos : oShape.m_aDirContactPositions[nDir]) {
				const int64_t nXY = oPos.first;
				const int32_t nBrickId = oPos.second;
				const NPoint oXY = Util::unpackPointFromInt64(nXY);
				const int32_t nX = oXY.m_nX;
				const int32_t nY = oXY.m_nY;
				std::cout << " (" << nX << "," << nY << "," << nBrickId << ")";
			}
			std::cout << '\n';
		}
	}
}
#endif //NDEBUG

} // namespace stmg
