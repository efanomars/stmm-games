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
 * File:   testBasicTypes.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/basictypes.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testBasicTypes, NRectEquals")
{
//std::cout << "testBasicTypes::NRectEquals()" << '\n';
	NRect oR1;
	oR1.m_nX = 11;
	oR1.m_nY = 33;
	oR1.m_nW = 55;
	oR1.m_nH = 77;

	REQUIRE( oR1 == oR1 );

	NRect oR2;
	oR2.m_nX = 11;
	oR2.m_nY = 33;
	oR2.m_nW = 55;
	oR2.m_nH = 77;

	REQUIRE( oR1 == oR2 );

	NRect oR3;
	oR2.m_nX = 9;
	oR2.m_nY = 33;
	oR2.m_nW = 55;
	oR2.m_nH = 77;

	REQUIRE_FALSE( oR1 == oR3 );
}
TEST_CASE("testBasicTypes, NRectDoIntersect")
{
//std::cout << "testBasicTypes::NRectDoIntersect()" << '\n';
	NRect oR1;
	oR1.m_nX = 10;
	oR1.m_nY = 10;
	oR1.m_nW = 30;
	oR1.m_nH = 30;

	REQUIRE( NRect::doIntersect(oR1, oR1) );

	NRect oR2;
	oR2.m_nX = 20;
	oR2.m_nY = 20;
	oR2.m_nW = 5;
	oR2.m_nH = 5;

	REQUIRE( NRect::doIntersect(oR1, oR2) );

	NRect oR3;
	oR2.m_nX = 5;
	oR2.m_nY = 10;
	oR2.m_nW = 5;
	oR2.m_nH = 7;

	REQUIRE_FALSE( NRect::doIntersect(oR1, oR3) );
}
TEST_CASE("testBasicTypes, NRectIntersectionRect")
{
//std::cout << "testBasicTypes::NRectIntersectionRect()" << '\n';
	NRect oR1;
	oR1.m_nX = 10;
	oR1.m_nY = 10;
	oR1.m_nW = 30;
	oR1.m_nH = 30;

	REQUIRE( NRect::intersectionRect(oR1, oR1) == oR1 );

	{
	NRect oR2;
	oR2.m_nX = 20;
	oR2.m_nY = 20;
	oR2.m_nW = 30;
	oR2.m_nH = 5;

	NRect oRR;
	oRR.m_nX = 20;
	oRR.m_nY = 20;
	oRR.m_nW = 20;
	oRR.m_nH = 5;

	REQUIRE( NRect::intersectionRect(oR1, oR2) == oRR );
	}

	{
	NRect oR2;
	oR2.m_nX = -5;
	oR2.m_nY = -5;
	oR2.m_nW = 16;
	oR2.m_nH = 16;

	NRect oRR;
	oRR.m_nX = 10;
	oRR.m_nY = 10;
	oRR.m_nW = 1;
	oRR.m_nH = 1;

	REQUIRE( NRect::intersectionRect(oR1, oR2) == oRR );
	}

	{
	NRect oR2;
	oR2.m_nX = 40;
	oR2.m_nY = 40;
	oR2.m_nW = 30;
	oR2.m_nH = 5;

	REQUIRE( NRect::intersectionRect(oR1, oR2).m_nW == 0 );
	}
}
TEST_CASE("testBasicTypes, NRectBoundingRect")
{
//std::cout << "testBasicTypes::NRectBoundingRect()" << '\n';
	NRect oR1;
	oR1.m_nX = 10;
	oR1.m_nY = 10;
	oR1.m_nW = 30;
	oR1.m_nH = 30;

	REQUIRE( NRect::boundingRect(oR1, oR1) == oR1 );

	NRect oR2;
	oR2.m_nX = 20;
	oR2.m_nY = 20;
	oR2.m_nW = 30;
	oR2.m_nH = 5;

	NRect oRR;
	oRR.m_nX = 10;
	oRR.m_nY = 10;
	oRR.m_nW = 40;
	oRR.m_nH = 30;

	REQUIRE( NRect::boundingRect(oR1, oR2) == oRR );
}

} // namespace testing

} // namespace stmg
