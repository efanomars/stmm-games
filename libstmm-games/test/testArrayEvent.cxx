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
 * File:   testArrayEvent.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "events/arrayevent.h"
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

class ArrayGameFixture : public GameFixture
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
	NamedObjIndex<Variable::VariableType> getVariablesTeam() const override
	{
		auto oTeamVarTypes = FixtureVariantVariablesTeam::getVariablesTeam();
		Variable::VariableType oVarType;
		oVarType.m_nInitialValue = 1;
		oTeamVarTypes.addNamedObj("Level", oVarType);
		return oTeamVarTypes;
	}
	int32_t getTeamLevelVarId() const
	{
		const auto oPair = m_refGame->variableIdAndOwner("Level");
		const int32_t nVarId = oPair.first;
		assert(oPair.second == OwnerType::TEAM);
		assert(nVarId >= 0);
		return nVarId;
	}
	int32_t getPlayerLivesVarId() const
	{
		const auto oPair = m_refGame->variableIdAndOwner("Lives");
		const int32_t nVarId = oPair.first;
		assert(oPair.second == OwnerType::PLAYER);
		assert(nVarId >= 0);
		return nVarId;
	}
	template<class FILLER>
	unique_ptr<ArrayEvent> createArrayEvent(int32_t nSize0, int32_t nSize1, FILLER oF) noexcept
	{
		Level* p0Level = m_refGame->level(0).get();
		ArrayEvent::Init oAInit;
		oAInit.m_p0Level = p0Level;

		ArrayEvent::Dimension oDim0;
		oDim0.m_nVarIndex = getTeamLevelVarId();
		oDim0.m_nVarTeam = 0;
		oDim0.m_nSize = nSize0;
		ArrayEvent::Dimension oDim1;
		oDim1.m_nSize = nSize1;
		std::vector<ArrayEvent::Dimension> aDimensions;
		aDimensions.push_back(oDim0);
		aDimensions.push_back(oDim1);
		oAInit.m_aDimensions = std::move(aDimensions);

		oAInit.m_aValues.resize(nSize0 * nSize1);

		for (int32_t nIdx0 = 0; nIdx0 < nSize0; ++nIdx0) {
			for (int32_t nIdx1 = 0; nIdx1 < nSize1; ++nIdx1) {
				const int32_t nCompoundIdx = nIdx0 * nSize1 + nIdx1;
				oAInit.m_aValues[nCompoundIdx] = oF(nIdx0, nIdx1);
			}
		}

		oAInit.m_nDefaultValue = -1;

		return std::make_unique<ArrayEvent>(std::move(oAInit));
	}
	template<class FILLER>
	unique_ptr<ArrayEvent> createArrayEvent(int32_t nSize0, int32_t nSize1, int32_t nSize2, FILLER oF) noexcept
	{
		Level* p0Level = m_refGame->level(0).get();
		ArrayEvent::Init oAInit;
		oAInit.m_p0Level = p0Level;

		ArrayEvent::Dimension oDim0;
		oDim0.m_nVarIndex = getTeamLevelVarId();
		oDim0.m_nVarTeam = 0;
		oDim0.m_nSize = nSize0;
		ArrayEvent::Dimension oDim1;
		oDim1.m_nSize = nSize1;
		ArrayEvent::Dimension oDim2;
		oDim2.m_nVarIndex = getPlayerLivesVarId();
		oDim2.m_nVarTeam = 0;
		oDim2.m_nVarMate = 0;
		oDim2.m_nSize = nSize2;
		std::vector<ArrayEvent::Dimension> aDimensions;
		aDimensions.push_back(oDim0);
		aDimensions.push_back(oDim1);
		aDimensions.push_back(oDim2);
		oAInit.m_aDimensions = std::move(aDimensions);

		oAInit.m_aValues.resize(nSize0 * nSize1 * nSize2);

		for (int32_t nIdx0 = 0; nIdx0 < nSize0; ++nIdx0) {
			for (int32_t nIdx1 = 0; nIdx1 < nSize1; ++nIdx1) {
				for (int32_t nIdx2 = 0; nIdx2 < nSize2; ++nIdx2) {
					const int32_t nCompoundIdx = (nIdx0 * nSize1 + nIdx1) * nSize2 + nIdx2;
//std::cout << "!!!!!!!!!!! (" << nIdx0 << "," << nIdx1 << "," << nIdx2 << ") = nCompoundIdx=" << nCompoundIdx << '\n';
					oAInit.m_aValues[nCompoundIdx] = oF(nIdx0, nIdx1, nIdx2);
				}
			}
		}

		oAInit.m_nDefaultValue = -1;

		return std::make_unique<ArrayEvent>(std::move(oAInit));
	}
};

