/*
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this program; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   testBlock.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "block.h"

#include <tuple>

namespace stmg
{

// using std::shared_ptr;

namespace testing
{

TEST_CASE("testBlock, EmptyConstructor")
{
	Block oBlock;
	REQUIRE(oBlock.isEmpty());
	REQUIRE(oBlock.totBricks() == 0);
	REQUIRE(oBlock.totShapes() == 0);
	Tile oTile;
	oTile.getTileColor().setColorPal(5);
	const int32_t nBrickId = oBlock.brickAdd(oTile, 0, 0, true);
	REQUIRE(nBrickId >= 0);
	REQUIRE(! oBlock.isEmpty());
	REQUIRE(oBlock.totBricks() == 1);
	REQUIRE(oBlock.totShapes() == 1);
}

		//// Helper function
		//struct ContactCompare
		//{
		//	constexpr bool operator()( const Block::Contact& oL, const Block::Contact& oR ) const
		//	{
		//		return std::make_tuple(oL.m_nRelX, oL.m_nRelY, oL.m_nBrickId) < std::make_tuple(oR.m_nRelX, oR.m_nRelY, oR.m_nBrickId);
		//	}
		//};

		// Helper function
		std::vector<int32_t> getAndTestBrickIdxToId(const Block& oBlock, int32_t nBricks, int32_t nTileColorIndexBase)
		{
			std::vector<int32_t> aFoundBrickId(nBricks, -1);
			auto aBrickId = oBlock.brickIds();
			for (auto& nBrickId : aBrickId) {
				const Tile& oTile = oBlock.brick(nBrickId);
				REQUIRE(!oTile.isEmpty());
				const TileColor& oTileColor = oTile.getTileColor();
				REQUIRE( oTileColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
				const int32_t nColorIndex = oTileColor.getColorIndex();
				const int32_t nBrickIdx = nColorIndex - nTileColorIndexBase;
				REQUIRE(( (nBrickIdx >= 0) && (nBrickIdx < nBricks) ));
				aFoundBrickId[nBrickIdx] = nBrickId;
			}
			return aFoundBrickId;
		}
		// Helper function
		std::vector<int32_t> getAndTestShapeIdxToId(const Block& oBlock, int32_t nShapes
								, const std::vector<int32_t>& aFoundBrickId
								, const std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos)
		{
//std::cout << "getAndTestShapeIdxToId   nShapes=" << nShapes << '\n';
			std::vector<int32_t> aFoundShapeId(nShapes, -1);
			for (int32_t nShapeIdx = 0; nShapeIdx < nShapes; ++nShapeIdx) {
				int32_t nFoundShapeId = -1;
				for (int32_t nShapeId = oBlock.shapeFirst(); nShapeId != -1; nShapeId = oBlock.shapeNext(nShapeId) ) {
//std::cout << "getAndTestShapeIdxToId   nShapeId=" << nShapeId << '\n';
					bool bPositionsSame = true;
					const int32_t nTotFoundBrickIds = static_cast<int32_t>(aFoundBrickId.size());
					for (int32_t nBrickIdx = 0; nBrickIdx < nTotFoundBrickIds; ++nBrickIdx) {
						const int32_t nBrickId = aFoundBrickId[nBrickIdx];
						REQUIRE( nBrickId != -1 );
						const bool bVisible = oBlock.shapeBrickVisible(nShapeId, nBrickId);
						const int32_t nPosX = oBlock.shapeBrickPosX(nShapeId, nBrickId);
						const int32_t nPosY = oBlock.shapeBrickPosY(nShapeId, nBrickId);
						bool bIniVisible;
						int32_t nIniX, nIniY;
						std::tie(bIniVisible, nIniX, nIniY) = aShapeBrickPos[nShapeIdx][nBrickIdx];
						//REQUIRE(bVisible == bIniVisible);
						if (! ((nPosX == nIniX) && (nPosY == nIniY) && (bVisible == bIniVisible))) {
							bPositionsSame = false;
							break;
						}
					}
					if (bPositionsSame) {
						// found Id matching Idx
						nFoundShapeId = nShapeId;
						break;
					}
				}
				REQUIRE( nFoundShapeId != -1);
				aFoundShapeId[nShapeIdx] = nFoundShapeId;
			}
			for (auto& nShapeId : aFoundShapeId) {
				REQUIRE( nShapeId != -1 );
			}
			return aFoundShapeId;
		}

TEST_CASE("testBlock, MainConstructor")
{
//std::cout << "testBlock::MainConstructor()" << '\n';
	//
	// .0..   ....   32..   ..3.
	// .1..   210.   .1..   012.
	// .23.   3...   .0..   ....
	// ....   ....   ....   ....
	//
	const int32_t nBricks = 4;
	const int32_t nShapes = 4;
	std::vector<Tile> aBricks;
	aBricks.resize(nBricks);
	for (int32_t nBrickIdx = 0; nBrickIdx < nBricks; ++nBrickIdx) {
		aBricks[nBrickIdx].getTileColor().setColorIndex(nBrickIdx + 1);
	}
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	aShapeBrickPos.resize(nShapes);
	for (auto& aBrickPos : aShapeBrickPos) {
		aBrickPos.resize(nBricks);
	}
	std::vector< std::vector< std::vector< Block::Contact > > > aDirShapeContactPos;
	aDirShapeContactPos.resize(4);
	assert( (Direction::UP >= 0) && (Direction::UP <= 3) );
	assert( (Direction::DOWN >= 0) && (Direction::DOWN <= 3) );
	assert( (Direction::LEFT >= 0) && (Direction::LEFT <= 3) );
	assert( (Direction::RIGHT >= 0) && (Direction::RIGHT <= 3) );
	aDirShapeContactPos[Direction::UP].resize(nShapes);
	aDirShapeContactPos[Direction::DOWN].resize(nShapes);
	aDirShapeContactPos[Direction::LEFT].resize(nShapes);
	aDirShapeContactPos[Direction::RIGHT].resize(nShapes);
	//  0123
	// 0 X
	// 1 X
	// 2 XX
	// 3
	aShapeBrickPos[0][0] = std::make_tuple(true, 1, 0);
	aShapeBrickPos[0][1] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[0][2] = std::make_tuple(true, 1, 2);
	aShapeBrickPos[0][3] = std::make_tuple(true, 2, 2);
	aDirShapeContactPos[Direction::UP][0].push_back({ 1,-1, 0 });
	aDirShapeContactPos[Direction::UP][0].push_back({ 2, 1, 3 });
	aDirShapeContactPos[Direction::DOWN][0].push_back({ 1, 3, 2 });
	aDirShapeContactPos[Direction::DOWN][0].push_back({ 2, 3, 3 });
	aDirShapeContactPos[Direction::LEFT][0].push_back({ 0, 0, 0 });
	aDirShapeContactPos[Direction::LEFT][0].push_back({ 0, 1, 1 });
	aDirShapeContactPos[Direction::LEFT][0].push_back({ 0, 2, 2 });
	aDirShapeContactPos[Direction::RIGHT][0].push_back({ 2, 0, 0 });
	aDirShapeContactPos[Direction::RIGHT][0].push_back({ 2, 1, 1 });
	aDirShapeContactPos[Direction::RIGHT][0].push_back({ 3, 2, 3 });
	//  0123
	// 0
	// 1XXX
	// 2X
	// 3
	aShapeBrickPos[1][0] = std::make_tuple(true, 2, 1);
	aShapeBrickPos[1][1] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[1][2] = std::make_tuple(true, 0, 1);
	aShapeBrickPos[1][3] = std::make_tuple(true, 0, 2);
	aDirShapeContactPos[Direction::UP][1].push_back({ 0, 0, 2 });
	aDirShapeContactPos[Direction::UP][1].push_back({ 1, 0, 1 });
	aDirShapeContactPos[Direction::UP][1].push_back({ 2, 0, 0 });
	aDirShapeContactPos[Direction::DOWN][1].push_back({ 0, 3, 3 });
	aDirShapeContactPos[Direction::DOWN][1].push_back({ 1, 2, 1 });
	aDirShapeContactPos[Direction::DOWN][1].push_back({ 2, 2, 0 });
	aDirShapeContactPos[Direction::LEFT][1].push_back({-1, 1, 2 });
	aDirShapeContactPos[Direction::LEFT][1].push_back({-1, 2, 3 });
	aDirShapeContactPos[Direction::RIGHT][1].push_back({ 3, 1, 0 });
	aDirShapeContactPos[Direction::RIGHT][1].push_back({ 1, 2, 3 });
	//  0123
	// 0XX
	// 1 X
	// 2 X
	// 3
	aShapeBrickPos[2][0] = std::make_tuple(true, 1, 2);
	aShapeBrickPos[2][1] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[2][2] = std::make_tuple(true, 1, 0);
	aShapeBrickPos[2][3] = std::make_tuple(true, 0, 0);
	aDirShapeContactPos[Direction::UP][2].push_back({ 0,-1, 3 });
	aDirShapeContactPos[Direction::UP][2].push_back({ 1,-1, 2 });
	aDirShapeContactPos[Direction::DOWN][2].push_back({ 0, 1, 3 });
	aDirShapeContactPos[Direction::DOWN][2].push_back({ 1, 3, 0 });
	aDirShapeContactPos[Direction::LEFT][2].push_back({-1, 0, 3 });
	aDirShapeContactPos[Direction::LEFT][2].push_back({ 0, 1, 1 });
	aDirShapeContactPos[Direction::LEFT][2].push_back({ 0, 2, 0 });
	aDirShapeContactPos[Direction::RIGHT][2].push_back({ 2, 0, 2 });
	aDirShapeContactPos[Direction::RIGHT][2].push_back({ 2, 1, 1 });
	aDirShapeContactPos[Direction::RIGHT][2].push_back({ 2, 2, 0 });
	//  0123
	// 0  X
	// 1XXX
	// 2
	// 3
	aShapeBrickPos[3][0] = std::make_tuple(true, 0, 1);
	aShapeBrickPos[3][1] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[3][2] = std::make_tuple(true, 2, 1);
	aShapeBrickPos[3][3] = std::make_tuple(true, 2, 0);
	aDirShapeContactPos[Direction::UP][3].push_back({ 0, 0, 0 });
	aDirShapeContactPos[Direction::UP][3].push_back({ 1, 0, 1 });
	aDirShapeContactPos[Direction::UP][3].push_back({ 2,-1, 3 });
	aDirShapeContactPos[Direction::DOWN][3].push_back({ 0, 2, 0 });
	aDirShapeContactPos[Direction::DOWN][3].push_back({ 1, 2, 1 });
	aDirShapeContactPos[Direction::DOWN][3].push_back({ 2, 2, 2 });
	aDirShapeContactPos[Direction::LEFT][3].push_back({ 1, 0, 3 });
	aDirShapeContactPos[Direction::LEFT][3].push_back({-1, 1, 0 });
	aDirShapeContactPos[Direction::RIGHT][3].push_back({ 3, 0, 3 });
	aDirShapeContactPos[Direction::RIGHT][3].push_back({ 3, 1, 2 });

	Block oBlock(nBricks, aBricks, nShapes, aShapeBrickPos);
	REQUIRE( !oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == nShapes);
	REQUIRE( oBlock.totBricks() == nBricks);

	std::vector<int32_t> aFoundBrickId = getAndTestBrickIdxToId(oBlock, nBricks, 1);
	std::vector<int32_t> aFoundShapeId = getAndTestShapeIdxToId(oBlock, nShapes, aFoundBrickId, aShapeBrickPos);

	REQUIRE( oBlock.shapeWidth(aFoundShapeId[2]) == 2 );
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[3]) == 3 );
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[0]) == 3 );
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[3]) == 2 );

	REQUIRE( oBlock.maxWidth() == 3 );
	REQUIRE( oBlock.maxHeight() == 3 );

	REQUIRE(( (oBlock.widestShapeId() == aFoundShapeId[1])
			|| (oBlock.widestShapeId() == aFoundShapeId[3])));
	REQUIRE(( (oBlock.highestShapeId() == aFoundShapeId[0])
			|| (oBlock.highestShapeId() == aFoundShapeId[2])));

	//const std::vector< std::tuple<int32_t, int32_t, int32_t> > 
	for (int32_t nShapeIdx = 0; nShapeIdx < nShapes; ++nShapeIdx) {
		for (int32_t nDir = 0; nDir <= 3; ++nDir) {
			const auto& aShapeContactPos = oBlock.shapeContacts(aFoundShapeId[nShapeIdx], (Direction::VALUE)nDir);
			std::set< Block::Contact > oContactSet; //, ContactCompare
			for (auto& oTuple : aShapeContactPos) {
				REQUIRE( oContactSet.find(oTuple) == oContactSet.end() );
				oContactSet.insert(oTuple);
			}
			std::set< Block::Contact > oContactControlSet; //, ContactCompare
			for (const Block::Contact& oContact : aDirShapeContactPos[nDir][nShapeIdx]) {
				const int32_t nBrickIdx = oContact.m_nBrickId;
				auto oNewContact = oContact;
				oNewContact.m_nBrickId = aFoundBrickId[nBrickIdx];
				oContactControlSet.insert(oNewContact);
			}
			REQUIRE( oContactSet == oContactControlSet);
		}
	}
}

TEST_CASE("testBlock, RotateShapeConstructor")
{
//std::cout << "testBlock::RotateShapeConstructor()" << '\n';
	//
	// .0.. ->   ....   ....   ....
	// .1.. ->   .210   .32.   ..3.
	// .23. ->   .3..   ..1.   012.
	// .... ->   ....   ..0.   ....
	//
	const int32_t nBricks = 4;
	const int32_t nShapes = 4;
	std::vector<Tile> aBricks;
	aBricks.resize(nBricks);
	for (int32_t nBrickIdx = 0; nBrickIdx < nBricks; ++nBrickIdx) {
		aBricks[nBrickIdx].getTileColor().setColorIndex(nBrickIdx + 1);
	}
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	aShapeBrickPos.resize(nShapes);
	for (auto& aBrickPos : aShapeBrickPos) {
		aBrickPos.resize(nBricks);
	}
	aShapeBrickPos[0][0] = std::make_tuple(true, 1, 0);
	aShapeBrickPos[0][1] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[0][2] = std::make_tuple(true, 1, 2);
	aShapeBrickPos[0][3] = std::make_tuple(true, 2, 2);

	aShapeBrickPos[1][0] = std::make_tuple(true, 3, 1);
	aShapeBrickPos[1][1] = std::make_tuple(true, 2, 1);
	aShapeBrickPos[1][2] = std::make_tuple(true, 1, 1);
	aShapeBrickPos[1][3] = std::make_tuple(true, 1, 2);

	aShapeBrickPos[2][0] = std::make_tuple(true, 2, 3);
	aShapeBrickPos[2][1] = std::make_tuple(true, 2, 2);
	aShapeBrickPos[2][2] = std::make_tuple(true, 2, 1);
	aShapeBrickPos[2][3] = std::make_tuple(true, 1, 1);

	aShapeBrickPos[3][0] = std::make_tuple(true, 0, 2);
	aShapeBrickPos[3][1] = std::make_tuple(true, 1, 2);
	aShapeBrickPos[3][2] = std::make_tuple(true, 2, 2);
	aShapeBrickPos[3][3] = std::make_tuple(true, 2, 1);

	const int32_t nWH = 4;
	Block oBlock(nBricks, aBricks, aShapeBrickPos[0], nWH);
	REQUIRE( !oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == nShapes);
	REQUIRE( oBlock.totBricks() == nBricks);

	std::vector<int32_t> aFoundBrickId = getAndTestBrickIdxToId(oBlock, nBricks, 1);
	std::vector<int32_t> aFoundShapeId = getAndTestShapeIdxToId(oBlock, nShapes, aFoundBrickId, aShapeBrickPos);
	for (int32_t nShapeIdx = 0; nShapeIdx < nShapes; ++nShapeIdx) {
		const int32_t nShapeId = aFoundShapeId[nShapeIdx];
		REQUIRE( nShapeId != -1 );
		if (nShapeIdx == 0) {
			REQUIRE( oBlock.shapePrec(nShapeId) == -1 );
			REQUIRE( oBlock.shapeNext(nShapeId) == aFoundShapeId[nShapeIdx + 1] );
		} else if (nShapeIdx == nShapes - 1) {
			REQUIRE( oBlock.shapePrec(nShapeId) == aFoundShapeId[nShapeIdx - 1] );
			REQUIRE( oBlock.shapeNext(nShapeId) == -1);
		} else {
			REQUIRE( oBlock.shapePrec(nShapeId) == aFoundShapeId[nShapeIdx - 1] );
			REQUIRE( oBlock.shapeNext(nShapeId) == aFoundShapeId[nShapeIdx + 1] );
		}
		int32_t nShouldBrickId;
		for (int32_t nBrickIdx = 0; nBrickIdx < nBricks; ++nBrickIdx) {
			const int32_t nBrickId = aFoundBrickId[nBrickIdx];
			bool bVisible;
			int32_t nX, nY;
			std::tie(bVisible, nX, nY) = aShapeBrickPos[nShapeIdx][nBrickIdx];
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, nX, nY);
			REQUIRE( (bVisible ? nBrickId : -1) == nShouldBrickId );
		}
		if (nShapeIdx == 0) {
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 0, 0);
			REQUIRE( nShouldBrickId == -1 );
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 2, 1);
			REQUIRE( nShouldBrickId == -1 );
		} else if (nShapeIdx == 1) {
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 0, 0);
			REQUIRE( nShouldBrickId == -1 );
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 2, 2);
			REQUIRE( nShouldBrickId == -1 );
		} else if (nShapeIdx == 2) {
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 1, 0);
			REQUIRE( nShouldBrickId == -1 );
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 0, 1);
			REQUIRE( nShouldBrickId == -1 );
		} else if (nShapeIdx == 3) {
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 0, 1);
			REQUIRE( nShouldBrickId == -1 );
			nShouldBrickId = oBlock.shapeBrickVisiblePosition(nShapeId, 0, 0);
			REQUIRE( nShouldBrickId == -1 );
		}
	}

	//int32_t nShapeId = aFoundShapeId[0];
}

TEST_CASE("testBlock, FusionConstructor")
{
//std::cout << "testBlock::FusionConstructor()" << '\n';
	//
	//  ..+0++..   + empty space of first
	//  ..+1++..   - empty space of second
	//  .-=234-.   = overlap empty space
	//  .-===C-.   0..4 first bricks
	//  .---AB-.   A..C second bricks
	//  .------.   (-1,2) position of second relative to first
	//
	// + empty space of result
	//  ..+0+++.   ..+++++.
	//  ..+1+++.   ..++210.
	//  ..+234+.   ..7+3++.
	//  ..+++5+.   ..654++.
	//  ..++76+.   ..+++++.
	//  ........   ........
	//
//std::cout << "testFusionConstructor --------- first" << '\n';

	const int32_t nFirstBricks = 5;
	std::vector<Tile> aFirstBricks;
	aFirstBricks.resize(nFirstBricks);
	for (int32_t nBrickIdx = 0; nBrickIdx < nFirstBricks; ++nBrickIdx) {
		aFirstBricks[nBrickIdx].getTileColor().setColorIndex(nBrickIdx + 1);
	}
	std::vector< std::tuple<bool, int32_t, int32_t> > aFirstBrickPos;
	aFirstBrickPos.resize(nFirstBricks);
	aFirstBrickPos[0] = std::make_tuple(true, 1, 0);
	aFirstBrickPos[1] = std::make_tuple(true, 1, 1);
	aFirstBrickPos[2] = std::make_tuple(true, 1, 2);
	aFirstBrickPos[3] = std::make_tuple(true, 2, 2);
	aFirstBrickPos[4] = std::make_tuple(true, 3, 2);

	const int32_t nFirstWH = 4;
	Block oFirstBlock(nFirstBricks, aFirstBricks, aFirstBrickPos, nFirstWH);
	REQUIRE( !oFirstBlock.isEmpty() );

//std::cout << "testFusionConstructor --------- second" << '\n';

	const int32_t nSecondBricks = 3;
	std::vector<Tile> aSecondBricks;
	aSecondBricks.resize(nSecondBricks);
	for (int32_t nBrickIdx = 0; nBrickIdx < nSecondBricks; ++nBrickIdx) {
		aSecondBricks[nBrickIdx].getTileColor().setColorIndex(nBrickIdx + 101);
	}
	std::vector< std::tuple<bool, int32_t, int32_t> > aSecondBrickPos;
	aSecondBrickPos.resize(nSecondBricks);
	aSecondBrickPos[0] = std::make_tuple(true, 3, 2);
	aSecondBrickPos[1] = std::make_tuple(true, 4, 2);
	aSecondBrickPos[2] = std::make_tuple(true, 4, 1);

	const int32_t nSecondWH = 6;
	Block oSecondBlock(nSecondBricks, aSecondBricks, aSecondBrickPos, nSecondWH);
	REQUIRE( !oSecondBlock.isEmpty() );

//std::cout << "testFusionConstructor --------- result" << '\n';
	const int32_t nSecRelX = -1;
	const int32_t nSecRelY = +2;
	int32_t nDeltaFromFirstPosX;
	int32_t nDeltaFromFirstPosY;
	std::unordered_map<int32_t, int32_t> oFirstBrickIds;
	std::unordered_map<int32_t, int32_t> oSecondBrickIds;
	Block oResBlock(0, oFirstBlock, 0, oSecondBlock, nSecRelX, nSecRelY
					, nDeltaFromFirstPosX, nDeltaFromFirstPosY
					, oFirstBrickIds, oSecondBrickIds);
//std::cout << "        nDeltaFromFirstPosX=" << nDeltaFromFirstPosX << "  nDeltaFromFirstPosY=" << nDeltaFromFirstPosY << '\n';

	REQUIRE(( (nDeltaFromFirstPosX == 0) && (nDeltaFromFirstPosY == 0) ));
	REQUIRE( oResBlock.shapeTotVisibleBricks(oResBlock.shapeFirst()) == nFirstBricks + nSecondBricks);

	REQUIRE( static_cast<int32_t>(oFirstBrickIds.size()) == nFirstBricks );
	for (auto& oFirstBrickIdMap : oFirstBrickIds) {
		const int32_t nFirstBrickId = oFirstBrickIdMap.first;
		const int32_t nResBrickId = oFirstBrickIdMap.second;
		const Tile& oFirstTile = oFirstBlock.brick(nFirstBrickId);
		const Tile& oResTile = oResBlock.brick(nResBrickId);
		REQUIRE( oFirstTile == oResTile );
		REQUIRE(!oFirstTile.isEmpty());
		const TileColor& oTileColor = oFirstTile.getTileColor();
		REQUIRE( oTileColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
	}
	REQUIRE( static_cast<int32_t>(oSecondBrickIds.size()) == nSecondBricks );
	for (auto& oSecondBrickIdMap : oSecondBrickIds) {
		const int32_t nSecondBrickId = oSecondBrickIdMap.first;
		const int32_t nResBrickId = oSecondBrickIdMap.second;
		const Tile& oSecondTile = oSecondBlock.brick(nSecondBrickId);
		const Tile& oResTile = oResBlock.brick(nResBrickId);
		REQUIRE( oSecondTile == oResTile );
	}

	std::vector<int32_t> aFoundFirstBrickId(nFirstBricks, -1);
	std::vector<int32_t> aFoundSecondBrickId(nSecondBricks, -1);
	auto aBrickId = oResBlock.brickIds();
	for (auto& nResBrickId : aBrickId) {
		const Tile& oTile = oResBlock.brick(nResBrickId);
		REQUIRE(!oTile.isEmpty());
		const TileColor& oTileColor = oTile.getTileColor();
		REQUIRE( oTileColor.getColorType() == TileColor::COLOR_TYPE_INDEX );
		const int32_t nColorIndex = oTileColor.getColorIndex();
		if (nColorIndex < 100) {
			const int32_t nBrickIdx = nColorIndex - 1;
			REQUIRE(( (nBrickIdx >= 0) && (nBrickIdx < nFirstBricks) ));
			aFoundFirstBrickId[nBrickIdx] = nResBrickId;
		} else {
			const int32_t nBrickIdx = nColorIndex - 101;
			REQUIRE(( (nBrickIdx >= 0) && (nBrickIdx < nSecondBricks) ));
			aFoundSecondBrickId[nBrickIdx] = nResBrickId;
		}
	}
	const int32_t nResShapeId = oResBlock.shapeFirst();
	for (int32_t nFirstBrickIdx = 0; nFirstBrickIdx < nFirstBricks; ++nFirstBrickIdx) {
		const int32_t nResBrickId = aFoundFirstBrickId[nFirstBrickIdx];
		REQUIRE( nResBrickId != -1 );
//std::cout << "     nFirstBrickIdx=" << nFirstBrickIdx << "  nResBrickId=" << nResBrickId << "  nResShapeId=" << nResShapeId << '\n';
		const bool bVisible = oResBlock.shapeBrickVisible(nResShapeId, nResBrickId);
		const int32_t nPosX = oResBlock.shapeBrickPosX(nResShapeId, nResBrickId);
		const int32_t nPosY = oResBlock.shapeBrickPosY(nResShapeId, nResBrickId);
//std::cout << "        nPosX=" << nPosX << "  nPosY=" << nPosY << '\n';
		bool bIniVisible;
		int32_t nIniX, nIniY;
		std::tie(bIniVisible, nIniX, nIniY) = aFirstBrickPos[nFirstBrickIdx];
//std::cout << "        nIniX=" << nIniX << "  nIniY=" << nIniY << '\n';
		REQUIRE(bVisible);
		REQUIRE( ((nPosX == nIniX - nDeltaFromFirstPosX) && (nPosY == nIniY - nDeltaFromFirstPosY)) );
	}
	for (int32_t nSecondBrickIdx = 0; nSecondBrickIdx < nSecondBricks; ++nSecondBrickIdx) {
		const int32_t nResBrickId = aFoundSecondBrickId[nSecondBrickIdx];
		REQUIRE( nResBrickId != -1 );
		const bool bVisible = oResBlock.shapeBrickVisible(nResShapeId, nResBrickId);
		const int32_t nPosX = oResBlock.shapeBrickPosX(nResShapeId, nResBrickId) - nSecRelX;
		const int32_t nPosY = oResBlock.shapeBrickPosY(nResShapeId, nResBrickId) - nSecRelY;
//std::cout << "     nSecondBrickIdx=" << nSecondBrickIdx << "  nResBrickId=" << nResBrickId << "  nResShapeId=" << nResShapeId << '\n';
//std::cout << "        nPosX=" << nPosX << "  nPosY=" << nPosY << '\n';
		bool bIniVisible;
		int32_t nIniX, nIniY;
		std::tie(bIniVisible, nIniX, nIniY) = aSecondBrickPos[nSecondBrickIdx];
//std::cout << "        nIniX=" << nIniX << "  nIniY=" << nIniY << '\n';
		REQUIRE(bVisible);
		REQUIRE( ((nPosX == nIniX - nDeltaFromFirstPosX) && (nPosY == nIniY - nDeltaFromFirstPosY)) );
	}

	REQUIRE( !oResBlock.isEmpty() );

	REQUIRE( oResBlock.totBricks() == nFirstBricks + nSecondBricks);
	REQUIRE( oResBlock.totShapes() == 4);
}

	Block commonExampleBlock(int32_t& nBricks, int32_t& nShapes
							, std::vector<Tile>& aBricks
							, std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > >& aShapeBrickPos
							, std::vector<int32_t>& aFoundBrickId, std::vector<int32_t>& aFoundShapeId)
	{
		//
		// .0..   ....   ....
		// .1..   .10.   .3..
		// .23.   .32.   .10.
		// ....   ....   ..2.
		//
		nBricks = 4;
		nShapes = 3;
		aBricks.resize(nBricks);
		for (int32_t nBrickIdx = 0; nBrickIdx < nBricks; ++nBrickIdx) {
			aBricks[nBrickIdx].getTileColor().setColorIndex(nBrickIdx + 1);
		}
		aShapeBrickPos.resize(nShapes);
		for (auto& aBrickPos : aShapeBrickPos) {
			aBrickPos.resize(nBricks);
		}
		aShapeBrickPos[0][0] = std::make_tuple(true, 1, 0);
		aShapeBrickPos[0][1] = std::make_tuple(true, 1, 1);
		aShapeBrickPos[0][2] = std::make_tuple(true, 1, 2);
		aShapeBrickPos[0][3] = std::make_tuple(true, 2, 2);

		aShapeBrickPos[1][0] = std::make_tuple(false, 2, 1);
		aShapeBrickPos[1][1] = std::make_tuple(true, 1, 1);
		aShapeBrickPos[1][2] = std::make_tuple(false, 2, 2);
		aShapeBrickPos[1][3] = std::make_tuple(false, 1, 2);

		aShapeBrickPos[2][0] = std::make_tuple(true, 2, 2);
		aShapeBrickPos[2][1] = std::make_tuple(true, 1, 2);
		aShapeBrickPos[2][2] = std::make_tuple(false, 2, 3);
		aShapeBrickPos[2][3] = std::make_tuple(true, 1, 1);

		Block oBlock(nBricks, aBricks, nShapes, aShapeBrickPos);
		REQUIRE( !oBlock.isEmpty() );

		REQUIRE( oBlock.totShapes() == nShapes);
		REQUIRE( oBlock.totBricks() == nBricks);

		aFoundBrickId = getAndTestBrickIdxToId(oBlock, nBricks, 1);
		aFoundShapeId = getAndTestShapeIdxToId(oBlock, nShapes, aFoundBrickId, aShapeBrickPos);

		return oBlock;
	}

TEST_CASE("testBlock, RemoveBrick")
{
//std::cout << "testBlock::RemoveBrick()" << '\n';
	int32_t nBricks;
	int32_t nShapes;
	std::vector<Tile> aBricks;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	std::vector<int32_t> aFoundBrickId;
	std::vector<int32_t> aFoundShapeId;
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);

	int32_t nShapeIdx = 0;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == nBricks);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 3);

	nShapeIdx = 1;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 1);

	nShapeIdx = 2;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 3);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 2);

	oBlock.brickRemove(aFoundBrickId[1]);

	REQUIRE( ! oBlock.isBrickId(aFoundBrickId[1]) );

	REQUIRE( oBlock.totShapes() == nShapes);
	REQUIRE( oBlock.totBricks() == nBricks - 1);

	nShapeIdx = 0;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 3);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 3);
	nShapeIdx = 1;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 0);
	nShapeIdx = 2;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 2);

	oBlock.brickRemove(aFoundBrickId[0]);

	REQUIRE( oBlock.totShapes() == nShapes);
	REQUIRE( oBlock.totBricks() == nBricks - 2);

	nShapeIdx = 0;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 2);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[nShapeIdx], aFoundBrickId[3]) == true);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[nShapeIdx], aFoundBrickId[2]) == true);
	nShapeIdx = 1;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 0);
	nShapeIdx = 2;
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeWidth(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeHeight(aFoundShapeId[nShapeIdx]) == 1);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[nShapeIdx], aFoundBrickId[3]) == true);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[nShapeIdx], aFoundBrickId[2]) == false);
}

TEST_CASE("testBlock, RemoveShape")
{
//std::cout << "testBlock::RemoveShape()" << '\n';
	int32_t nBricks;
	int32_t nShapes;
	std::vector<Tile> aBricks;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	std::vector<int32_t> aFoundBrickId;
	std::vector<int32_t> aFoundShapeId;
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);

	oBlock.shapeRemove(aFoundShapeId[0]);
	REQUIRE( !oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == 2);
	REQUIRE( oBlock.totBricks() == nBricks);

	REQUIRE( ! oBlock.isShapeId(aFoundShapeId[0]) );

	oBlock.shapeRemove(aFoundShapeId[2]);
	REQUIRE( !oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == 1);
	REQUIRE( oBlock.totBricks() == nBricks);

	REQUIRE( ! oBlock.isShapeId(aFoundShapeId[2]) );

	oBlock.shapeRemove(aFoundShapeId[1]);
	REQUIRE( ! oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == 0);
	REQUIRE( oBlock.totBricks() == nBricks);

	REQUIRE( ! oBlock.isShapeId(aFoundShapeId[1]) );
}

TEST_CASE("testBlock, RemoveAllInvisibleShapes")
{
//std::cout << "testBlock::RemoveAllInvisibleShapes()" << '\n';
	int32_t nBricks;
	int32_t nShapes;
	std::vector<Tile> aBricks;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	std::vector<int32_t> aFoundBrickId;
	std::vector<int32_t> aFoundShapeId;
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);

	oBlock.brickRemove(aFoundBrickId[1]);

	oBlock.shapeRemoveAllInvisible();

	REQUIRE( !oBlock.isEmpty() );

	REQUIRE( oBlock.totShapes() == 2);
	REQUIRE( oBlock.totBricks() == nBricks - 1);

	REQUIRE(   oBlock.isShapeId(aFoundShapeId[0]) );
	REQUIRE( ! oBlock.isShapeId(aFoundShapeId[1]) );
	REQUIRE(   oBlock.isShapeId(aFoundShapeId[2]) );
}

TEST_CASE("testBlock, RemoveSetVisible")
{
//std::cout << "testBlock::SetVisible()" << '\n';
	int32_t nBricks;
	int32_t nShapes;
	std::vector<Tile> aBricks;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	std::vector<int32_t> aFoundBrickId;
	std::vector<int32_t> aFoundShapeId;
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);

	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[0]) == 4);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[0], aFoundBrickId[1]) == true);
	oBlock.shapeBrickSetVisible(aFoundShapeId[0], aFoundBrickId[1] , false);
	REQUIRE( oBlock.shapeTotVisibleBricks(aFoundShapeId[0]) == 3);
	REQUIRE( oBlock.shapeBrickVisible(aFoundShapeId[0], aFoundBrickId[1]) == false);
}

TEST_CASE("testBlock, AddBrick")
{
//std::cout << "testBlock::AddBrick()" << '\n';
	Block oBlock;
	Tile oTile1;

	REQUIRE( oBlock.isEmpty() );
	oTile1.getTileColor().setColorIndex(77);
	REQUIRE( oBlock.totBricks() == 0 );
	const int32_t nBrickId1 = oBlock.brickAdd(oTile1, 3, 2, true);
	REQUIRE( oBlock.totBricks() == 1 );
	REQUIRE( oBlock.brick(nBrickId1) == oTile1 );
	REQUIRE( oBlock.totShapes() == 1 );
	REQUIRE( ! oBlock.isEmpty() );
	auto aBrickId = oBlock.brickIds();
	REQUIRE( aBrickId.size() == 1 );
	REQUIRE( aBrickId[0] == nBrickId1 );
//std::cout << "BlockTest::testAddBrick 1" << '\n';
	const int32_t nShapeId1 = oBlock.shapeInsert(-1);
	REQUIRE( ! oBlock.isEmpty() );
	REQUIRE( oBlock.totBricks() == 1 );
	REQUIRE( oBlock.totShapes() == 2 );
	REQUIRE( oBlock.shapeTotVisibleBricks(nShapeId1) == 0 );

	Tile oTile2;
	oTile2.getTileColor().setColorIndex(88);
	const int32_t nBrickId2 = oBlock.brickAdd(oTile2, -1, 1, true);
	REQUIRE( oBlock.totBricks() == 2 );
	aBrickId = oBlock.brickIds();
	REQUIRE( aBrickId.size() == 2 );
	if (aBrickId[0] == nBrickId1) {
		REQUIRE( aBrickId[1] == nBrickId2 );
	} else {
		REQUIRE( aBrickId[0] == nBrickId2 );
		REQUIRE( aBrickId[1] == nBrickId1 );
	}
	REQUIRE( oBlock.brick(nBrickId1) == oTile1 );
	REQUIRE( oBlock.brick(nBrickId2) == oTile2 );
	REQUIRE( oBlock.shapeTotVisibleBricks(nShapeId1) == 1 );
	int32_t nControlBrickId;
	nControlBrickId = oBlock.shapeBrickVisiblePosition(nShapeId1, -1, 1);
	REQUIRE( nControlBrickId == nBrickId2 );

	Tile oTile3;
	oTile3.getTileColor().setColorIndex(88);
	const int32_t nBrickId3 = oBlock.brickAdd(oTile3, -1, 1, true);
	REQUIRE( oBlock.totBricks() == 3 );
	aBrickId = oBlock.brickIds();
	REQUIRE( aBrickId.size() == 3 );
	REQUIRE( oBlock.shapeTotVisibleBricks(nShapeId1) == 1 );
	nControlBrickId = oBlock.shapeBrickVisiblePosition(nShapeId1, -1, 1);
	REQUIRE( nControlBrickId == nBrickId2 );
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId1, nBrickId3) );
}

TEST_CASE("testBlock, ModifyBrick")
{
//std::cout << "testBlock::ModifyBrick()" << '\n';
	Block oBlock;

	Tile oTile1;
	oTile1.getTileColor().setColorIndex(77);
	const int32_t nBrickId1 = oBlock.brickAdd(oTile1, 3, 2, true);
	REQUIRE( oBlock.totBricks() == 1 );
	REQUIRE( oBlock.isBrickId(nBrickId1) );
	REQUIRE( oBlock.brick(nBrickId1) == oTile1 );

	Tile oTile2;
	oTile2.getTileColor().setColorIndex(78);
	const int32_t nBrickId2 = oBlock.brickAdd(oTile2, 1, 2, true);
	REQUIRE( oBlock.totBricks() == 2 );
	REQUIRE( oBlock.isBrickId(nBrickId2) );
	REQUIRE( oBlock.brick(nBrickId2) == oTile2 );

	Tile oTile3;
	oTile3.getTileColor().setColorIndex(78);
	const int32_t nBrickId3 = oBlock.brickAdd(oTile3, 1, 2, true);
	REQUIRE( oBlock.totBricks() == 3 );
	REQUIRE( oBlock.isBrickId(nBrickId3) );
	REQUIRE( oBlock.brick(nBrickId3) == oTile3 );

	Tile oTileM;
	oTileM.getTileColor().setColorIndex(99);
	oBlock.brickModify(nBrickId2, oTileM);
	REQUIRE( oBlock.totBricks() == 3 );
	REQUIRE( oBlock.isBrickId(nBrickId2) );
	REQUIRE( oBlock.brick(nBrickId2) == oTileM );
}

TEST_CASE("testBlock, InsertShape")
{
//std::cout << "testBlock::InsertShape()" << '\n';
	{
	Block oBlock;
	REQUIRE( oBlock.totShapes() == 0 );
	const int32_t nShapeId1 = oBlock.shapeInsert(-1);
	REQUIRE( oBlock.isEmpty() );
	REQUIRE( oBlock.totShapes() == 1 );
	auto aShapeId = oBlock.shapeIds();
	REQUIRE( aShapeId.size() == 1 );
	REQUIRE( aShapeId[0] == nShapeId1 );

	const int32_t nShapeId2 = oBlock.shapeInsert(nShapeId1);
	REQUIRE( oBlock.isEmpty() );
	REQUIRE( oBlock.totShapes() == 2 );
	REQUIRE( oBlock.shapeFirst() == nShapeId2 );
	REQUIRE( oBlock.shapeLast() == nShapeId1 );

	oBlock.shapeRemove(nShapeId2);
	REQUIRE( oBlock.totShapes() == 1 );

	const int32_t nShapeId3 = oBlock.shapeInsert(-1);
	REQUIRE( oBlock.totShapes() == 2 );
	REQUIRE( oBlock.shapeFirst() == nShapeId1 );
	REQUIRE( oBlock.shapeLast() == nShapeId3 );
	}
	{
	Block oBlock;
	Tile oTile1;
	oTile1.getTileColor().setColorIndex(77);
	REQUIRE( oBlock.totBricks() == 0 );
	const int32_t nBrickId1 = oBlock.brickAdd(oTile1, 3, 2, true);
	REQUIRE( oBlock.totBricks() == 1 );
	REQUIRE( oBlock.brick(nBrickId1) == oTile1 );
	REQUIRE( oBlock.totShapes() == 1 );

	const int32_t nShapeId1 = oBlock.shapeInsert(-1);
	REQUIRE( ! oBlock.isEmpty() );
	REQUIRE( oBlock.totShapes() == 2 );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId1) == 0 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId1) == 0 );
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId1, nBrickId1) );

	Tile oTile2;
	oTile2.getTileColor().setColorIndex(88);
	const int32_t nBrickId2 = oBlock.brickAdd(oTile2, 3, 2, true);
	REQUIRE( oBlock.totBricks() == 2 );
	REQUIRE( oBlock.brick(nBrickId2) == oTile2 );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId2) == 3 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId2) == 2 );
	// position already occupied by another brick
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId1, nBrickId2) );

	const int32_t nShapeId2 = oBlock.shapeInsert(-1, nShapeId1);
	REQUIRE( oBlock.totShapes() == 3 );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId2, nBrickId1) == 0 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId2, nBrickId1) == 0 );
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId2, nBrickId1) );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId2, nBrickId2) == 3 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId2, nBrickId2) == 2 );
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId2, nBrickId2) );
	}
}

TEST_CASE("testBlock, ModifyShape")
{
//std::cout << "testBlock::ModifyShape()" << '\n';
	int32_t nBricks;
	int32_t nShapes;
	std::vector<Tile> aBricks;
	std::vector< std::vector< std::tuple<bool, int32_t, int32_t> > > aShapeBrickPos;
	std::vector<int32_t> aFoundBrickId;
	std::vector<int32_t> aFoundShapeId;
	// .0..   ....   ....
	// .1..   .10.   .3..
	// .23.   .32.   .10.
	// ....   ....   ..2.
	{
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);
	const int32_t nShapeId1 = aFoundShapeId[1];
	const int32_t nBrickId0 = aFoundBrickId[0];
	//const int32_t nBrickId1 = aFoundBrickId[1];
	//const int32_t nBrickId2 = aFoundBrickId[2];
	//const int32_t nBrickId3 = aFoundBrickId[3];

	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId0) == 2 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId0) == 1 );
	bool bAllV = oBlock.shapeModifyBricks(nShapeId1, {std::make_tuple(nBrickId0, 1, 1, true)});
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId0) == 1 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId0) == 1 );
	REQUIRE( ! oBlock.shapeBrickVisible(nShapeId1, nBrickId0) );
	REQUIRE( !bAllV );
	bAllV = oBlock.shapeModifyBricks(nShapeId1, {std::make_tuple(nBrickId0, 0, 1, true)});
	REQUIRE( oBlock.shapeBrickVisible(nShapeId1, nBrickId0) );
	REQUIRE( bAllV );
	}
	{
	Block oBlock = commonExampleBlock(nBricks, nShapes, aBricks, aShapeBrickPos, aFoundBrickId, aFoundShapeId);
	const int32_t nShapeId1 = aFoundShapeId[1];
	const int32_t nBrickId0 = aFoundBrickId[0];
	const int32_t nBrickId1 = aFoundBrickId[1];
	//const int32_t nBrickId2 = aFoundBrickId[2];
	//const int32_t nBrickId3 = aFoundBrickId[3];

	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId0) == 2 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId0) == 1 );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId1) == 1 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId1) == 1 );
	bool bAllV = oBlock.shapeModifyBricks(nShapeId1, {std::make_tuple(nBrickId0, 1, 1, true), std::make_tuple(nBrickId1, 2, 1, true)});
	REQUIRE( bAllV );
	REQUIRE( oBlock.shapeBrickVisible(nShapeId1, nBrickId0) );
	REQUIRE( oBlock.shapeBrickVisible(nShapeId1, nBrickId1) );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId0) == 1 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId0) == 1 );
	REQUIRE( oBlock.shapeBrickPosX(nShapeId1, nBrickId1) == 2 );
	REQUIRE( oBlock.shapeBrickPosY(nShapeId1, nBrickId1) == 1 );
	}
}

} // namespace testing

} // namespace stmg
