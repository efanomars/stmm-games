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
 * File:   testNamedObjIndex.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/namedobjindex.h"

namespace stmg
{

namespace testing
{

TEST_CASE("testNamedObjIndex, Constructor")
{
//std::cout << "testNamedObjIndex::Constructor()" << '\n';
	NamedObjIndex<int32_t> oNamedObjIndex;
	REQUIRE( oNamedObjIndex.size() == 0 );
	REQUIRE_FALSE( oNamedObjIndex.isIndex(0) );

	NamedObjIndex<int32_t> oNamedObjIndex2(oNamedObjIndex);
	NamedObjIndex<int32_t> oNamedObjIndex3(std::move(oNamedObjIndex));
}

TEST_CASE("testNamedObjIndex, Add")
{
	NamedObjIndex<int32_t> oNamedObjIndex;
	const int32_t nIdx = oNamedObjIndex.addNamedObj("test", 876);
	REQUIRE( nIdx == 0 );
	REQUIRE( oNamedObjIndex.size() == 1 );
	REQUIRE( oNamedObjIndex.getIndex("test") == nIdx);
	REQUIRE( oNamedObjIndex.getObj(nIdx) == 876);
	const int32_t nIdx2 = oNamedObjIndex.addNamedObj("test2", 111);
	REQUIRE( nIdx2 == 1 );
	REQUIRE( oNamedObjIndex.size() == 2 );
	REQUIRE( oNamedObjIndex.getIndex("test2") == nIdx2);
	REQUIRE( oNamedObjIndex.getIndex("test") == nIdx);
	const int32_t nIdx3 = oNamedObjIndex.addNamedObj("test", 333);
	REQUIRE( nIdx3 == nIdx );
	REQUIRE( oNamedObjIndex.size() == 2 );
}

TEST_CASE("testNamedObjIndex, FindNameStarts")
{
	NamedObjIndex<int32_t> oNamedObjIndex;
	const int32_t nIdx = oNamedObjIndex.addNamedObj("test", 876);
	const int32_t nIdx2 = oNamedObjIndex.addNamedObj("test2", 111);
	const int32_t nIdx3 = oNamedObjIndex.addNamedObj("__test3", 333);
	const int32_t nIdx4 = oNamedObjIndex.addNamedObj("__tost4", 333);
	REQUIRE( oNamedObjIndex.findIndexNameStarts("test") == nIdx);
	REQUIRE( oNamedObjIndex.findIndexNameStarts("test2") == nIdx2);
	REQUIRE( oNamedObjIndex.findIndexNameStarts("__") == nIdx3);
	REQUIRE( oNamedObjIndex.findIndexNameStarts("__tost") == nIdx4);
}

TEST_CASE("testAssignableNamedObjIndex, Constructor")
{
//std::cout << "testNamedObjIndex::Constructor()" << '\n';
	AssignableNamedObjIndex<int32_t> oNamedObjIndex;
	REQUIRE( oNamedObjIndex.size() == 0 );
	REQUIRE_FALSE( oNamedObjIndex.isIndex(0) );
}

TEST_CASE("testAssignableNamedObjIndex, AddAndClear")
{
	AssignableNamedObjIndex<int32_t> oNamedObjIndex;
	const int32_t nIdx = oNamedObjIndex.addNamedObj("test", 876);
	REQUIRE( nIdx == 0 );
	REQUIRE( oNamedObjIndex.size() == 1 );
	REQUIRE( oNamedObjIndex.getIndex("test") == nIdx);
	REQUIRE( oNamedObjIndex.getObj(nIdx) == 876);
	const int32_t nIdx2 = oNamedObjIndex.addNamedObj("test2", 111);
	REQUIRE( nIdx2 == 1 );
	REQUIRE( oNamedObjIndex.size() == 2 );
	REQUIRE( oNamedObjIndex.getIndex("test2") == nIdx2);
	REQUIRE( oNamedObjIndex.getIndex("test") == nIdx);
	const int32_t nIdx3 = oNamedObjIndex.addNamedObj("test", 333);
	REQUIRE( nIdx3 == nIdx );
	REQUIRE( oNamedObjIndex.size() == 2 );
	oNamedObjIndex.clear();
	REQUIRE( oNamedObjIndex.size() == 0 );
}

} // namespace testing

} // namespace stmg
