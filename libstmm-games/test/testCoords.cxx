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
 * File:   testCoords.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/coords.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testCoords, Constructor")
{
//std::cout << "testCoords::Constructor()" << '\n';
	{
		Coords oCoords(100);
		REQUIRE( oCoords.size() == 0 );

		oCoords.reInit();
		REQUIRE( oCoords.size() == 0 );
	}
	Coords oCoords;
	REQUIRE( oCoords.size() == 0 );

	oCoords.reInit();
	REQUIRE( oCoords.size() == 0 );

	oCoords.reInit(10);
	REQUIRE( oCoords.size() == 0 );
}

TEST_CASE("testCoords, Add")
{
//std::cout << "testCoords::Add()" << '\n';
	Coords oCoords;

	oCoords.add(10,20);
	REQUIRE( oCoords.size() == 1 );
	REQUIRE( oCoords.contains(10,20) );
	REQUIRE( !oCoords.contains(20,10) );
	oCoords.add(10,20);
	REQUIRE( oCoords.size() == 1 );
	REQUIRE( oCoords.contains(10,20) );
	REQUIRE( !oCoords.contains(20,10) );

	NRect oRect;
	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 10 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 1 );
	REQUIRE( oRect.m_nH == 1 );

	oCoords.add(11,21);
	REQUIRE( oCoords.size() == 2 );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 10 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 2 );
	REQUIRE( oRect.m_nH == 2 );

	oCoords.addRect(7,22,6,2);
	REQUIRE( oCoords.size() == 2 + 6*2);
	REQUIRE( !oCoords.contains(6,22) );
	REQUIRE( !oCoords.contains(7,24) );
	REQUIRE( !oCoords.contains(7,21) );
	REQUIRE( oCoords.contains(7,22) );
	REQUIRE( oCoords.contains(7,23) );
	REQUIRE( oCoords.contains(8,22) );
	REQUIRE( oCoords.contains(12,22) );
	REQUIRE( oCoords.contains(12,23) );
	REQUIRE( !oCoords.contains(13,22) );
	REQUIRE( !oCoords.contains(12,24) );

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

	oCoords.add(8,22);
	REQUIRE( oCoords.size() == 2 + 6*2);
	REQUIRE( oCoords.contains(8,22) );

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 20 );
	REQUIRE( oRect.m_nW == 6 );
	REQUIRE( oRect.m_nH == 4 );

	oCoords.remove(8,22);
	REQUIRE( oCoords.size() == 2 + 6*2 - 1);
	REQUIRE( !oCoords.contains(8,22) );

	oCoords.removeInRect(10,20,4,4);
	REQUIRE( oCoords.size() == 5);

	oRect = oCoords.getMinMax();
	REQUIRE( oRect.m_nX == 7 );
	REQUIRE( oRect.m_nY == 22 );
	REQUIRE( oRect.m_nW == 3 );
	REQUIRE( oRect.m_nH == 2 );
}

TEST_CASE("testCoords, Subtract")
{
//std::cout << "testCoords::Subtract()" << '\n';
	Coords oCoords1;
	Coords oCoords2;

	oCoords1.addRect(10,20,5,4);
	REQUIRE( oCoords1.size() == 20 );

	oCoords2.addRect(11,21,5,4);
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
TEST_CASE("testCoords, SubtractCoords")
{
	Coords oCoords1;

	oCoords1.addRect(10,20,5,4);
	REQUIRE( oCoords1.size() == 20 );

	Coords oCoords2;
	oCoords2.addRect(12,22,3,2);
	REQUIRE( oCoords2.size() == 6 );

	oCoords1.remove(oCoords2);
	REQUIRE( oCoords1.size() == 14 );
}
TEST_CASE("testCoords, AddCoords")
{
	Coords oCoords1;

	oCoords1.addRect(10,20,5,4);
	REQUIRE( oCoords1.size() == 20 );

	Coords oCoords2;
	oCoords2.addRect(12,22,3,2);
	REQUIRE( oCoords2.size() == 6 );

	oCoords1.add(oCoords2);
	REQUIRE( oCoords1.size() == 20 );

	Coords oCoords3;
	oCoords1.addRect(9,20,2,4);
	oCoords1.add(oCoords3);
	REQUIRE( oCoords1.size() == 20 + 4 );

	oCoords1.removeInRect(10,20,5,4);
	REQUIRE( oCoords1.size() == 4 );
}

TEST_CASE("testCoords, Iterator")
{
//std::cout << "testCoords::Iterator()" << '\n';
	Coords oCoords1;

	oCoords1.add(25,3);
	oCoords1.add(28,2);
	oCoords1.add(22,4);
	REQUIRE( oCoords1.size() == 3 );

	int32_t nCount = 0;
	for (Coords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		++nCount;
	}
	REQUIRE( nCount == 3 );

	nCount = 0;
	for (Coords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 25) && (itC1.y() == 3)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	nCount = 0;
	for (Coords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 28) && (itC1.y() == 2)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	nCount = 0;
	for (Coords::const_iterator itC1 = oCoords1.begin(); itC1 != oCoords1.end(); itC1.next()) {
		if ((itC1.x() == 22) && (itC1.y() == 4)) {
			++nCount;
		}
	}
	REQUIRE( nCount == 1 );

	Coords::const_iterator itC1 = oCoords1.begin();
	itC1.next();
	Coords::const_iterator itC2 = itC1;
	REQUIRE( itC2 == itC1 );
	itC2.next();
	REQUIRE( itC2 != itC1 );
}

TEST_CASE("testCoords, IteratorCompare")
{
//std::cout << "testCoords::IteratorCompare()" << '\n';
	Coords oCoords1;
	{
		Coords::const_iterator itC1 = oCoords1.begin();
		REQUIRE( oCoords1.size() == 0 );
		REQUIRE( itC1 == itC1);
		REQUIRE( itC1 == oCoords1.begin() );
		REQUIRE( itC1 == oCoords1.end() );
	}

	oCoords1.add(25,3);
	oCoords1.add(28,2);
	oCoords1.add(22,4);
	REQUIRE( oCoords1.size() == 3 );

	Coords::const_iterator itC1 = oCoords1.begin();
	itC1.next();
	Coords::const_iterator itC2 = itC1;
	REQUIRE( itC2 == itC1 );
	itC2.next();
	REQUIRE( itC2 != itC1 );
	itC2.next();
	REQUIRE( itC2 == oCoords1.end() );
	REQUIRE( itC2 != oCoords1.begin() );
}

} // namespace testing

} // namespace stmg
