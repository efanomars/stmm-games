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
 * File:   testSysEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/logevent.h"
#include "events/sysevent.h"

#include "stmm-games-fake/fixtureGame.h"
#include "stmm-games-fake/mockevent.h"

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class SysEventGameFixture : public GameFixture
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

TEST_CASE_METHOD(STFX<SysEventGameFixture>, "Constructor")
{
	REQUIRE_FALSE( m_refGame->isRunning() );
	REQUIRE( m_refGame->level(0)->boardWidth() == 10 );
	REQUIRE( m_refGame->level(0)->boardHeight() == 6 );
	Level* p0Level = m_refGame->level(0).get();
	SysEvent::Init oSInit;
	oSInit.m_p0Level = p0Level;
	oSInit.m_nLevelTeam = 0;
	oSInit.m_eType = SysEvent::SYS_EVENT_TYPE_LEVEL_COMPLETED;
	oSInit.m_bCreateTexts = true;
	oSInit.m_bFinishIfPossible = true;
	auto refSysEvent = make_unique<SysEvent>(std::move(oSInit));
	SysEvent* p0SysEvent = refSysEvent.get();
	p0Level->addEvent(std::move(refSysEvent));
	p0Level->activateEvent(p0SysEvent, 1);
	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	REQUIRE( gameEndedCount() == 0);
	m_refGame->handleTimer();
	REQUIRE( gameEndedCount() == 1);
	REQUIRE( m_refGame->gameElapsed() == 2 );
	m_refGame->handleTimer();
	REQUIRE( gameEndedCount() == 1);
	REQUIRE( m_refGame->gameElapsed() == 3 );
}

TEST_CASE_METHOD(STFX<SysEventGameFixture>, "PlayerOut")
{
	REQUIRE_FALSE( m_refGame->isRunning() );
	Level* p0Level = m_refGame->level(0).get();

	MockEvent::Init oMInit;
	oMInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	SysEvent::Init oSInit;
	oSInit.m_p0Level = p0Level;
	oSInit.m_nLevelTeam = 0;
	oSInit.m_eType = SysEvent::SYS_EVENT_TYPE_PLAYER_OUT;
	oSInit.m_bCreateTexts = true;
	oSInit.m_bFinishIfPossible = false;
	auto refSysEvent = make_unique<SysEvent>(std::move(oSInit));
	SysEvent* p0SysEvent = refSysEvent.get();
	p0Level->addEvent(std::move(refSysEvent));

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 87654;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0SysEvent->addListener(SysEvent::LISTENER_GROUP_GAME_OVER, p0LogEvent, 1001);
	p0MockEvent->addListener(888, p0SysEvent, 0); // mate 0

	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );

	p0MockEvent->setTriggerValue(888, 0, 1); // group 888, value 0 (mate), trigger after one tick

	m_refGame->handleTimer();
	REQUIRE( gameEndedCount() == 0);
	Variable& oVarFinishedTime0_0 = p0Level->variable(m_refGame->getPlayerVarIdFinishedTime(), 0, 0);
	REQUIRE( oVarFinishedTime0_0.get() < 0 );
	const int32_t nSaveElapsedMillisec = m_refGame->gameElapsedMillisec();
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	REQUIRE( gameEndedCount() == 0);
	REQUIRE( oVarFinishedTime0_0.get() == nSaveElapsedMillisec );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	REQUIRE( gameEndedCount() == 0);
	REQUIRE( oVarFinishedTime0_0.get() == nSaveElapsedMillisec );

	p0MockEvent->addListener(999, p0SysEvent, 1); // mate 1
	p0MockEvent->setTriggerValue(999, 0, 0); // immediately

	Variable& oVarFinishedTime0_1 = p0Level->variable(m_refGame->getPlayerVarIdFinishedTime(), 0, 1);
	const int32_t nSaveElapsedMillisec2 = m_refGame->gameElapsedMillisec();
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 4 );
	// Since we have a game with one team and two mates, both dead, game over is triggered
	REQUIRE( gameEndedCount() == 1);
	REQUIRE( oVarFinishedTime0_1.get() == nSaveElapsedMillisec2 );
	REQUIRE( oVarFinishedTime0_0.get() == nSaveElapsedMillisec2 ); // overwritten!

	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 87654 );
	REQUIRE( oEntry.m_nGameTick == 3 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == nSaveElapsedMillisec2 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0SysEvent) );

	Variable& oVarStatus0_0 = p0Level->variable(m_refGame->getPlayerVarIdStatus(), 0, 0);
	Variable& oVarStatus0_1 = p0Level->variable(m_refGame->getPlayerVarIdStatus(), 0, 1);
	REQUIRE( oVarStatus0_0.get() == GameProxy::VAR_VALUE_PLAYER_STATUS_FAILED );
	REQUIRE( oVarStatus0_1.get() == GameProxy::VAR_VALUE_PLAYER_STATUS_FAILED );

	Variable& oVarWinnerTeam = m_refGame->variable(m_refGame->getGameVarIdWinnerTeam());
	REQUIRE( oVarWinnerTeam.get() == 0 );
}

} // namespace testing

} // namespace stmg
