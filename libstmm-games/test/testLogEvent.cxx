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
 * File:   testLogEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/logevent.h"

#include "stmm-games-fake/mockevent.h"
#include "stmm-games-fake/fixtureGame.h"

#include <iostream>

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class LogEventGameFixture : public GameFixture
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

TEST_CASE_METHOD(STFX<LogEventGameFixture>, "Constructor")
{
	Level* p0Level = m_refGame->level(0).get();
	LogEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	oInit.m_bToStdOut = false;
	oInit.m_nTag = 76543;
	auto refLogEvent = make_unique<LogEvent>(std::move(oInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));
	p0Level->activateEvent(p0LogEvent, 1);
	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 76543 );
	REQUIRE( oEntry.m_nGameTick == 1 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 0 );
	REQUIRE( oEntry.m_nValue == 0 );
	REQUIRE( oEntry.m_nTriggeringEventAddr == reinterpret_cast<int64_t>(nullptr) );
	REQUIRE( m_refGame->gameElapsed() == 2 );
}

TEST_CASE_METHOD(STFX<LogEventGameFixture>, "Trigger")
{
	Level* p0Level = m_refGame->level(0).get();
	LogEvent::Init oInit;
	oInit.m_p0Level = p0Level;
	oInit.m_bToStdOut = false;
	oInit.m_nTag = 76544;
	auto refLogEvent = make_unique<LogEvent>(std::move(oInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));
	REQUIRE_FALSE( p0LogEvent->isActive() );

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0LogEvent, 44);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	p0MockEvent->setTriggerValue(77, 11, 0);
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 76544 );
	REQUIRE( oEntry.m_nGameTick == 1 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 44 );
	REQUIRE( oEntry.m_nValue == 11 );
	REQUIRE( oEntry.m_nTriggeringEventAddr == reinterpret_cast<int64_t>(p0MockEvent) );
}

} // namespace testing

} // namespace stmg
