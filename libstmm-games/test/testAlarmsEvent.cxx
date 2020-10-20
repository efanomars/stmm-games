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
 * File:   testAlarmsEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/alarmsevent.h"
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

class AlarmsGameFixture : public GameFixture
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

TEST_CASE_METHOD(STFX<AlarmsGameFixture>, "ConstructorTicks")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();
	AlarmsEvent::Init oAInit;
	oAInit.m_p0Level = p0Level;
	AlarmsEvent::AlarmsStage oAlarmsStage;
	oAlarmsStage.m_nRepeat = 1;
	oAlarmsStage.m_eAlarmsStageType = AlarmsEvent::ALARMS_STAGE_SET_TICKS;
	oAlarmsStage.m_nChange = 1;
	oAInit.m_aAlarmsStages.push_back(oAlarmsStage);
	oAInit.m_aAlarmsStages.push_back(std::move(oAlarmsStage));
	auto refAlarmsEvent = make_unique<AlarmsEvent>(std::move(oAInit));
	AlarmsEvent* p0AlarmsEvent = refAlarmsEvent.get();
	p0Level->addEvent(std::move(refAlarmsEvent));
	p0Level->activateEvent(p0AlarmsEvent, 1);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0LogEvent, 1001);
	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0AlarmsEvent, AlarmsEvent::MESSAGE_ALARMS_NEXT);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	m_refGame->handleTimer();

	// Make sure AlarmsEvent::MESSAGE_ALARMS_NEXT is ignored when finished
	REQUIRE( m_refGame->gameElapsed() == 3 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 4 );
	m_refGame->handleTimer();

	REQUIRE( LogEvent::msgLog().totEntries() == 2);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 0 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 3 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
}

TEST_CASE_METHOD(STFX<AlarmsGameFixture>, "RepeatTicks")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();
	AlarmsEvent::Init oAInit;
	oAInit.m_p0Level = p0Level;
	AlarmsEvent::AlarmsStage oAlarmsStage;
	oAlarmsStage.m_nRepeat = 1;
	oAlarmsStage.m_eAlarmsStageType = AlarmsEvent::ALARMS_STAGE_SET_TICKS;
	oAlarmsStage.m_nChange = 1;
	oAInit.m_aAlarmsStages.push_back(oAlarmsStage);
	oAlarmsStage.m_nRepeat = 2;
	oAInit.m_aAlarmsStages.push_back(std::move(oAlarmsStage));
	auto refAlarmsEvent = make_unique<AlarmsEvent>(std::move(oAInit));
	AlarmsEvent* p0AlarmsEvent = refAlarmsEvent.get();
	p0Level->addEvent(std::move(refAlarmsEvent));
	p0Level->activateEvent(p0AlarmsEvent, 1);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0LogEvent, 1001);
	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0AlarmsEvent, AlarmsEvent::MESSAGE_ALARMS_NEXT);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 1); // first timeout
//std::cout << "LogEvent::msgLog().totEntries()=" << LogEvent::msgLog().totEntries() << '\n';
//LogEvent::msgLog().dump();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 2); // repeatead timeout

	REQUIRE( m_refGame->gameElapsed() == 4 );
	m_refGame->handleTimer();

	REQUIRE( LogEvent::msgLog().totEntries() == 3);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(2);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 0 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 3 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 4 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 2 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
}

TEST_CASE_METHOD(STFX<AlarmsGameFixture>, "SetMillisec")
{
	LogEvent::msgLog().reset();

	const int32_t nInterval = m_refGame->gameInterval();
	assert(nInterval > 1);

	Level* p0Level = m_refGame->level(0).get();
	AlarmsEvent::Init oAInit;
	oAInit.m_p0Level = p0Level;

	AlarmsEvent::AlarmsStage oAlarmsStage;
	oAlarmsStage.m_nRepeat = 1;
	oAlarmsStage.m_eAlarmsStageType = AlarmsEvent::ALARMS_STAGE_SET_MILLISEC;
	oAlarmsStage.m_nChange = nInterval;
	oAInit.m_aAlarmsStages.push_back(oAlarmsStage);
	oAlarmsStage.m_nChange = nInterval / 2;
	oAInit.m_aAlarmsStages.push_back(oAlarmsStage);
	oAlarmsStage.m_nChange = nInterval * 2;
	oAInit.m_aAlarmsStages.push_back(std::move(oAlarmsStage));

	auto refAlarmsEvent = make_unique<AlarmsEvent>(std::move(oAInit));
	AlarmsEvent* p0AlarmsEvent = refAlarmsEvent.get();
	p0Level->addEvent(std::move(refAlarmsEvent));
	p0Level->activateEvent(p0AlarmsEvent, 1);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0LogEvent, 1001);
	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0AlarmsEvent, AlarmsEvent::MESSAGE_ALARMS_NEXT);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 1); // first timeout
//std::cout << "LogEvent::msgLog().totEntries()=" << LogEvent::msgLog().totEntries() << '\n';
//LogEvent::msgLog().dump();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 2);
	REQUIRE( m_refGame->gameElapsed() == 4 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 5 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 3);
	REQUIRE( m_refGame->gameElapsed() == 6 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 7 );
	m_refGame->handleTimer();

	REQUIRE( LogEvent::msgLog().totEntries() == 3);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(2);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 0 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 3 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 5 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 2 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0AlarmsEvent) );
	}
}

TEST_CASE_METHOD(STFX<AlarmsGameFixture>, "JumpStage")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();
	AlarmsEvent::Init oAInit;
	oAInit.m_p0Level = p0Level;
	AlarmsEvent::AlarmsStage oAlarmsStage;
	oAlarmsStage.m_eAlarmsStageType = AlarmsEvent::ALARMS_STAGE_SET_TICKS;
	oAlarmsStage.m_nChange = 1;
	oAlarmsStage.m_nRepeat = 10;
	oAInit.m_aAlarmsStages.push_back(oAlarmsStage);
	oAlarmsStage.m_eAlarmsStageType = AlarmsEvent::ALARMS_STAGE_SET_TICKS;
	oAlarmsStage.m_nChange = 3;
	oAlarmsStage.m_nRepeat = 10;
	oAInit.m_aAlarmsStages.push_back(std::move(oAlarmsStage));
	auto refAlarmsEvent = make_unique<AlarmsEvent>(std::move(oAInit));
	AlarmsEvent* p0AlarmsEvent = refAlarmsEvent.get();
	p0Level->addEvent(std::move(refAlarmsEvent));
	p0Level->activateEvent(p0AlarmsEvent, 0);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0LogEvent, 1001);
	p0AlarmsEvent->addListener(AlarmsEvent::LISTENER_GROUP_TIMEOUT, p0AlarmsEvent, AlarmsEvent::MESSAGE_ALARMS_NEXT);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0AlarmsEvent, AlarmsEvent::MESSAGE_ALARMS_STAGE_NEXT);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 1); // first timeout, second alarm started
	REQUIRE( m_refGame->gameElapsed() == 2 );

	const int32_t nSkipTicks = 0;
	p0MockEvent->setTriggerValue(77, 0, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 2); // second timeout, third alarm is started from stage 2
	REQUIRE( m_refGame->gameElapsed() == 3 );

	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 2);
	REQUIRE( m_refGame->gameElapsed() == 4 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 2);
	REQUIRE( m_refGame->gameElapsed() == 5 );
	m_refGame->handleTimer();
	REQUIRE( LogEvent::msgLog().totEntries() == 3); // third timeout, from stage 2
	REQUIRE( m_refGame->gameElapsed() == 6 );

}

} // namespace testing

} // namespace stmg
