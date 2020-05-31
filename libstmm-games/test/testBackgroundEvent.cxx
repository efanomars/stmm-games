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
 * File:   testBackgroundEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/backgroundevent.h"
#include "mockevent.h"
#include "events/logevent.h"

#include "fixtureGame.h"

#include <iostream>

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class BackgroundGameFixture : public GameFixture
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

TEST_CASE_METHOD(STFX<BackgroundGameFixture>, "ConstructorTicks")
{
//	LogEvent::msgLog().reset();
//
//	Level* p0Level = m_refGame->level(0).get();
//	BackgroundEvent::Init oBInit;
//	oBInit.m_p0Level = p0Level;
//	BackgroundEvent::AlarmTimeout oAlarmTimeout;
//	oAlarmTimeout.m_nRepeat = 1;
//	oAlarmTimeout.m_eAlarmTimeoutType = BackgroundEvent::ALARMS_EVENT_SET_TICKS;
//	oAlarmTimeout.m_nChange = 1;
//	oBInit.m_aAlarmTimeouts.push_back(oAlarmTimeout);
//	oBInit.m_aAlarmTimeouts.push_back(std::move(oAlarmTimeout));
//	auto refBackgroundEvent = make_unique<BackgroundEvent>(std::move(oBInit));
//	BackgroundEvent* p0BackgroundEvent = refBackgroundEvent.get();
//	p0Level->addEvent(std::move(refBackgroundEvent));
//	p0Level->activateEvent(p0BackgroundEvent, 1);
//
//	LogEvent::Init oLInit;
//	oLInit.m_p0Level = p0Level;
//	oLInit.m_eKind = LogEvent::LOG_EVENT_KIND_MSG_LOG_SILENT;
//	oLInit.m_nTag = 82234;
//	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
//	LogEvent* p0LogEvent = refLogEvent.get();
//	p0Level->addEvent(std::move(refLogEvent));
//
//	p0BackgroundEvent->addListener(BackgroundEvent::LISTENER_GROUP_TIMEOUT, p0LogEvent, 1001);
//	p0BackgroundEvent->addListener(BackgroundEvent::LISTENER_GROUP_TIMEOUT, p0BackgroundEvent, BackgroundEvent::MESSAGE_ALARMS_NEXT);
//
//	m_refGame->start();
//	REQUIRE( m_refGame->gameElapsed() == 0 );
//	m_refGame->handleTimer();
//	REQUIRE( m_refGame->gameElapsed() == 1 );
//	m_refGame->handleTimer();
//	REQUIRE( m_refGame->gameElapsed() == 2 );
//	m_refGame->handleTimer();
//
//	// Make sure BackgroundEvent::MESSAGE_ALARMS_NEXT is ignored when finished
//	REQUIRE( m_refGame->gameElapsed() == 3 );
//	m_refGame->handleTimer();
//	REQUIRE( m_refGame->gameElapsed() == 4 );
//	m_refGame->handleTimer();
//
//	REQUIRE( LogEvent::msgLog().totEntries() == 2);
//	{
//	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
//	REQUIRE_FALSE( oEntry.isEmpty() );
//	REQUIRE( oEntry.tag() == 82234 );
//	REQUIRE( oEntry.gameTick() == 2 );
//	REQUIRE( oEntry.level() == 0 );
//	REQUIRE( oEntry.msg() == 1001 );
//	REQUIRE( oEntry.value() == 0 );
//	REQUIRE( oEntry.triggeringEventAdr() == reinterpret_cast<int64_t>(p0BackgroundEvent) );
//	}
//	{
//	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
//	REQUIRE_FALSE( oEntry.isEmpty() );
//	REQUIRE( oEntry.tag() == 82234 );
//	REQUIRE( oEntry.gameTick() == 3 );
//	REQUIRE( oEntry.level() == 0 );
//	REQUIRE( oEntry.msg() == 1001 );
//	REQUIRE( oEntry.value() == 1 );
//	REQUIRE( oEntry.triggeringEventAdr() == reinterpret_cast<int64_t>(p0BackgroundEvent) );
//	}
}

} // namespace testing

} // namespace stmg
