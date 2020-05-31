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
 * File:   testIntSet.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/intset.h"

#include <memory>
#include <cassert>
#include <iostream>
 
namespace stmg
{

namespace testing
{

TEST_CASE("testIntSet, Constructor")
{
	{
	IntSet oSet{};

	REQUIRE( oSet.size() == 0 );
	REQUIRE( oSet.getIndexOfValue(0) == -1 );

	}
	{
	IntSet oSet{5};

	REQUIRE( oSet.size() == 1 );
	REQUIRE( oSet.getIndexOfValue(0) == -1 );
	REQUIRE( oSet.getIndexOfValue(5) == 0 );

	}
	{
	IntSet oSet{5,2};

	REQUIRE( oSet.size() == 4 );
	REQUIRE( oSet.getIndexOfValue(6) == -1 );
	REQUIRE( oSet.getIndexOfValue(5) >= 0 );
	REQUIRE( oSet.getIndexOfValue(4) >= 0 );
	REQUIRE( oSet.getIndexOfValue(3) >= 0 );
	REQUIRE( oSet.getIndexOfValue(2) >= 0 );
	REQUIRE( oSet.getIndexOfValue(1) == -1 );

	}
	{
	IntSet oSet{-5,-2, 2};

	REQUIRE( oSet.size() == 2 );
	REQUIRE( oSet.getIndexOfValue(-6) == -1 );
	REQUIRE( oSet.getIndexOfValue(-5) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-4) == -1 );
	REQUIRE( oSet.getIndexOfValue(-3) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-2) == -1 );

	}
	{
	std::vector<int32_t> aValues{-10,-7,-2,+8,+33};
	IntSet oSet{std::move(aValues)};

	REQUIRE( oSet.size() == 5 );
	REQUIRE( oSet.getIndexOfValue(-10) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-7) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-2) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+8) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+33) >= 0 );
	REQUIRE( oSet.getIndexOfValue(0) == -1 );

	}
	{
	IntSet oSet{-5,-21, -5}; // -5 -10 -15 -20

	REQUIRE( oSet.size() == 4 );
	REQUIRE( oSet.getIndexOfValue(-5) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-10) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-15) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-20) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-25) == -1 );

	}
	{
	std::vector<int32_t> aValues{-10,-10,5,10,-10};
	IntSet oSet{std::move(aValues)};

	REQUIRE( oSet.size() == 3 );
	REQUIRE( oSet.getIndexOfValue(-10) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+5) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+10) >= 0 );
	REQUIRE( oSet.getIndexOfValue(0) == -1 );

	}
}
TEST_CASE("testIntSet, AsVector")
{
	{
	std::vector<int32_t> aValues{-10, 5, 20, 5, 10};
	IntSet oSet{std::move(aValues)};

	const std::vector<int32_t> aRes = oSet.getAsVector();
	REQUIRE( aRes.size() == 4 );
	REQUIRE( aRes[0] == -10 );
	REQUIRE( aRes[1] == 5 );
	REQUIRE( aRes[2] == 10 );
	REQUIRE( aRes[3] == 20 );
	}
}
TEST_CASE("testIntSet, Difference")
{
	{
	std::vector<int32_t> aValues{-10,-7,-2,+8,+33};
	IntSet oSet{std::move(aValues)};

	IntSet oSetE{-5,-21, -5}; // -5 -10 -15 -20

	oSet = IntSet::makeDifference(oSet, oSetE);

	REQUIRE( oSet.size() == 4 );
	REQUIRE( oSet.getIndexOfValue(-10) == -1 );
	REQUIRE( oSet.getIndexOfValue(-7) >= 0 );
	REQUIRE( oSet.getIndexOfValue(-2) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+8) >= 0 );
	REQUIRE( oSet.getIndexOfValue(+33) >= 0 );

	}
}

} // namespace testing

} // namespace stmg
