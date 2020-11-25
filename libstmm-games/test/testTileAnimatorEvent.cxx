/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testTileAnimatorEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/tileanimatorevent.h"

#include "traitsets/tiletraitsets.h"

#include "events/logevent.h"

#include "utile/tilebuffer.h"

#include "stmm-games-fake/fixtureGame.h"
#include "stmm-games-fake/mockevent.h"
#include "stmm-games-fake/dumbblockevent.h"
#include "stmm-games-fake/fakelevelview.h"

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class TileAnimatorEventBaseGameFixture : public GameFixture
							//default , public FixtureVariantDevicesKeys_Two, public FixtureVariantDevicesJoystick_Two
							, public FixtureVariantPrefsTeams<1>
							//default , public FixtureVariantPrefsMates<0,2>
							//default , public FixtureVariantMatesPerTeamMax_Three, public FixtureVariantAIMatesPerTeamMax_Zero
							//default , public FixtureVariantAllowMixedAIHumanTeam_False, public FixtureVariantPlayersMax_Six
							//default , public FixtureVariantTeamsMin_One, public FixtureVariantTeamsMax_Two
							//default , public FixtureVariantKeyActions_AllCapabilityClassesDefaults
							//default , public FixtureVariantLayoutTeamDistribution_AllTeamsInOneLevel
							//default , public FixtureVariantLayoutShowMode_Show
							//default , public FixtureVariantLayoutCreateVarWidgetsFromVariables_False
							//default , public FixtureVariantLayoutCreateActionWidgetsFromKeyActions_False
							, public FixtureVariantVariablesGame_Time
							//, public FixtureVariantVariablesTeam
							, public FixtureVariantVariablesPlayer_Lives<3>
							//default , public FixtureVariantLevelInitBoardWidth<10>
							//default , public FixtureVariantLevelInitBoardHeight<6>
							//default , public FixtureVariantLevelInitShowWidth<10>
							//default , public FixtureVariantLevelInitShowHeight<6>
{
protected:
	void setup() override
	{
		GameFixture::setup();
	}
	void teardown() override
	{
		GameFixture::teardown();
	}
};

class TAEOneTileGameFixture : public TileAnimatorEventBaseGameFixture
{
	void fillBoard(int32_t nBoardW, int32_t nBoardH, std::vector<Tile>& aBoard) override
	{
		assert(nBoardW > 0);
		assert(nBoardH > 0);
		assert(static_cast<int32_t>(aBoard.size()) >= nBoardW * nBoardH);
		Tile oTile;
		oTile.getTileChar().setChar(65);
		{
		const int32_t nX = nBoardW - 1;
		const int32_t nY = nBoardH - 1;
		aBoard[Level::Init::getBoardIndex(NPoint{nX, nY}, NSize{nBoardW, nBoardH})] = oTile;
		}

		oTile.getTileChar().setChar(66);
		{
		const int32_t nX = 0;
		const int32_t nY = 0;
		aBoard[Level::Init::getBoardIndex(NPoint{nX, nY}, NSize{nBoardW, nBoardH})] = oTile;
		}
	}
};

class TAEManyTileGameFixture : public TileAnimatorEventBaseGameFixture
							, public FixtureVariantLevelInitBoardWidth<10>
							, public FixtureVariantLevelInitBoardHeight<12>
{
	void fillBoard(int32_t nBoardW, int32_t nBoardH, std::vector<Tile>& aBoard) override
	{
		assert(nBoardW > 0);
		assert(nBoardH > 0);
		assert(static_cast<int32_t>(aBoard.size()) >= nBoardW * nBoardH);
		Tile oTile1;
		oTile1.getTileChar().setChar(65);
		Tile oTile2;
		oTile2.getTileColor().setColorPal(99);
		int32_t nC = 0;
		for (int32_t nY = 2; nY < 8; ++nY) {
			for (int32_t nX = 3; nX < 7; ++nX) {
				aBoard[Level::Init::getBoardIndex(NPoint{nX, nY}, NSize{nBoardW, nBoardH})] = (((nC + nY) % 2 == 0) ? oTile1 : oTile2);
				++nC;
			}
		}
	}
};

