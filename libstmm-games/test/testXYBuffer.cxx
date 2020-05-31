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
 * File:   testXYBuffer.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/xybuffer.h"

#include <cassert>
#include <iostream>
 
namespace stmg
{

namespace testing
{

class BufData
{
public:
	BufData()
	: m_fValue(-1)
	{
	};
	explicit BufData(double fValue)
	: m_fValue(fValue)
	{
	};

	double m_fValue;
};

class BufDataNoC
{
public:
	explicit BufDataNoC(int32_t nValue)
	: m_nValue(nValue)
	{
	};

	int32_t m_nValue;
private:
	BufDataNoC();
};

TEST_CASE("testXYBuffer, Constructor")
{
	const int32_t nW = 5;
	const int32_t nH = 7;
	{
	XYBuffer<BufData> oBuf({nW,nH});

	REQUIRE( oBuf.getW() == nW );
	REQUIRE( oBuf.getH() == nH );

	REQUIRE( oBuf.get({0,0}).m_fValue == -1 );
	REQUIRE( oBuf.get({nW -1,nH -1}).m_fValue == -1 );
	}
	{
	XYBuffer<BufDataNoC> oBufC({nW,nH}, BufDataNoC(77));

	REQUIRE( oBufC.getW() == nW );
	REQUIRE( oBufC.getH() == nH );

	REQUIRE( oBufC.get({0,0}).m_nValue == 77 );
	REQUIRE( oBufC.get({nW -1,nH -1}).m_nValue == 77 );
	}
}

TEST_CASE("testXYBuffer, GetSet")
{
	const int32_t nW = 5;
	const int32_t nH = 7;
	{
	XYBuffer<BufData> oBuf({nW,nH});

	REQUIRE( oBuf.getW() == nW );
	REQUIRE( oBuf.getH() == nH );

	oBuf.set({3,2},BufData(111));
	REQUIRE( oBuf.get({3,2}).m_fValue == 111 );
	REQUIRE( oBuf.get({2,3}).m_fValue == -1 );
	REQUIRE( oBuf.get({nW -1, nH -1}).m_fValue == -1 );
	}
	{
	XYBuffer<BufDataNoC> oBufC({nW,nH},BufDataNoC(77));

	REQUIRE( oBufC.getW() == nW );
	REQUIRE( oBufC.getH() == nH );

	oBufC.set({3,2}, BufDataNoC(1212));
	REQUIRE( oBufC.get({3,2}).m_nValue == 1212 );
	REQUIRE( oBufC.get({2,3}).m_nValue == 77 );
	REQUIRE( oBufC.get({nW -1,nH -1}).m_nValue == 77 );

	oBufC.get({4,1}).m_nValue = 888;
	REQUIRE( oBufC.get({4,1}).m_nValue == 888 );
	}
}

} // namespace testing

} // namespace stmg
