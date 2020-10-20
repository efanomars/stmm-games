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
 * File:   testVariableEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/variableevent.h"
#include "events/logevent.h"

#include "stmm-games-fake/fixtureGame.h"
#include "stmm-games-fake/mockevent.h"

namespace stmg
{

using std::shared_ptr;
using std::unique_ptr;
using std::make_unique;

namespace testing
{

class VariableEventGameFixture : public GameFixture
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
							, public FixtureVariantVariablesPlayer_Points
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

TEST_CASE_METHOD(STFX<VariableEventGameFixture>, "Constructor")
{
	Level* p0Level = m_refGame->level(0).get();

	MockEvent::Init oMInit;
	oMInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	const int32_t nVarId = getVariablesPlayer().getIndex("Points");
	assert(nVarId >= 0);
	VariableEvent::Init oVInit;
	oVInit.m_p0Level = p0Level;
	oVInit.m_nVarTeam = 0;
	oVInit.m_nVarMate = 0;
	oVInit.m_nVarIndex = nVarId;
	oVInit.m_nDefaultIncBy = 77;
	auto refVariableEvent = make_unique<VariableEvent>(std::move(oVInit));
	VariableEvent* p0VariableEvent = refVariableEvent.get();
	p0Level->addEvent(std::move(refVariableEvent));

	const int32_t nInitValue = 1234;
	{
	const int32_t nGroup = 888;
	const int32_t nMsg = VariableEvent::MESSAGE_SET_VALUE;
	const int32_t nValue = nInitValue;
	p0MockEvent->addListener(nGroup, p0VariableEvent, nMsg);

	m_refGame->start();
	REQUIRE( m_refGame->isRunning() );
	REQUIRE( m_refGame->gameElapsed() == 0 );

	const int32_t nSkipTicks = 0;
	p0MockEvent->setTriggerValue(nGroup, nValue, nSkipTicks);
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );

	auto& oVar = p0Level->variable(nVarId, 0, 0);
	REQUIRE( oVar.get() == nValue );
	}

	{
	const int32_t nGroup = 999;
	const int32_t nMsg = -121212; // bogus message
	const int32_t nValue = 989898; // is ignored
	p0MockEvent->addListener(nGroup, p0VariableEvent, nMsg);

	const int32_t nSkipTicks = 1;
	p0MockEvent->setTriggerValue(nGroup, nValue, nSkipTicks);
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );

	auto& oVar = p0Level->variable(nVarId, 0, 0);
	REQUIRE( oVar.get() == nInitValue );
	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );
	REQUIRE( oVar.get() == nInitValue + 77 );
	}
}

} // namespace testing

} // namespace stmg
