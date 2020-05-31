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
 * File:   testRandomTiles.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "utile/randomtiles.h"

#include "stdrandomsource.h"
#include "traitsets/tiletraitsets.h"
#include "utile/tilerect.h"

namespace stmg
{

using std::unique_ptr;
using std::make_unique;

namespace testing
{

TEST_CASE("testRandomTiles, Construct")
{
	auto refCRTS = std::make_unique<ColorRgbTraitSet>(ColorRedTraitSet{24,26}, ColorGreenTraitSet{122,125}, ColorBlueTraitSet{33, 37, 4});
	auto refColorST = make_unique<ColorTraitSet>( std::move(refCRTS) );

	StdRandomSource oSource;
	RandomTiles::ProbTraitSets oProbTraitSets;
	oProbTraitSets.m_nProb = 33;
	oProbTraitSets.m_aTraitSets.push_back(std::move(refColorST));
	RandomTiles::ProbTileGen oProbTileGen;
	oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
	RandomTiles oRandomTiles(oSource, std::move(oProbTileGen));

	for (int32_t nCount = 0; nCount < 10; ++nCount) {
		Tile oTile = oRandomTiles.createTile();

		REQUIRE(! oTile.isEmpty());
		REQUIRE(oTile.getTileColor().getColorType() == TileColor::COLOR_TYPE_RGB);
		uint8_t nR, nG, nB;
		oTile.getTileColor().getColorRGB(nR, nG, nB);
		REQUIRE(nR >= 24);
		REQUIRE(nR <= 26);
		REQUIRE(nG >= 122);
		REQUIRE(nG <= 125);
		REQUIRE(((nB == 33) || (nB == 37)) == true);
	}
}
//
//TEST_CASE("testRandomTiles, SomeTiles")
//{
//	{
//	auto refCRTS = std::make_unique<ColorRgbTraitSet>(ColorRedTraitSet{24}, ColorGreenTraitSet{122,125}, ColorBlueTraitSet{33, 37, 4});
//	auto refColorST = make_unique<ColorTraitSet>( std::move(refCRTS) );
//
//	StdRandomSource oSource;
//	RandomTiles::ProbTraitSets oProbTraitSets;
//	oProbTraitSets.m_nProb = 33;
//	oProbTraitSets.m_aTraitSets.push_back(std::move(refColorST));
//	RandomTiles::ProbTileGen oProbTileGen;
//	oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
//	RandomTiles oRandomTiles(oSource, std::move(oProbTileGen));
//
//	const int32_t nBufferW = 20;
//	const int32_t nBufferH = 30;
//	const int32_t nGaps = 6;
//	TileBuffer oTileBuffer{NSize{nBufferW, nBufferH}};
//	oRandomTiles.createRandomTiles(oTileBuffer, nGaps);
//
//	REQUIRE(oTileBuffer.getW() == nBufferW);
//	REQUIRE(oTileBuffer.getH() == nBufferH);
//	REQUIRE_FALSE(TileBuffer::isAllEmptyTiles(oTileBuffer));
//	int32_t nCount = 0;
//	for (int32_t nX = 0; nX < nBufferW; ++nX) {
//		for (int32_t nY = 0; nY < nBufferH; ++nY) {
//			const Tile& oTile = oTileBuffer.get({nX, nY});
//			if (! oTile.isEmpty()) {
//				++nCount;
//				const auto& oTileColor = oTile.getTileColor();
//				REQUIRE(oTileColor.getColorType() == TileColor::COLOR_TYPE_RGB);
//				uint8_t nR, nG, nB;
//				oTileColor.getColorRGB(nR, nG, nB);
//				REQUIRE(nR == 24);
//				REQUIRE(((nG >= 122) && (nG <= 125)));
//				REQUIRE(((nB == 33) || (nB == 37) || (nB == 4)));
//			}
//		}
//	}
//	REQUIRE(nCount == (nBufferW * nBufferH - nGaps));
//	}
//}

} // namespace testing

} // namespace stmg
