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
 * File:   testNewRows.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "stdrandomsource.h"

#include "traitsets/tiletraitsets.h"
#include "utile/newrows.h"
#include "utile/tilerect.h"
#include "utile/tilebuffer.h"

namespace stmg
{

using std::unique_ptr;
using std::make_unique;

namespace testing
{

TEST_CASE("testNewRows, Construct")
{
	auto refCoRTS = std::make_unique<ColorRgbTraitSet>(ColorRedTraitSet{24,26}, ColorGreenTraitSet{122,125}, ColorBlueTraitSet{33, 37, 4});
	auto refColorTS = make_unique<ColorTraitSet>( std::move(refCoRTS) );

	NewRows::Init oInit;
	StdRandomSource oSource;
	{
	RandomTiles::ProbTraitSets oProbTraitSets;
	oProbTraitSets.m_nProb = 33;
	oProbTraitSets.m_aTraitSets.push_back(std::move(refColorTS));
	RandomTiles::ProbTileGen oProbTileGen;
	oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
	oInit.m_aRandomTiles.push_back(std::move(oProbTileGen));
	}
	//
	const int32_t nBufferW = 10;

	NewRows::NewRowGen oNewRowGen;
	{
	auto refDistr = std::make_unique<NewRows::DistrRandPosSingle>();
	NewRows::DistrRandPosSingle& oTilesDistr = *refDistr;
	oTilesDistr.m_nFill = 2;
	oTilesDistr.m_nLeaveEmpty = 3;
	oTilesDistr.m_oPositions = IntSet{0, nBufferW - 1};
	oTilesDistr.m_oTile.getTileColor().setColorIndex(55);
	oNewRowGen.m_aDistrs.push_back(std::move(refDistr));
	}
	oInit.m_aNewRowGens.push_back(std::move(oNewRowGen));
	NewRows oNewRows(oSource, std::move(oInit));

	TileBuffer oTileBuffer{NSize{nBufferW, 1}};
	oNewRows.createNewRow(0, oTileBuffer, 0);

	REQUIRE(oNewRows.getTotNewRowGens() == 1);
	REQUIRE(oNewRows.getTotRandomTileGens() == 1);
	REQUIRE(oTileBuffer.getW() == nBufferW);
	REQUIRE(oTileBuffer.getH() == 1);

	int32_t nNonEmpty = 0;
	for (int32_t nX = 0; nX < nBufferW; ++ nX) {
		Tile& oTile = oTileBuffer.get(NPoint{nX, 0});
		if (! oTile.isEmpty()) {
			++nNonEmpty;
		}
	}
	REQUIRE(nNonEmpty == 2);
}

TEST_CASE("testNewRows, Random")
{
	auto refCoRTS = std::make_unique<ColorRgbTraitSet>(ColorRedTraitSet{24,26}, ColorGreenTraitSet{122,125}, ColorBlueTraitSet{33, 37, 4});
	auto refColorTS = make_unique<ColorTraitSet>( std::move(refCoRTS) );

	auto refChITS = std::make_unique<CharIndexTraitSet>(0, 50, 10); // 0, 10, 20, 30, 40, 50
	auto refCharTS = make_unique<CharTraitSet>( std::move(refChITS) );

	NewRows::Init oInit;
	StdRandomSource oSource;
	{
	RandomTiles::ProbTraitSets oProbTraitSets;
	oProbTraitSets.m_nProb = 33;
	oProbTraitSets.m_aTraitSets.push_back(std::move(refColorTS));
	RandomTiles::ProbTileGen oProbTileGen;
	oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
	oInit.m_aRandomTiles.push_back(std::move(oProbTileGen));
	}
	{
	RandomTiles::ProbTraitSets oProbTraitSets;
	oProbTraitSets.m_nProb = 55;
	oProbTraitSets.m_aTraitSets.push_back(std::move(refCharTS));
	RandomTiles::ProbTileGen oProbTileGen;
	oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
	oInit.m_aRandomTiles.push_back(std::move(oProbTileGen));
	}
	//
	const int32_t nBufferW = 5;

	{
	NewRows::NewRowGen oNewRowGen;
	{
	auto refDistr = std::make_unique<NewRows::DistrRandPosSingle>();
	NewRows::DistrRandPosSingle& oTilesDistr = *refDistr;
	oTilesDistr.m_nFill = 2;
	oTilesDistr.m_nLeaveEmpty = 2;
	oTilesDistr.m_oPositions = IntSet{0, nBufferW - 1};
	oTilesDistr.m_oTile.getTileColor().setColorIndex(55);
	oNewRowGen.m_aDistrs.push_back(std::move(refDistr));
	}
	{
	auto refDistr = std::make_unique<NewRows::DistrRandPosTiles>();
	NewRows::DistrRandPosTiles& oTilesDistr = *refDistr;
	oTilesDistr.m_nFill = 2;
	oTilesDistr.m_nLeaveEmpty = 2;
	oTilesDistr.m_oPositions = IntSet{0, nBufferW - 1};
	oTilesDistr.m_nRandomTilesIdx = 0;
	oNewRowGen.m_aDistrs.push_back(std::move(refDistr));
	}
	oInit.m_aNewRowGens.push_back(std::move(oNewRowGen));
	}

	{
	NewRows::NewRowGen oNewRowGen;
	{
	auto refDistr = std::make_unique<NewRows::DistrRandTiles>();
	NewRows::DistrRandTiles& oTilesDistr = *refDistr;
	oTilesDistr.m_nFill = -1;
	oTilesDistr.m_nLeaveEmpty = 2;
	oTilesDistr.m_nRandomTilesIdx = 1;
	oNewRowGen.m_aDistrs.push_back(std::move(refDistr));
	}
	oInit.m_aNewRowGens.push_back(std::move(oNewRowGen));
	}

	NewRows oNewRows(oSource, std::move(oInit));

	TileBuffer oTileBuffer{NSize{nBufferW, 1}};

	oNewRows.createNewRow(0, oTileBuffer, 0);

	REQUIRE(oNewRows.getTotNewRowGens() == 2);
	REQUIRE(oNewRows.getTotRandomTileGens() == 2);
	REQUIRE(oTileBuffer.getW() == nBufferW);
	REQUIRE(oTileBuffer.getH() == 1);

	{
	int32_t nNonEmpty = 0;
	int32_t nColor55  = 0;
	for (int32_t nX = 0; nX < nBufferW; ++ nX) {
		Tile& oTile = oTileBuffer.get(NPoint{nX, 0});
		if (! oTile.isEmpty()) {
			++nNonEmpty;
			if (oTile.getTileColor().getColorType() == TileColor::COLOR_TYPE_INDEX) {
				++nColor55;
			}
		}
	}
	// first distr fills 2
	// second distr fills 1 because it has to leave 2 empty
	REQUIRE(nNonEmpty == 3);
	REQUIRE(nColor55 == 2);
	}

	oTileBuffer.setAll(Tile::s_oEmptyTile);

	oNewRows.createNewRow(1, oTileBuffer, 0);
	{
	int32_t nNonEmpty = 0;
	int32_t nChars = 0;
	for (int32_t nX = 0; nX < nBufferW; ++ nX) {
		Tile& oTile = oTileBuffer.get(NPoint{nX, 0});
		if (! oTile.isEmpty()) {
			++nNonEmpty;
			if (oTile.getTileChar().isCharIndex()) {
				++nChars;
			}
		}
	}
	REQUIRE(nNonEmpty == nBufferW - 2);
	REQUIRE(nChars == nBufferW - 2);
	}
}

} // namespace testing

} // namespace stmg
