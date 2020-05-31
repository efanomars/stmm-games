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
 * File:   testVariantSet.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "util/variantset.h"

#include <memory>
#include <cassert>
#include <iostream>
 
namespace stmg
{

namespace testing
{

TEST_CASE("testVariantSet, Constructor")
{
	{
//std::cout << "----- empty" << '\n';
	VariantSet oSet{};

	REQUIRE( oSet.size() == 0 );
	REQUIRE( ! oSet.contains(Variant{0}) );

	}
	{
	VariantSet oSet{Variant{5}};

	REQUIRE( oSet.size() == 1 );
	REQUIRE( ! oSet.contains(Variant{0}) );
	REQUIRE( oSet.contains(Variant{5}) );

	}
	{
	VariantSet oSet{Variant{5},Variant{2}};

	REQUIRE( oSet.size() == 4 );
	REQUIRE( ! oSet.contains(Variant{6}) );
	REQUIRE( oSet.contains(Variant{5}) );
	REQUIRE( oSet.contains(Variant{4}) );
	REQUIRE( oSet.contains(Variant{3}) );
	REQUIRE( oSet.contains(Variant{2}) );
	REQUIRE( ! oSet.contains(Variant{1}) );
	REQUIRE( ! oSet.contains(Variant{true}) );
	REQUIRE( ! oSet.contains(Variant{false}) );

	}
	{
	std::vector<Variant> aValues{Variant{-10},Variant{-7},Variant{-2},Variant{+8},Variant{+33}};
	VariantSet oSet{std::move(aValues)};

	REQUIRE( oSet.size() == 5 );
	REQUIRE( oSet.contains(Variant{-10}) );
	REQUIRE( oSet.contains(Variant{-7}) );
	REQUIRE( oSet.contains(Variant{-2}) );
	REQUIRE( oSet.contains(Variant{+8}) );
	REQUIRE( oSet.contains(Variant{+33}) );
	REQUIRE( ! oSet.contains(Variant{0}) );

	}
	{
	std::vector<Variant> aValues{Variant{-10},Variant{-10},Variant{5},Variant{10},Variant{-10}};
	VariantSet oSet{std::move(aValues)};

	REQUIRE( oSet.size() == 3 );
	REQUIRE( oSet.contains(Variant{-10}) );
	REQUIRE( oSet.contains(Variant{+5}) );
	REQUIRE( oSet.contains(Variant{+10}) );
	REQUIRE( ! oSet.contains(Variant{0}) );

	}
	{
	std::vector<Variant> aValues{Variant{true},Variant{false},Variant{false}};
	VariantSet oSet{std::move(aValues)};

	REQUIRE( oSet.size() == 2 );
	REQUIRE( oSet.contains(Variant{true}) );
	REQUIRE( oSet.contains(Variant{false}) );

	}
}

} // namespace testing

} // namespace stmg
