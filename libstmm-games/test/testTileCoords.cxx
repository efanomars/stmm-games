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
 * File:   testTileCoords.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "utile/tilecoords.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testTileCoords, Constructor")
{
//std::cout << "testTileCoords::Constructor()" << '\n';
	{
		TileCoords oCoords(100);
		REQUIRE( oCoords.size() == 0 );

		oCoords.reInit();
		REQUIRE( oCoords.size() == 0 );
	}
	TileCoords oCoords;
	REQUIRE( oCoords.size() == 0 );

	oCoords.reInit();
	REQUIRE( oCoords.size() == 0 );

	oCoords.reInit(10);
	REQUIRE( oCoords.size() == 0 );
}

TEST_CASE("testTileCoords, Add")
{
//std::cout << "testTileCoords::Add()" << '\n';
	TileCoords oCoords;

	Tile oTile;
	oTile.getTileColor().setColorIndex(55);
	oCoords.add(10,20, oTile);
	REQUIRE( oCoords.size() == 1 );
	REQUIRE( oCoords.contains(10,20) );
	REQUIRE( !oCoords.contains(20,10) );
	REQUIRE( oCoords.getTile(10,20).first );
	REQUIRE( oCoords.getTile(10,20).second == oTile );
	Tile oTile2;
	oTile2.getTileChar().setChar(88);
	oCoords.add(10,20, oTile2);
	REQUIRE( oCoords.size() == 1 );
	REQUIRE( oCoords.contains(10,20) );
	REQUIRE( !oCoords.contains(20,10) );
	REQUIRE( oCoords.getTile(NPoint{10,20}).first );
	REQUIRE( oCoords.getTile(10,20).second == oTile2 );

	NRect oRect;
	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 10 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 1 );
	REQUIRE( oRect.m_nH == 1 );

	oCoords.add(11,21, oTile);
	REQUIRE( oCoords.size() == 2 );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 10 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 2 );
	REQUIRE( oRect.m_nH == 2 );

	Tile oTile3;
	oTile3.getTileAlpha().setAlpha(127);
	oCoords.addRect(7,22,6,2, oTile3);
	REQUIRE( oCoords.size() == 2 + 6*2);
	REQUIRE( !oCoords.contains(6,22) );
	REQUIRE( !oCoords.contains(7,24) );
	REQUIRE( !oCoords.getTile(NPoint{7, 24}).first );
	REQUIRE( !oCoords.contains(7,21) );
	REQUIRE( oCoords.contains(7,22) );
	REQUIRE( oCoords.getTile(NPoint{7, 22}).first );
	REQUIRE( oCoords.getTile(7, 22).second == oTile3 );
	REQUIRE( oCoords.contains(7,23) );
	REQUIRE( oCoords.contains(8,22) );
	REQUIRE( oCoords.contains(12,22) );
	REQUIRE( oCoords.getTile(NPoint{12, 22}).first );
	REQUIRE( oCoords.getTile(12, 22).second == oTile3 );
	REQUIRE( oCoords.contains(12,23) );
	REQUIRE( !oCoords.contains(13,22) );
	REQUIRE( !oCoords.contains(12,24) );
	REQUIRE( !oCoords.getTile(NPoint{12, 24}).first );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 6 );
	REQUIRE( oRect.m_nH == 4 );

	oCoords.remove(8,22);
	REQUIRE( oCoords.size() == 2 + 6*2 - 1);
	REQUIRE( !oCoords.contains(8,22) );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 6 );
	REQUIRE( oRect.m_nH == 4 );

	oCoords.add(8,22, oTile2);
	REQUIRE( oCoords.size() == 2 + 6*2);
	REQUIRE( oCoords.contains(8,22) );
	REQUIRE( oCoords.getTile(NPoint{8, 22}).first );
	REQUIRE( oCoords.getTile(8, 22).second == oTile2 );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 6 );
	REQUIRE( oRect.m_nH == 4 );

	oCoords.remove(8,22);
	REQUIRE( oCoords.size() == 2 + 6*2 - 1);
	REQUIRE( !oCoords.contains(8,22) );
	REQUIRE( !oCoords.getTile(NPoint{8, 22}).first );

	oCoords.removeInRect(10,20,4,4);
	REQUIRE( oCoords.size() == 5);

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 22 );
	REQUIRE( oRect.m_nW == 3 );
	REQUIRE( oRect.m_nH == 2 );
}
TEST_CASE("testTileCoords, SuperAdd")
{
//std::cout << "testTileCoords::SuperAdd()" << '\n';
	TileCoords oCoords;
	REQUIRE( oCoords.size() == 0 );
	oCoords.Coords::add(1,2);
	REQUIRE( oCoords.size() == 1 );

	TileCoords oCoords2;
	oCoords2.add(oCoords);
	REQUIRE( oCoords2.size() == 1 );
}

