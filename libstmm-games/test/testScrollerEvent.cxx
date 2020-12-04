/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   testScrollerEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/scrollerevent.h"

#include "utile/tileselector.h"

#include "stmm-games-fake/fixtureGame.h"
#include "stmm-games-fake/fakelevelview.h"
#include "stmm-games-fake/mockevent.h"

#include "events/sysevent.h"
#include "events/logevent.h"
#include "traitsets/tiletraitsets.h"

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class ScrollerGameFixture : public GameFixture
							//default , public FixtureVariantDevicesKeys_Two, public FixtureVariantDevicesJoystick_Two
							, public FixtureVariantPrefsTeams<1>
							, public FixtureVariantPrefsMates<0,1>
							//default , public FixtureVariantMatesPerTeamMax_Three, public FixtureVariantAIMatesPerTeamMax_Zero
							//default , public FixtureVariantAllowMixedAIHumanTeam_False, public FixtureVariantPlayersMax_Six
							//default , public FixtureVariantTeamsMin_One, public FixtureVariantTeamsMax_Two
							, public FixtureVariantKeyActions_Custom
							, public FixtureVariantOptions_Custom
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

		const int32_t nLevel = 0;
		m_refLevel = m_refGame->level(nLevel);
		assert(m_refLevel);
		assert( m_refLevel->boardWidth() == 10 );
		assert( m_refLevel->boardHeight() == 6 );
		Level* p0Level = m_refLevel.get();
		ScrollerEvent::Init oInit;
		oInit.m_p0Level = p0Level;
		oInit.m_nStep = getStep();
		oInit.m_nSlices = getSlices();
		oInit.m_aInhibitors = getInhibitors();
		oInit.m_nCheckNewRowTries = getCheckNewRowTries();
		//
		NewRows::Init oNRInit;
		NewRows::NewRowGen oNewRowGen;
		NewRows::DistrRandTiles oDistrRandTiles;
		oDistrRandTiles.m_nLeaveEmpty = 0;
		oDistrRandTiles.m_nRandomTilesIdx = 0;
		oNewRowGen.m_aDistrs.push_back(std::make_unique<NewRows::DistrRandTiles>(std::move(oDistrRandTiles)));
		oNRInit.m_aNewRowGens.push_back(std::move(oNewRowGen));

		auto refCTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(40, 46));

		RandomTiles::ProbTraitSets oProbTraitSets;
		oProbTraitSets.m_nProb = 10;
		oProbTraitSets.m_aTraitSets.push_back(std::move(refCTS));
		RandomTiles::ProbTileGen oProbTileGen;
		oProbTileGen.m_aProbTraitSets.push_back(std::move(oProbTraitSets));
		oNRInit.m_aRandomTiles.push_back(std::move(oProbTileGen));
		//
		oInit.m_refNewRows = std::make_unique<NewRows>(*m_refGame, std::move(oNRInit));

		auto refScrollerEvent = make_unique<ScrollerEvent>(std::move(oInit));
		m_p0ScrollerEvent = refScrollerEvent.get();
		p0Level->addEvent(std::move(refScrollerEvent));
	}
	void teardown() override
	{
		GameFixture::teardown();
	}

	virtual int32_t getPriority() const
	{
		return ScrollerEvent::Init{}.m_nPriority;
	}
	virtual int32_t getStep() const
	{
		return ScrollerEvent::LocalInit{}.m_nStep;
	}
	virtual int32_t getSlices() const
	{
		return ScrollerEvent::LocalInit{}.m_nSlices;
	}
	virtual std::vector< unique_ptr<TileSelector> > getInhibitors() const
	{
		return ScrollerEvent::LocalInit{}.m_aInhibitors;
	}
	virtual int32_t getCheckNewRowTries() const
	{
		return ScrollerEvent::LocalInit{}.m_nCheckNewRowTries;
	}

	std::vector< shared_ptr<Option> > getCustomOptions() const override
	{
		return std::vector< shared_ptr<Option> >{};
	}
	std::vector<StdConfig::KeyAction> getCustomKeyActions() const override
	{
		return std::vector<StdConfig::KeyAction>{};
	}
protected:
	shared_ptr<Level> m_refLevel;
	ScrollerEvent* m_p0ScrollerEvent;
};

class ScrollerCase1GameFixture : public ScrollerGameFixture
{
protected:
	void setup() override
	{
		ScrollerGameFixture::setup();
		Level* p0Level = m_refLevel.get();
		p0Level->activateEvent(m_p0ScrollerEvent, 0);
	}
	void teardown() override
	{
		ScrollerGameFixture::teardown();
	}
	void fillBoard(int32_t nBoardW, int32_t nBoardH, std::vector<Tile>& aBoard) override
	{
		{
			Tile oTile;
			oTile.getTileColor().setColorPal(4);
			const int32_t nY = nBoardH - 1;
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				aBoard[nX + nY * nBoardW] = oTile;
			}
		}
		{
			Tile oTile;
			oTile.getTileColor().setColorPal(5);
			const int32_t nY = 0;
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				aBoard[nX + nY * nBoardW] = oTile;
			}
		}
	}
};