TEST_CASE_METHOD(STFX<TAEOneTileGameFixture>, "Constructor")
{
	REQUIRE_FALSE( m_refGame->isRunning() );
	auto& refLevel = m_refGame->level(0);
	assert(refLevel);
	REQUIRE( refLevel->boardWidth() == 10 );
	REQUIRE( refLevel->boardHeight() == 6 );
	Level* p0Level = refLevel.get();
	FakeLevelView oFakeLevelView(m_refGame.get(), p0Level);
	TileAnimatorEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	const int32_t nTileAniCharA = m_refGame->getNamed().tileAnis().addName("TestTileAni");
	oInit.m_nAniNameIdx = nTileAniCharA;
	oInit.m_oDuration.m_oTicks.m_nFrom = 10;
	oInit.m_oDuration.m_oTicks.m_nTo = 10;
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(65));
	oInit.m_refSelect = make_unique<TileSelector>(make_unique<TileSelector::Trait>(false, std::move(refCTS)));
	auto refTileAnimatorEvent = make_unique<TileAnimatorEvent>(std::move(oInit));
	TileAnimatorEvent* p0TileAnimatorEvent = refTileAnimatorEvent.get();
	p0Level->addEvent(std::move(refTileAnimatorEvent));
	p0Level->activateEvent(p0TileAnimatorEvent, 1);
	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	const TileAnimator* p0TA = p0Level->boardGetTileAnimator(9, 5, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	const auto aCalls = oFakeLevelView.getCalled<FakeLevelView::BoardAnimateTiles>();
	REQUIRE( aCalls.size() == 1 );
	REQUIRE( aCalls[0]->m_oArea.m_nX == 9 );
	REQUIRE( aCalls[0]->m_oArea.m_nY == 5 );
	REQUIRE( aCalls[0]->m_oArea.m_nW == 1 );
	REQUIRE( aCalls[0]->m_oArea.m_nH == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	//const TileAnimator* p0TA2 = p0Level->boardGetTileAnimator(9, 5, nTileAniCharA);
	//REQUIRE( p0TA2 == nullptr );
}

TEST_CASE_METHOD(STFX<TAEOneTileGameFixture>, "Block")
{
	REQUIRE_FALSE( m_refGame->isRunning() );
	auto& refLevel = m_refGame->level(0);
	assert(refLevel);
	assert( refLevel->boardWidth() == 10 );
	assert( refLevel->boardHeight() == 6 );
	Level* p0Level = refLevel.get();
	TileAnimatorEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	const int32_t nTileAniCharA = m_refGame->getNamed().tileAnis().addName("TestTileAni");
	oInit.m_nAniNameIdx = nTileAniCharA;
	oInit.m_oDuration.m_oTicks.m_nFrom = 10;
	oInit.m_oDuration.m_oTicks.m_nTo = 10;
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(65));
	//TileSelector oTS{make_unique<TileSelector::Trait>(false, std::move(refCTS))};
	oInit.m_refSelect = make_unique<TileSelector>(make_unique<TileSelector::Trait>(false, std::move(refCTS)));
	auto refTileAnimatorEvent = make_unique<TileAnimatorEvent>(std::move(oInit));
	TileAnimatorEvent* p0TileAnimatorEvent = refTileAnimatorEvent.get();
	p0Level->addEvent(std::move(refTileAnimatorEvent));
	p0Level->activateEvent(p0TileAnimatorEvent, 1);
	//
	Block oBlock;
	Tile oTile;
	oTile.getTileChar().setChar(65);
	const int32_t nBrickId = oBlock.brickAdd(oTile, 1, 1, true);
	assert(!oBlock.isEmpty());
	DumbBlockEvent::Init oDInit;
	oDInit.m_p0Level = p0Level;
	oDInit.m_oBlock = std::move(oBlock);
	oDInit.m_oInitPos = NPoint{3,4};
	auto refDumbBlockEvent = make_unique<DumbBlockEvent>(std::move(oDInit));
	DumbBlockEvent* p0DumbBlockEvent = refDumbBlockEvent.get();
	p0Level->addEvent(std::move(refDumbBlockEvent));
	p0Level->activateEvent(p0DumbBlockEvent, 1);
	//
	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );
	auto aBlocks = p0Level->blocksGetAll();
	REQUIRE( aBlocks.size() == 0);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	aBlocks = p0Level->blocksGetAll();
	REQUIRE( aBlocks.size() == 0);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	aBlocks = p0Level->blocksGetAll();
	REQUIRE( aBlocks.size() == 1);

	LevelBlock* p0LB = aBlocks[0];
	REQUIRE( p0LB != nullptr );
	REQUIRE( p0LB == p0DumbBlockEvent );
	const TileAnimator* p0TA = p0LB->blockGetTileAnimator(nBrickId, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
}