TEST_CASE("testTileCoords, Subtract")
{
//std::cout << "testTileCoords::Subtract()" << '\n';
	TileCoords oCoords1;
	TileCoords oCoords2;

	Tile oTile3;
	oTile3.getTileAlpha().setAlpha(127);
	oCoords1.addRect(10,20,5,4, oTile3);
	REQUIRE( oCoords1.size() == 20 );

	Tile oTile2;
	oTile2.getTileChar().setChar(88);
	oCoords2.addRect(11,21,5,4, oTile2);
	REQUIRE( oCoords2.size() == 20 );

	oCoords1.remove(oCoords2);
	REQUIRE( oCoords1.size() == 8 );

	NRect oRect;
	oRect = oCoords1.getMinMax();
	REQUIRE( oRect.m_nX == 10 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 5 );
	REQUIRE( oRect.m_nH == 4 );

	REQUIRE( oCoords1.contains(10,20) );
	REQUIRE( oCoords1.contains(10,23) );
	REQUIRE( oCoords1.contains(14,20) );
	REQUIRE( !oCoords1.contains(11,21) );
	REQUIRE( !oCoords1.contains(14,25) );

	oCoords1.remove(oCoords1);
	REQUIRE( oCoords1.size() == 0 );

	oCoords1 = oCoords2;
	REQUIRE( oCoords1.size() == 20 );
	oCoords1.remove(oCoords2);
	REQUIRE( oCoords1.size() == 0 );
}

