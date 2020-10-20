/*
 * Copyright © 2019-2020  Stefano Marsili, <stemars@gmx.ch>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 3 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, see <http://www.gnu.org/licenses/>
 */
/*
 * File:   fixtureStdConfig.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_STD_CONFIG_H
#define STMG_TESTING_FIXTURE_STD_CONFIG_H

#include "fixtureTestBase.h"

#include "fixturevariantPlayers.h"
#include "fixturevariantTeams.h"
#include "fixturevariantKeyActions.h"
#include "fixturevariantOptions.h"

#include <stmm-games/stdconfig.h>
#include <stmm-games/appconstraints.h>

#include <stmm-input-fake/fakedevicemanager.h>

#include <cassert>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class StdConfigFixture : public TestBaseFixture
						, public FixtureVariantMatesPerTeamMax, public FixtureVariantAIMatesPerTeamMax
						, public FixtureVariantAllowMixedAIHumanTeam, public FixtureVariantPlayersMax
						, public FixtureVariantTeamsMin, public FixtureVariantTeamsMax
						, public FixtureVariantOptions
						, public FixtureVariantKeyActions
{
protected:
	void setup() override
	{
		TestBaseFixture::setup();

		StdConfig::Init oStdConfigInit;
		oStdConfigInit.m_sAppName = "TestApp";
		oStdConfigInit.m_sAppVersion = "0.0";

		oStdConfigInit.m_bSoundEnabled = true;
		oStdConfigInit.m_bSoundPerPlayerAllowed = true;

		m_refDM = std::make_shared<stmi::testing::FakeDeviceManager>();
		oStdConfigInit.m_refDeviceManager = m_refDM;

		AppConstraints& oAppConstraints = oStdConfigInit.m_oAppConstraints;
		oAppConstraints.m_bAllowMixedAIHumanTeam = getAllowMixedAIHumanTeam();
		oAppConstraints.m_nTeamsMin = getTeamsMin();
		oAppConstraints.m_nTeamsMax = getTeamsMax();
		oAppConstraints.m_nMatesPerTeamMax = getMatesPerTeamMax();
		oAppConstraints.m_nPlayersMax = getPlayersMax();
		oAppConstraints.m_nAIMatesPerTeamMax = getAIMatesPerTeamMax();
		assert(oAppConstraints.isValid());

		oStdConfigInit.m_aKeyActions = getKeyActions();
		oStdConfigInit.m_aOptions = FixtureVariantOptions::getConfigOptions();

		StdConfig::CapabilityAssignment& oCapabilityAssignment = oStdConfigInit.m_oCapabilityAssignment;
		oCapabilityAssignment.m_nMaxCapabilitiesExplicitlyAssignedToPlayer = 1000;
		oCapabilityAssignment.m_bCapabilitiesAutoAssignedToActivePlayer = true;

		m_refStdConfig = std::make_shared<StdConfig>(std::move(oStdConfigInit));
	}
	void teardown() override
	{
		m_refStdConfig.reset();
		m_refDM.reset();

		TestBaseFixture::teardown();
	}
public:
	shared_ptr<stmi::testing::FakeDeviceManager> m_refDM;
	shared_ptr<StdConfig> m_refStdConfig;
};

} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_STD_CONFIG_H */