TEST_CASE_METHOD(STFX<ArrayGameFixture>, "Array5x10_Level_nValue")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();

	const int32_t nLevelVarId = getTeamLevelVarId();

	const int32_t nSize0 = 5;
	const int32_t nSize1 = 10;
	unique_ptr<ArrayEvent> refArrayEvent = createArrayEvent(nSize0, nSize1, [](int32_t nIdx0, int32_t nIdx1)
	{
		return nIdx0 * nIdx1 + 1; 
	});
	ArrayEvent* p0ArrayEvent = refArrayEvent.get();
	p0Level->addEvent(std::move(refArrayEvent));
	p0Level->activateEvent(p0ArrayEvent, 0);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0ArrayEvent->addListener(ArrayEvent::LISTENER_GROUP_ARRAY_RESULT, p0LogEvent, 1001);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0ArrayEvent, ArrayEvent::MESSAGE_ARRAY_GET);

	const int32_t nSkipTicks = 0;

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );

	int32_t nValue = 0;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = 0;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );

	nValue = nSize0 - 1;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = 1;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );

	nValue = nSize0 - 1;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = nSize1 - 1;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );

	REQUIRE( LogEvent::msgLog().totEntries() == 3);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(2);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 0 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 1 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == (nSize0 - 1) * 1 + 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == (nSize0 - 1) * (nSize1 - 1) + 1 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
}

TEST_CASE_METHOD(STFX<ArrayGameFixture>, "Array5x10x20_Level_nValue_Lives")
{
	LogEvent::msgLog().reset();

	Level* p0Level = m_refGame->level(0).get();

	const int32_t nLevelVarId = getTeamLevelVarId();

	const int32_t nLivesVarId = getPlayerLivesVarId();

	const int32_t nSize0 = 5;
	const int32_t nSize1 = 10;
	const int32_t nSize2 = 20;
	unique_ptr<ArrayEvent> refArrayEvent = createArrayEvent(nSize0, nSize1, nSize2, [](int32_t nIdx0, int32_t nIdx1, int32_t nIdx2)
	{
		return nIdx0 * nIdx1 * nIdx2 + 3; 
	});
	ArrayEvent* p0ArrayEvent = refArrayEvent.get();
	p0Level->addEvent(std::move(refArrayEvent));
	p0Level->activateEvent(p0ArrayEvent, 0);

	LogEvent::Init oLInit;
	oLInit.m_p0Level = p0Level;
	oLInit.m_bToStdOut = false;
	oLInit.m_nTag = 82234;
	auto refLogEvent = make_unique<LogEvent>(std::move(oLInit));
	LogEvent* p0LogEvent = refLogEvent.get();
	p0Level->addEvent(std::move(refLogEvent));

	p0ArrayEvent->addListener(ArrayEvent::LISTENER_GROUP_ARRAY_RESULT, p0LogEvent, 1001);

	MockEvent::Init oMockInit;
	oMockInit.m_p0Level = p0Level;
	auto refMockEvent = make_unique<MockEvent>(std::move(oMockInit));
	MockEvent* p0MockEvent = refMockEvent.get();
	p0Level->addEvent(std::move(refMockEvent));

	p0MockEvent->addListener(77, p0ArrayEvent, ArrayEvent::MESSAGE_ARRAY_GET);

	const int32_t nSkipTicks = 0;

	m_refGame->start();
	REQUIRE( m_refGame->gameElapsed() == 0 );

	int32_t nValue = 0;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = 0;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);
	nValue = 0;
	p0MockEvent->setVariable(nLivesVarId, 0, 0, nValue);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 1 );

	nValue = nSize0 - 1;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = 1;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);
	nValue = nSize2 - 1;
	p0MockEvent->setVariable(nLivesVarId, 0, 0, nValue);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 2 );

	nValue = nSize0 - 1;
	p0MockEvent->setVariable(nLevelVarId, 0, -1, nValue);
	nValue = nSize1 - 1;
	p0MockEvent->setTriggerValue(77, nValue, nSkipTicks);
	nValue = nSize2 / 2;
	p0MockEvent->setVariable(nLivesVarId, 0, 0, nValue);

	m_refGame->handleTimer();
	REQUIRE( m_refGame->gameElapsed() == 3 );

	REQUIRE( LogEvent::msgLog().totEntries() == 3);
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(2);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 0 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == 3 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last(1);
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 1 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == (nSize0 - 1) * 1 * (nSize2 - 1) + 3 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
	{
	const LogEvent::MsgLog::Entry& oEntry = LogEvent::msgLog().last();
	REQUIRE_FALSE( oEntry.isEmpty() );
	REQUIRE( oEntry.m_nTag == 82234 );
	REQUIRE( oEntry.m_nGameTick == 2 );
	REQUIRE( oEntry.m_nLevel == 0 );
	REQUIRE( oEntry.m_nMsg == 1001 );
	REQUIRE( oEntry.m_nValue == (nSize0 - 1) * (nSize1 - 1) * (nSize2 / 2) + 3 );
	REQUIRE( oEntry.m_nTriggeringEventAdr == reinterpret_cast<int64_t>(p0ArrayEvent) );
	}
}

} // namespace testing

} // namespace stmg