TEST_CASE("testTileCoords, SuperSubtract")
{
//std::cout << "testTileCoords::SuperSubtract()" << '\n';
	{
	TileCoords oCoords;
	REQUIRE( oCoords.size() == 0 );
	oCoords.Coords::add(1,2);
	bool bExisted = oCoords.Coords::remove(2,1);
	REQUIRE( ! bExisted );
	REQUIRE( oCoords.size() == 1 );
	bExisted = oCoords.Coords::remove(1,2);
	REQUIRE( bExisted );
	REQUIRE( oCoords.size() == 0 );
	}

	{
	TileCoords oCoords;
	REQUIRE( oCoords.size() == 0 );
	oCoords.Coords::add(1,2);

	TileCoords oCoords2;
	oCoords2.Coords::add(oCoords);
	oCoords2.Coords::remove(oCoords);
	REQUIRE( oCoords2.size() == 0 );
	bool bExisted = oCoords.Coords::remove(2,1);
	REQUIRE( ! bExisted );
	}
}
TEST_CASE("testTileCoords, EmptyTiles")
{
	TileCoords oCoords1;
	Tile oTile0;
	oTile0.getTileChar().setChar(65);
	oCoords1.addRect(10,20,5,4, oTile0);
	auto oPair1 = oCoords1.getTile(10,21);
	REQUIRE( oCoords1.size() == 20 );
	REQUIRE( oPair1.first );
	REQUIRE( oPair1.second == oTile0);
	for (int32_t nX = 0; nX < 5; ++nX) {
		for (int32_t nY = 0; nY < 4; ++nY) {
			Tile oTile2;
			oTile2.getTileColor().setColorIndex(100 + nX * 10 + nY);
			oCoords1.add(10 + nX, 20 + nY, oTile2);
		}
	}
//std::cout << "-EmptyTiles  --- 1" << '\n';
	oCoords1.Coords::add(10,21);
	REQUIRE( oCoords1.size() == 20 );
	oPair1 = oCoords1.getTile(10,21);
	REQUIRE( oPair1.first );
	REQUIRE( oPair1.second == Tile{});

	auto oCheck = [&](bool bPresent)
	{
		for (int32_t nX = 0; nX < 5; ++nX) {
			for (int32_t nY = 0; nY < 4; ++nY) {
				auto oPair2 = oCoords1.getTile(10 + nX, 20 + nY);
				if ((nX == 0) && (nY == 1)) {
					if (bPresent) {
						REQUIRE( oPair2.first );
						REQUIRE( oPair2.second == Tile{});
					} else {
						REQUIRE( ! oPair2.first );
					}
					continue;
				}
				Tile oTile2;
				oTile2.getTileColor().setColorIndex(100 + nX * 10 + nY);
				REQUIRE( oPair2.first );
				REQUIRE( oPair2.second == oTile2);
			}
		}
	};
	oCheck(true);

	Tile oTile1;
	oTile1.getTileColor().setColorIndex(55);
	oCoords1.add(10,21, oTile1);
	REQUIRE( oCoords1.size() == 20 );
	oPair1 = oCoords1.getTile(10,21);
	REQUIRE( oPair1.first );
	REQUIRE( oPair1.second == oTile1);
	REQUIRE( ! (oPair1.second == Tile{}));

	oCoords1.add(10,21, Tile{});
	REQUIRE( oCoords1.size() == 20 );
	oPair1 = oCoords1.getTile(10,21);
	REQUIRE( oPair1.first );
	REQUIRE( oPair1.second == Tile{});

	oCheck(true);

	oCoords1.remove(10,21);
	REQUIRE( oCoords1.size() == 19 );

	oCheck(false);
}
TEST_CASE("testTileCoords, Iterator")
{
//std::cout << "testTileCoords::Iterator()" << '\n';
	TileCoords oCoords1;

	Tile oTile;
	oTile.getTileColor().setColorIndex(55);
	Tile oTile2;
	oTile2.getTileChar().setChar(88);
	Tile oTile3;
	oTile3.getTileAlpha().setAlpha(127);

	oCoords1.add(25,3, oTile);
	oCoords1.add(28,2, oTile2);
	oCoords1.add(22,4, oTile3);
	REQUIRE( oCoords1.size() == 3 );

	int32_t nCount = 0;
	for (TileCoords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		++nCount;
	}
	REQUIRE( nCount == 3 );

	nCount = 0;
	for (TileCoords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 25) && (itC1.x() == 25) && (itC1.getTile() == oTile)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	nCount = 0;
	for (TileCoords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 28) && (itC1.y() == 2) && (itC1.getTile() == oTile2)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	nCount = 0;
	for (TileCoords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 22) && (itC1.y() == 4) && (itC1.getTile() == oTile3)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	TileCoords::const_iterator itC1 = oCoords1.begin();
	itC1.next();
	TileCoords::const_iterator itC2 = itC1;
	REQUIRE( itC2 == itC1 );
	itC2.next();
	REQUIRE( itC2 != itC1 );
}

TEST_CASE("testTileCoords, IteratorCompare")
{
//std::cout << "testTileCoords::IteratorCompare()" << '\n';
	TileCoords oCoords1;
	{
		TileCoords::const_iterator itC1 = oCoords1.begin();
		REQUIRE( oCoords1.size() == 0 );
		REQUIRE( itC1 == itC1);
		REQUIRE( itC1 == oCoords1.begin() );
		REQUIRE( itC1 == oCoords1.end() );
	}

	Tile oTile;
	oTile.getTileColor().setColorIndex(55);
	oCoords1.add(25,3, oTile);
	oCoords1.add(28,2, oTile);
	oCoords1.add(22,4, oTile);
	REQUIRE( oCoords1.size() == 3 );

	TileCoords::const_iterator itC1 = oCoords1.begin();
	itC1.next();
	TileCoords::const_iterator itC2 = itC1;
	REQUIRE( itC2 == itC1 );
	itC2.next();
	REQUIRE( itC2 != itC1 );
	itC2.next();
	REQUIRE( itC2 == oCoords1.end() );
	REQUIRE( itC2 != oCoords1.begin() );
}

} // namespace testing

} // namespace stmg
