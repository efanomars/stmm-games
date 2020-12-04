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
 * File:   testRandomEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/randomevent.h"
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

class RandomGameFixture : public GameFixture
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

TEST_CASE_METHOD(STFX<RandomGameFixture>, "GeneratePermutations")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();
	RandomEvent::Init oRInit;
	oRInit.m_p0Level = p0Level;
	oRInit.m_nFrom = 0;
	oRInit.m_nTo = 9;
	oRInit.m_bPermutations = true;
	auto refRandomEvent = make_unique<RandomEvent>(std::move(oRInit));
	RandomEvent* p0RandomEvent = refRandomEvent.get();
	p0Level->addEvent(std::move(refRandomEvent));
	p0Level->activateEvent(p0RandomEvent, 1);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0RandomEvent->addListener(RandomEvent::LISTENER_GROUP_RANDOM, p0LogEvent, 1001);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0RandomEvent, RandomEvent::MESSAGE_GENERATE);

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );
	int32_t nExpectSum = 0;
	for (int32_t nIdx = 0; nIdx < 10; ++nIdx) {
		m_refGame->handleTimer();
		REQUIRE( m_refGame->gameElapsed() == 1 + nIdx);
		p0MockEvent->setTriggerValue(77, RandomEvent::MESSAGE_GENERATE, 0);
		nExpectSum += nIdx;
	}
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 11 );

	int32_t nSum = 0;
	REQUIRE( LogEvent::msgLog().totEntries() == 10);
	for (int32_t nIdx = 0; nIdx < 10; ++nIdx) {
		const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(nIdx);
		REQUIRE_FALSE( oEntry.isEmpty() );
		REQUIRE( oEntry.m_nTag == 82234 );
		//REQUIRE( oEntry.m_nGameTick == 2 );
		REQUIRE( oEntry.m_nLevel == 0 );
		REQUIRE( oEntry.m_nMsg == 1001 );
		REQUIRE( oEntry.m_nValue >= 0 );
		REQUIRE( oEntry.m_nValue <= 9 );
		nSum += oEntry.m_nValue;
		REQUIRE( oEntry.m_nTriggeringEventAddr == reinterpret_cast<int64_t>(p0RandomEvent) );
	}
	REQUIRE( nSum == nExpectSum );
	//
	for (int32_t nIdx = 0; nIdx < 10; ++nIdx) {
		m_refGame->handleTimer();
		REQUIRE( m_refGame->gameElapsed() == 12 + nIdx);
		p0MockEvent->setTriggerValue(77, RandomEvent::MESSAGE_GENERATE, 0);
	}
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 22 );

	REQUIRE( LogEvent::msgLog().totEntries() == 20);
	for (int32_t nIdx = 0; nIdx < 10; ++nIdx) {
		const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(nIdx);
		REQUIRE_FALSE( oEntry.isEmpty() );
		REQUIRE( oEntry.m_nTag == 82234 );
		//REQUIRE( oEntry.m_nGameTick == 2 );
		REQUIRE( oEntry.m_nLevel == 0 );
		REQUIRE( oEntry.m_nMsg == 1001 );
		REQUIRE( oEntry.m_nValue >= 0 );
		REQUIRE( oEntry.m_nValue <= 9 );
		nSum += oEntry.m_nValue;
		REQUIRE( oEntry.m_nTriggeringEventAddr == reinterpret_cast<int64_t>(p0RandomEvent) );
	}
	REQUIRE( nSum == nExpectSum * 2 );
}

} // namespace testing

} // namespace stmg
