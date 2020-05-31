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
 * File:   testTileSelector.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "utile/tileselector.h"
#include "traitsets/tiletraitsets.h"

namespace stmg
{

using std::unique_ptr;
using std::make_unique;

namespace testing
{

TEST_CASE("testTileSelector, Constructor")
{
	TileSelector oTS{make_unique<TileSelector::Skin>(false, make_unique<IntSet>(2,4))};
}

TEST_CASE("testTileSelector, SkinSelect")
{
	{
	auto refSkin = make_unique<TileSelector::Skin>(false, make_unique<IntSet>(1,3));
	TileSelector oTS{std::move(refSkin)};
	Tile oTile;
	REQUIRE( oTS.select(oTile, 2) );
	}
	{
	auto refSkin = make_unique<TileSelector::Skin>(make_unique<IntSet>(1,3));
	TileSelector oTS{std::move(refSkin)};
	Tile oTile;
	REQUIRE( oTS.select(oTile, 2) );
	}
	{
	auto refSkin = make_unique<TileSelector::Skin>(true, make_unique<IntSet>(1,3));
	TileSelector oTS{std::move(refSkin)};
	Tile oTile;
	REQUIRE_FALSE( oTS.select(oTile, 2) );
	}
}

TEST_CASE("testTileSelector, TraitSelect")
{
	{
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(121));
	TileSelector oTS{make_unique<TileSelector::Trait>(false, std::move(refCTS))};
	Tile oTile;
	REQUIRE_FALSE( oTS.select(oTile) );
	REQUIRE_FALSE( oTS.select(oTile, 2) );
	}
	{
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(121));
	TileSelector oTS{make_unique<TileSelector::Trait>(std::move(refCTS))};
	Tile oTile;
	REQUIRE_FALSE( oTS.select(oTile) );
	REQUIRE_FALSE( oTS.select(oTile, 2) );
	}
	{
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(121));
	TileSelector oTS{make_unique<TileSelector::Trait>(true, std::move(refCTS))};
	Tile oTile;
	REQUIRE( oTS.select(oTile) );
	REQUIRE( oTS.select(oTile, 2) );
	}
	//
	{
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(121,125,2));
	TileSelector oTS{make_unique<TileSelector::Trait>(std::move(refCTS))};
	Tile oTile;
	oTile.getTileChar().setChar(123);
	REQUIRE( oTS.select(oTile) );
	REQUIRE( oTS.select(oTile, 2) );
	}
	{
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(121,125,2));
	TileSelector oTS{make_unique<TileSelector::Trait>(std::move(refCTS))};
	Tile oTile;
	oTile.getTileChar().setChar(124);
	REQUIRE_FALSE( oTS.select(oTile) );
	REQUIRE_FALSE( oTS.select(oTile, 2) );
	}
}
TEST_CASE("testTileSelector, OrOperator")
{
	{
	auto refCharTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(std::vector<int32_t>{1,3,6,7}));
	auto refTrait1 = make_unique<TileSelector::Trait>(std::move(refCharTS));
	auto refColorTS = make_unique<ColorTraitSet>(make_unique<ColorIndexTraitSet>(std::vector<int32_t>{2,3,4,8}));
	auto refTrait2 = make_unique<TileSelector::Trait>(std::move(refColorTS));
	std::vector< unique_ptr<TileSelector::Operand> > aOperands;
	aOperands.emplace_back(std::move(refTrait1));
	aOperands.emplace_back(std::move(refTrait2));
	auto refOr = make_unique<TileSelector::Operator>(TileSelector::Operator::OP_TYPE_OR, aOperands);
	TileSelector oTS{std::move(refOr)};
	Tile oTile;
	oTile.getTileChar().setCharIndex(6);
	REQUIRE( oTS.select(oTile) );
	oTile.getTileColor().setColorIndex(1);
	REQUIRE( oTS.select(oTile) );
	oTile.getTileChar().clear();
	REQUIRE_FALSE( oTS.select(oTile) );
	oTile.getTileChar().setCharIndex(4);
	REQUIRE_FALSE( oTS.select(oTile) );
	oTile.getTileColor().setColorIndex(8);
	REQUIRE( oTS.select(oTile) );
	}
}
TEST_CASE("testTileSelector, AndOperator")
{
	{
	auto refCharTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(std::vector<int32_t>{1,3,6,7}));
	auto refTrait1 = make_unique<TileSelector::Trait>(std::move(refCharTS));
	auto refColorTS = make_unique<ColorTraitSet>(make_unique<ColorIndexTraitSet>(std::vector<int32_t>{2,3,4,8}));
	auto refTrait2 = make_unique<TileSelector::Trait>(std::move(refColorTS));
	auto refTrait3 = make_unique<TileSelector::Skin>(make_unique<IntSet>(1,3));
	std::vector< unique_ptr<TileSelector::Operand> > aOperands;
	aOperands.emplace_back(std::move(refTrait1));
	aOperands.emplace_back(std::move(refTrait2));
	aOperands.emplace_back(std::move(refTrait3));
	auto refAnd = make_unique<TileSelector::Operator>(TileSelector::Operator::OP_TYPE_AND, aOperands);
	TileSelector oTS{std::move(refAnd)};
	Tile oTile;
	oTile.getTileChar().setCharIndex(6);
	REQUIRE_FALSE( oTS.select(oTile, 1) );
	oTile.getTileColor().setColorIndex(4);
	REQUIRE( oTS.select(oTile, 1) );
	REQUIRE( oTS.select(oTile) );
	REQUIRE_FALSE( oTS.select(oTile, 0) );
	oTile.getTileChar().clear();
	REQUIRE_FALSE( oTS.select(oTile) );
	oTile.getTileChar().setCharIndex(4);
	REQUIRE_FALSE( oTS.select(oTile) );
	}
}
TEST_CASE("testTileSelector, NotOperator")
{
	{
	auto refCharTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(std::vector<int32_t>{1,3,6,7}));
	unique_ptr<TileSelector::Operand> refTrait{make_unique<TileSelector::Trait>(std::move(refCharTS))};
	auto refNot = make_unique<TileSelector::Operator>(TileSelector::Operator::OP_TYPE_NOT, std::move(refTrait));
	TileSelector oTS{std::move(refNot)};
	Tile oTile;
	oTile.getTileChar().setCharIndex(6);
	REQUIRE_FALSE( oTS.select(oTile, 1) );
	oTile.getTileColor().setColorIndex(4);
	REQUIRE_FALSE( oTS.select(oTile, 1) );
	REQUIRE_FALSE( oTS.select(oTile) );
	oTile.getTileChar().clear();
	REQUIRE( oTS.select(oTile) );
	oTile.getTileChar().setCharIndex(4);
	REQUIRE( oTS.select(oTile) );
	}
}
TEST_CASE("testTileSelector, Partial")
{
	{
	auto refCharTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(3));
	unique_ptr<TileSelector::Operand> refTrait{make_unique<TileSelector::Trait>(std::move(refCharTS))};
	TileSelector oTS{std::move(refTrait)};
	Tile oTile;
	oTile.getTileChar().setCharIndex(6);
	oTile.getTileColor().setColorPal(7);
	REQUIRE_FALSE( oTS.select(oTile) );
	oTile.getTileChar().setCharIndex(3);
	REQUIRE( oTS.select(oTile) );
	}
}

} // namespace testing

} // namespace stmg
