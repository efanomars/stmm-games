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
 * File:   testDelayedQueueEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/delayedqueueevent.h"
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

class DelayedQueueGameFixture : public GameFixture
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
							//default , public FixtureVariantVariablesGame_None
							//default , public FixtureVariantVariablesTeam_None
							//default , public FixtureVariantVariablesPlayer_None
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

TEST_CASE_METHOD(STFX<DelayedQueueGameFixture>, "ConstructorTicks")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();
	DelayedQueueEvent::Init oDQInit;
	oDQInit.m_p0Level = p0Level;
	oDQInit.m_nInitialIntervalTicks = 2;
	oDQInit.m_nInitialIntervalMillisecs = 0;
	oDQInit.m_nMaxQueueSize = 2;
	auto refDelayedQueueEvent = make_unique<DelayedQueueEvent>(std::move(oDQInit));
	DelayedQueueEvent* p0DelayedQueueEvent = refDelayedQueueEvent.get();
	p0Level->addEvent(std::move(refDelayedQueueEvent));
	p0Level->activateEvent(p0DelayedQueueEvent, 1);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 92371;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0DelayedQueueEvent->addListener(DelayedQueueEvent::LISTENER_GROUP_VALUE, p0LogEvent, 1001);
	p0DelayedQueueEvent->addListener(DelayedQueueEvent::LISTENER_GROUP_OVERFLOW, p0LogEvent, 1002);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0DelayedQueueEvent, DelayedQueueEvent::MESSAGE_QUEUE_VALUE);

	const int32_t nSkipTicks = 0;

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );

	int32_t nValue = 121;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );

	nValue = 122;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );

	//nValue = 123;
	//p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();

	REQUIRE( m_refGame->gameElapsed() == 3 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 4 );
	m_refGame->handleTimer();

	REQUIRE( LogEvent::msgLog().totEntries() == 2);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 92371 );
	REQUIRE( oEntry.m_nGameTick == 0 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 121 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0DelayedQueueEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 92371 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 122 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0DelayedQueueEvent) );
	}
}


} // namespace testing

} // namespace stmg