class ScrollerCase2GameFixture : public ScrollerGameFixture
{
protected:
	void setup() override
	{
		ScrollerGameFixture::setup();
		Level* p0Level = m_refLevel.get();
		p0Level->activateEvent(m_p0ScrollerEvent, 0);
	}
	void teardown() override
	{
		ScrollerGameFixture::teardown();
	}
	void fillBoard(int32_t nBoardW, int32_t nBoardH, std::vector<Tile>& aBoard) override
	{
		{
			Tile oTile;
			oTile.getTileColor().setColorPal(4);
			const int32_t nY = nBoardH - 1;
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				aBoard[nX + nY * nBoardW] = oTile;
			}
		}
		{
			Tile oTile;
			oTile.getTileColor().setColorPal(5);
			const int32_t nY = 0;
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				aBoard[nX + nY * nBoardW] = oTile;
			}
		}
	}
};

class ScrollerEmptyBoardFixture : public ScrollerGameFixture
{
protected:
	void setup() override
	{
		ScrollerGameFixture::setup();
	}
	void teardown() override
	{
		ScrollerGameFixture::teardown();
	}
	void fillBoard(int32_t /*nBoardW*/, int32_t /*nBoardH*/, std::vector<Tile>& /*aBoard*/) override
	{
	}
	int32_t getSlices() const override
	{
		return 1;
	}
};

class ScrollerEmptyBoardInhibitorsFixture : public ScrollerGameFixture
{
protected:
	void setup() override
	{
		ScrollerGameFixture::setup();
	}
	void teardown() override
	{
		ScrollerGameFixture::teardown();
	}
	void fillBoard(int32_t /*nBoardW*/, int32_t /*nBoardH*/, std::vector<Tile>& /*aBoard*/) override
	{
	}
	int32_t getPriority() const override
	{
		return -10;
	}
	int32_t getSlices() const override
	{
		return 1;
	}
	int32_t getCheckNewRowTries() const override
	{
		return 20000;
	}
	std::vector< unique_ptr<TileSelector> > getInhibitors() const override
	{
		std::vector< unique_ptr<TileSelector> > aInhibitors;
		auto refCTS = make_unique<CharTraitSet>(make_unique<CharIndexTraitSet>(41, 41));
		auto refInhibit = std::make_unique<TileSelector>(make_unique<TileSelector::Trait>(false, std::move(refCTS)));
		aInhibitors.push_back(std::move(refInhibit));
		return aInhibitors;
	}
};

TEST_CASE_METHOD(STFX<ScrollerEmptyBoardFixture>, "ScrollEachTick")
{
	Level* p0Level = m_refLevel.get();

	const int32_t nBoardW = p0Level->boardWidth();
	const int32_t nBoardH = p0Level->boardHeight();

	auto oCheckTiles = [&](int32_t nExpectedTiles)
	{
		int32_t nNotEmpty = 0;
		for (int32_t nY = 0; nY < nBoardH; ++nY) {
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				const Tile& oTile = p0Level->boardGetTile(nX, nY);
				if (! oTile.isEmpty()) {
					++nNotEmpty;
				}
			}
		}
		REQUIRE(nNotEmpty == nExpectedTiles);
	};

	p0Level->activateEvent(m_p0ScrollerEvent, 0);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	oCheckTiles(0 * nBoardW);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	oCheckTiles(1 * nBoardW);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	oCheckTiles(2 * nBoardW);
}

TEST_CASE_METHOD(STFX<ScrollerEmptyBoardInhibitorsFixture>, "ScrollEachTickInhibiting")
{
	Level* p0Level = m_refLevel.get();

	const int32_t nBoardW = p0Level->boardWidth();
	const int32_t nBoardH = p0Level->boardHeight();

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	oMockInit.m_nPriority = 10;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	const int32_t nMockGroup = 8889;
	p0MockEvent->addListener(nMockGroup, m_p0ScrollerEvent, ScrollerEvent::MESSAGE_INHIBIT_START_INDEX_BASE + 0);


	auto oCheckTiles = [&](int32_t nExpectedTiles)
	{
		int32_t nFound = 0;
		for (int32_t nY = 0; nY < nBoardH; ++nY) {
			for (int32_t nX = 0; nX < nBoardW; ++nX) {
				const Tile& oTile = p0Level->boardGetTile(nX, nY);
				if ((! oTile.isEmpty()) && oTile.getTileChar().isCharIndex()
						&& oTile.getTileChar().getCharIndex() != 41) {
					++nFound;
				}
			}
		}
		REQUIRE(nFound == nExpectedTiles);
	};

	p0Level->activateEvent(m_p0ScrollerEvent, 0);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	oCheckTiles(0 * nBoardW);

	p0MockEvent->setTriggerValue(nMockGroup, 0, 0); // group 8889, unused, trigger after one tick

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	oCheckTiles(1 * nBoardW);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	oCheckTiles(2 * nBoardW);

}

} // namespace testing

} // namespace stmg