TEST_CASE_METHOD(STFX<TAEManyTileGameFixture>, "InsertUp")
{
	assert( ! m_refGame->isRunning() );
	auto& refLevel = m_refGame->level(0);
	assert(refLevel);
	assert( refLevel->boardWidth() == 10 );
	assert( refLevel->boardHeight() == 12 );
	Level* p0Level = refLevel.get();
	//FakeLevelView oFakeLevelView(m_refGame.get(), p0Level);
	TileAnimatorEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	const int32_t nTileAniCharA = m_refGame->getNamed().tileAnis().addName("TestTileAni");
	oInit.m_nAniNameIdx = nTileAniCharA;
	oInit.m_oDuration.m_oTicks.m_nFrom = 10;
	oInit.m_oDuration.m_oTicks.m_nTo = 10;
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(65));
	oInit.m_refSelect = make_unique<TileSelector>(make_unique<TileSelector::Trait>(false, std::move(refCTS)));
	auto refTileAnimatorEvent = make_unique<TileAnimatorEvent>(std::move(oInit));
	TileAnimatorEvent* p0TileAnimatorEvent = refTileAnimatorEvent.get();
	p0Level->addEvent(std::move(refTileAnimatorEvent));
	p0Level->activateEvent(p0TileAnimatorEvent, 1);

	NRect oArea;
	oArea.m_nX = 3;
	oArea.m_nY = 2;
	oArea.m_nW = 4;
	oArea.m_nH = 6;
	const shared_ptr<TileBuffer>& refTiles = std::make_shared<TileBuffer>(NSize{4, 1});
	Tile oTile1;
	oTile1.getTileChar().setChar(65);
	refTiles->setAll(oTile1);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit), [&](Level& oLevel)
	{
		oLevel.boardInsert(Direction::UP, oArea, refTiles);
	});
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));
	p0Level->activateEvent(p0MockEvent, 3);

	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	const TileAnimator* p0TA = p0Level->boardGetTileAnimator(3, 2, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 2, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
	p0TA = p0Level->boardGetTileAnimator(5, 2, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(3, 3, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 3, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
//	const auto aCalls = oFakeLevelView.getCalled<FakeLevelView::BoardAnimateTiles>();
//	REQUIRE( aCalls.size() == 1 );
//	REQUIRE( aCalls[0]->m_oArea.m_nX == 9 );
//	REQUIRE( aCalls[0]->m_oArea.m_nY == 5 );
//	REQUIRE( aCalls[0]->m_oArea.m_nW == 1 );
//	REQUIRE( aCalls[0]->m_oArea.m_nH == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 4 );
	// the insert should have taken place
	p0TA = p0Level->boardGetTileAnimator(3, 2, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 2, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(5, 2, nTileAniCharA);
	REQUIRE( p0TA == nullptr );

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 5 );
//	m_refGame->handleTimer();
//	REQUIRE( m_refGame->gameElapsed() == 6 );
	p0TA = p0Level->boardGetTileAnimator(3, 7, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 7, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(6, 7, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
}

TEST_CASE_METHOD(STFX<TAEManyTileGameFixture>, "InsertUpAnimatedSubArea")
{
	assert( ! m_refGame->isRunning() );
	auto& refLevel = m_refGame->level(0);
	assert(refLevel);
	assert( refLevel->boardWidth() == 10 );
	assert( refLevel->boardHeight() == 12 );
	Level* p0Level = refLevel.get();
	//FakeLevelView oFakeLevelView(m_refGame.get(), p0Level);
	TileAnimatorEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	const int32_t nTileAniCharA = m_refGame->getNamed().tileAnis().addName("TestTileAni");
	oInit.m_nAniNameIdx = nTileAniCharA;
	oInit.m_oArea = NRect{3,5, 4,3};
	oInit.m_oDuration.m_oTicks.m_nFrom = 100;
	oInit.m_oDuration.m_oTicks.m_nTo = 100;
	auto refCTS = make_unique<CharTraitSet>(make_unique<CharUcs4TraitSet>(65));
	oInit.m_refSelect = make_unique<TileSelector>(make_unique<TileSelector::Trait>(false, std::move(refCTS)));
	auto refTileAnimatorEvent = make_unique<TileAnimatorEvent>(std::move(oInit));
	TileAnimatorEvent* p0TileAnimatorEvent = refTileAnimatorEvent.get();
	p0Level->addEvent(std::move(refTileAnimatorEvent));
	p0Level->activateEvent(p0TileAnimatorEvent, 0);

	NRect oArea;
	oArea.m_nX = 3;
	oArea.m_nY = 2;
	oArea.m_nW = 4;
	oArea.m_nH = 6;
	const shared_ptr<TileBuffer>& refTiles = std::make_shared<TileBuffer>(NSize{4, 1});
	Tile oTile1;
	oTile1.getTileChar().setChar(65);
	refTiles->setAll(oTile1);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit), [&](Level& oLevel)
	{
		oLevel.boardInsert(Direction::UP, oArea, refTiles);
	});
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));
	p0Level->activateEvent(p0MockEvent, 1);

	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	// the  tile animator is active
	const TileAnimator* p0TA = p0Level->boardGetTileAnimator(3, 2, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 5, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	// the insert hs taken place
	p0TA = p0Level->boardGetTileAnimator(4, 4, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
	p0TA = p0Level->boardGetTileAnimator(3, 5, nTileAniCharA);
	REQUIRE( p0TA != nullptr );
	p0TA = p0Level->boardGetTileAnimator(4, 5, nTileAniCharA);
	REQUIRE( p0TA == nullptr );
}

} // namespace testing

} // namespace stmg
