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
 * File:   testLayout.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "layout.h"
#include "gamewidgets.h"

#include "stmm-games-fake/fixturevariantVariables.h"
#include "stmm-games-fake/fixtureStdPreferences.h"

namespace stmg
{

 using std::shared_ptr;
 
namespace testing
{

class LayoutStdPreferencesFixture : public StdPreferencesFixture
									//default , public FixtureVariantDevicesKeys_Two, public FixtureVariantDevicesJoystick_Two
									, public FixtureVariantPrefsTeams<1>
									//default , public FixtureVariantPrefsMates<0,2>
									//default , public FixtureVariantMatesPerTeamMax_Three, public FixtureVariantAIMatesPerTeamMax_Zero
									//default , public FixtureVariantAllowMixedAIHumanTeam_False, public FixtureVariantPlayersMax_Six
									//default , public FixtureVariantTeamsMin_One, public FixtureVariantTeamsMax_Two
									//default , public FixtureVariantKeyActions_AllCapabilityClassesDefaults
									//, public FixtureVariantVariablesGame
									//, public FixtureVariantVariablesTeam
									, public FixtureVariantVariablesPlayer, public FixtureVariantVariablesPlayer_Lives<3>
{
protected:
	void setup() override
	{
		StdPreferencesFixture::setup();
		assert(m_refPrefs);
		assert(m_refPrefs->getTotTeams() == 1);
		assert(m_refPrefs->getTeamFull(0)->getTotMates() == 2);
		// Set up AllTeamsInOneLevel 1 team layout
		LevelShowWidget::Init oLSInit;
		oLSInit.setExpand(true);
		oLSInit.setAlign(WidgetAxisFit::ALIGN_FILL);
		m_refLSW = std::make_shared<LevelShowWidget>(std::move(oLSInit));
		//
		VarWidget::Init oVarInit;
		oVarInit.m_sName = "Life";
		oVarInit.setHorizAlign(WidgetAxisFit::ALIGN_FILL);
		oVarInit.setVertAlign(WidgetAxisFit::ALIGN_START);
		oVarInit.m_nTeam = 0;
		oVarInit.m_nMate = 0;
		oVarInit.m_sTitle = "Lives";
		oVarInit.m_nVarId = FixtureVariantVariablesPlayer::getVariablesPlayer().getIndex("Lives");
		oVarInit.m_fReferenceWFactor = 0.2;
		oVarInit.m_fReferenceHFactor = 0.1;
		auto oVarInit0 = oVarInit;
		auto refPlayer0Var = std::make_shared<VarWidget>(std::move(oVarInit0));
		auto oVarInit1 = oVarInit;
		oVarInit1.m_nMate = 1;
		auto refPlayer1Var = std::make_shared<VarWidget>(std::move(oVarInit1));
		//
		BoxWidget::Init oLevelBoxInit;
		oLevelBoxInit.setSwitchable(true, false, true);
		oLevelBoxInit.m_aChildWidgets.push_back(refPlayer0Var);
		oLevelBoxInit.m_aChildWidgets.push_back(refPlayer1Var);
		auto refLevelBox = std::make_shared<BoxWidget>(std::move(oLevelBoxInit));
		//
		BoxWidget::Init oRootBoxInit;
		oRootBoxInit.setSwitchable(true, false, true);
		oRootBoxInit.m_aChildWidgets.push_back(m_refLSW);
		oRootBoxInit.m_aChildWidgets.push_back(refLevelBox);
		m_refRootWidget = std::make_shared<BoxWidget>(std::move(oRootBoxInit));
	}
	void teardown() override
	{
		StdPreferencesFixture::teardown();
	}
protected:
	shared_ptr<GameWidget> m_refRootWidget;
	shared_ptr<LevelShowWidget> m_refLSW;
};

TEST_CASE_METHOD(STFX<LayoutStdPreferencesFixture>, "Constructor")
{
	auto refLayout = std::make_shared<Layout>(m_refRootWidget, shared_ptr<LevelShowWidget>{}
											, true, false, m_refPrefs);
//std::cout << refLayout->getErrorString() << '\n';
//std::cout << "DUMP" << '\n';
//refLayout->dump();
	REQUIRE(refLayout->isValid());
	REQUIRE(refLayout->isAllTeamsInOneLevel());
	REQUIRE_FALSE(refLayout->isSubshows());
	REQUIRE(refLayout->getRoot() == m_refRootWidget);
	REQUIRE(refLayout->getReferenceShow() == m_refLSW);
	REQUIRE(refLayout->getWidgetNamed("Life", 0, 0).operator->() != nullptr) ;
	REQUIRE(refLayout->getWidgetNamed("Life", 0, 1).operator->() != nullptr) ;
	REQUIRE(refLayout->getWidgetNamed("Life", 0, -1).operator->() == nullptr) ;
	REQUIRE(refLayout->getWidgetNamed("Life", -1, -1).operator->() == nullptr) ;
	REQUIRE(refLayout->getPrefs() == m_refPrefs);
	REQUIRE(refLayout->getPrefs() == m_refPrefs);

}

} // namespace testing

} // namespace stmg
