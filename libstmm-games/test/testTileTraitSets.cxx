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
 * File:   testTileTraitSets.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "traitsets/tiletraitsets.h"

#include <memory>

namespace stmg
{

using std::make_unique;

namespace testing
{

TEST_CASE("testAlphaTraitSet, Constructor")
{
//std::cout << "testAlphaTraitSet::Constructor()" << '\n';
	{
	AlphaTraitSet oATS{{4,7,11}};
	REQUIRE( oATS.getTotValues() == 3 );
	}
	{
	AlphaTraitSet oATS{4,11,2}; // 4,6,8,10
	REQUIRE( oATS.getTotValues() == 4 );
	}
}
TEST_CASE("testAlphaTraitSet, EmptyValue")
{
	{
	AlphaTraitSet oATS{TileAlpha::ALPHA_MAX};
	REQUIRE( oATS.getTotValues() == 1 );
	REQUIRE_FALSE( oATS.hasEmptyValue() );
	}
	{
	AlphaTraitSet oATS{};
//std::cout << " oATS.getTotValues() = " << oATS.getTotValues() << '\n';
//std::cout << " oATS.hasEmptyValue() = " << oATS.hasEmptyValue() << '\n';
	REQUIRE( oATS.getTotValues() == 1 );
	REQUIRE( oATS.hasEmptyValue() );
	}
	{
	AlphaTraitSet oATS(IntSet{}, false);
//std::cout << " oATS.getTotValues() = " << oATS.getTotValues() << '\n';
//std::cout << " oATS.hasEmptyValue() = " << oATS.hasEmptyValue() << '\n';
	REQUIRE( oATS.getTotValues() == 0 );
	REQUIRE_FALSE( oATS.hasEmptyValue() );
	}
}

TEST_CASE("testFontTraitSet, Constructor")
{
	{
	FontTraitSet oFTS{67,69,2};
	REQUIRE( oFTS.getTotValues() == 2 );
	}
}
TEST_CASE("testFontTraitSet, EmptyValue")
{
	{
	FontTraitSet oFTS{0,56};
	REQUIRE( oFTS.getTotValues() == 57 );
	REQUIRE_FALSE( oFTS.hasEmptyValue() );
	}
	{
	FontTraitSet oFTS{};
	REQUIRE( oFTS.getTotValues() == 1 );
	REQUIRE( oFTS.hasEmptyValue() );
	}
	{
	FontTraitSet oFTS{std::vector<int32_t>{}};
	REQUIRE( oFTS.getTotValues() == 0 );
	REQUIRE_FALSE( oFTS.hasEmptyValue() );
	}
}

TEST_CASE("testColorTraitSet, Constructor")
{
	{
	ColorPalTraitSet oCPTS{1};
	REQUIRE( oCPTS.getTotValues() == 1 );
	}
	{
	ColorIndexTraitSet oCITS{1212};
	REQUIRE( oCITS.getTotValues() == 1 );
	}
	{
	ColorRedTraitSet oCRTS{255};
	REQUIRE( oCRTS.getTotValues() == 1 );
	}
	{
	ColorGreenTraitSet oCGTS{0};
	REQUIRE( oCGTS.getTotValues() == 1 );
	}
	{
	ColorBlueTraitSet oCBTS{0};
	REQUIRE( oCBTS.getTotValues() == 1 );
	}
	{
	ColorRgbTraitSet oCRTS{ColorRedTraitSet{24}, ColorGreenTraitSet{122}, ColorBlueTraitSet{33}};
	REQUIRE( oCRTS.getTotValues() == 1 );
	}
	{
	ColorTraitSet oCTS{};
	REQUIRE( oCTS.getTotValues() == 1 ); // The empty value is still one value
	}
	{
	auto refCRTS = std::make_unique<ColorRgbTraitSet>(ColorRedTraitSet{24}, ColorGreenTraitSet{122, 125}, ColorBlueTraitSet{33, 37, 4});
	ColorTraitSet oCTS{ std::move(refCRTS) };
	REQUIRE( oCTS.getTotValues() == 1 * 4 * 2 );
	}
	{
	ColorTraitSet oCTS{ std::make_unique<ColorPalTraitSet>(8) };
	REQUIRE( oCTS.getTotValues() == 1 );
	}
	{
	ColorTraitSet oCTS{ std::make_unique<ColorIndexTraitSet>(11010) };
	REQUIRE( oCTS.getTotValues() == 1 );
	}
	{
	std::vector<TileColor> aTileColors(4);
	aTileColors[0].setColorIndex(55);
	aTileColors[1].setColorRGB(41, 43, 7);
	aTileColors[2].setColorPal(3);
	aTileColors[3].setColorRGB(41, 43, 45);
	ColorTraitSet oCTS{ aTileColors };
	REQUIRE( oCTS.getTotValues() == 4 );
	}
}
TEST_CASE("testColorTraitSet, EmptyValue")
{
	{
	ColorTraitSet oCTS{ std::make_unique<ColorPalTraitSet>(0,15) };
	REQUIRE( oCTS.getTotValues() == 16 );
	REQUIRE_FALSE( oCTS.hasEmptyValue() );
	}
	{
	ColorTraitSet oCTS{};
	REQUIRE( oCTS.getTotValues() == 1 );
	REQUIRE( oCTS.hasEmptyValue() );
	}
}
TEST_CASE("testColorTraitSet, ColorRGBTraitSet")
{
	{
	std::vector<std::tuple<uint8_t, uint8_t, uint8_t>> aValues;
	aValues.push_back(std::make_tuple(39, 41, 111));
	aValues.push_back(std::make_tuple(31, 49, 111));
	aValues.push_back(std::make_tuple(31, 49, 115));
	aValues.push_back(std::make_tuple(1, 111, 115));
	ColorRgbTraitSet oCRTS{aValues};
	REQUIRE( oCRTS.getTotValues() == 4 );
	REQUIRE_FALSE( oCRTS.hasEmptyValue() );
	}
}

TEST_CASE("testCharTraitSet, Constructor")
{
	{
	CharIndexTraitSet oCITS{9999};
	REQUIRE( oCITS.getTotValues() == 1 );
	}
	{
	CharUcs4TraitSet oCUTS{1212};
	REQUIRE( oCUTS.getTotValues() == 1 );
	}
	{
	CharTraitSet oCTS{ std::make_unique<CharIndexTraitSet>(0) };
	REQUIRE( oCTS.getTotValues() == 1 );
	}
	{
	CharTraitSet oCTS{ std::make_unique<CharUcs4TraitSet>(121) };
	REQUIRE( oCTS.getTotValues() == 1 );
	}
}
TEST_CASE("testCharTraitSet, EmptyValue")
{
	{
	CharTraitSet oCTS{ std::make_unique<CharUcs4TraitSet>(std::vector<int32_t>{72,69,76,79}) };
	REQUIRE( oCTS.getTotValues() == 4 );
	REQUIRE_FALSE( oCTS.hasEmptyValue() );
	}
	{
	CharTraitSet oCTS{};
	REQUIRE( oCTS.getTotValues() == 1 );
	REQUIRE( oCTS.hasEmptyValue() );
	}
}

} // namespace testing

} // namespace stmg
