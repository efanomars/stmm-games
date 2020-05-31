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
 * File:   testNamedIndex.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/namedindex.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testNamedIndex, Constructor")
{
//std::cout << "testNamedIndex::Constructor()" << '\n';
	NamedIndex oNamedIndex;
	REQUIRE( oNamedIndex.size() == 0 );
	REQUIRE_FALSE( oNamedIndex.isIndex(0) );

	NamedIndex oNamedIndex2(oNamedIndex);
	NamedIndex oNamedIndex3(std::move(oNamedIndex));
}

TEST_CASE("testNamedIndex, Add")
{
	NamedIndex oNamedIndex;
	const int32_t nIdx = oNamedIndex.addName("test");
	REQUIRE( nIdx == 0 );
	REQUIRE( oNamedIndex.size() == 1 );
	REQUIRE( oNamedIndex.getIndex("test") == nIdx);
	const int32_t nIdx2 = oNamedIndex.addName("test2");
	REQUIRE( nIdx2 == 1 );
	REQUIRE( oNamedIndex.size() == 2 );
	REQUIRE( oNamedIndex.getIndex("test2") == nIdx2);
	REQUIRE( oNamedIndex.getIndex("test") == nIdx);
	const int32_t nIdx3 = oNamedIndex.addName("test");
	REQUIRE( nIdx3 == nIdx );
	REQUIRE( oNamedIndex.size() == 2 );
}

TEST_CASE("testNamedIndex, FindNameStarts")
{
	NamedIndex oNamedIndex;
	const int32_t nIdx = oNamedIndex.addName("test");
	const int32_t nIdx2 = oNamedIndex.addName("test2");
	const int32_t nIdx3 = oNamedIndex.addName("__test3");
	const int32_t nIdx4 = oNamedIndex.addName("__tost4");
	REQUIRE( oNamedIndex.findIndexNameStarts("test") == nIdx);
	REQUIRE( oNamedIndex.findIndexNameStarts("test2") == nIdx2);
	REQUIRE( oNamedIndex.findIndexNameStarts("__") == nIdx3);
	REQUIRE( oNamedIndex.findIndexNameStarts("__tost") == nIdx4);
}

} // namespace testing

} // namespace stmg
