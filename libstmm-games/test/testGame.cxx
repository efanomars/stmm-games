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
 * File:   testGame.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "game.h"
#include "event.h"

#include "fixtureLayoutAuto.h"
#include "fixtureGameOwner.h"

namespace stmg
{

using std::shared_ptr;

namespace testing
{

class GameLayoutAutoFixture : public LayoutAutoFixture
							, public Game::CreateLevelCallback, public GameOwnerFixture //, public GameOwner
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
{
protected:
	void setup() override
	{
		LayoutAutoFixture::setup();
		GameOwnerFixture::setup();
	}
	void teardown() override
	{
		GameOwnerFixture::teardown();
		LayoutAutoFixture::teardown();
	}
public:
	shared_ptr<Level> createLevel(Game* p0Game, int32_t nLevel
									, const shared_ptr<AppPreferences>& refPreferences
									, const Level::Init& oInit) noexcept override
	{
		assert(p0Game != nullptr);
		return std::make_shared<Level>(p0Game, nLevel, refPreferences, oInit);
	}
};

TEST_CASE_METHOD(STFX<GameLayoutAutoFixture>, "Constructor")
{
	GameOwnerFixture::resetGameOwner();

	Named oNamed;
	Level::Init oLevelInit;
	oLevelInit.m_nBoardW = 10;
	oLevelInit.m_nBoardH = 8;
	oLevelInit.m_nShowW = 10;
	oLevelInit.m_nShowH = 8;
	Game::Init oGameInit;
	oGameInit.m_sName = std::string{"Test"};
	oGameInit.m_p0GameOwner = this;
	oGameInit.m_oNamed = std::move(oNamed);
	oGameInit.m_oGameVariableTypes = getVariablesGame();
	oGameInit.m_oTeamVariableTypes = getVariablesTeam();
	oGameInit.m_oPlayerVariableTypes = getVariablesPlayer();
	oGameInit.m_refLayout = m_refLayout;
	Game oGame{std::move(oGameInit), *this, oLevelInit};
	REQUIRE( oGame.getLayout() == m_refLayout);
	REQUIRE( oGame.getNamed().animations().size() == oNamed.animations().size());

	REQUIRE( oGame.isAllTeamsInOneLevel() );
	REQUIRE( oGame.getTotLevels() == 1 );
	shared_ptr<Level>& refLevel = oGame.level(0);
	REQUIRE( refLevel->getLevel() == 0 );

	REQUIRE( refLevel->getTotLevelTeams() == 1 );
	REQUIRE( refLevel->getTotLevelPlayers() == 2 );
	REQUIRE( refLevel->boardWidth() == 10 );
	REQUIRE( refLevel->boardHeight() == 8 );
	LevelShow& oLevelShow = refLevel->showGet();
	REQUIRE( oLevelShow.getW() == 10 );
	REQUIRE( oLevelShow.getH() == 8 );
	REQUIRE_FALSE( refLevel->subshowMode() );
	//LevelShow& oLevelSubShow0 = refLevel->subshowGet(0, 0);
	//REQUIRE( oLevelSubShow0.getW() == 10 );
	//REQUIRE( oLevelSubShow0.getH() == 8 );
	//LevelShow& oLevelSubShow1 = refLevel->subshowGet(1);
	//REQUIRE( oLevelSubShow1.getW() == 10 );
	//REQUIRE( oLevelSubShow1.getH() == 8 );

	REQUIRE( oGame.getLayout() == m_refLayout );

	{
	const int32_t nGameTimeId = getVariablesGame().getIndex("Time");
	assert(nGameTimeId >= 0);
	const auto oPairTime = oGame.variableIdAndOwner("Time");
	REQUIRE( oPairTime.first == nGameTimeId);
	REQUIRE( oPairTime.second == OwnerType::GAME);
	REQUIRE( oGame.hasVariableId(nGameTimeId, OwnerType::GAME) );
	REQUIRE( oGame.variable(nGameTimeId).getValue().get() == 0);
	REQUIRE( oGame.variable(nGameTimeId).get() == 0 );
	}
	{
	const int32_t nPlayerLivesId = getVariablesPlayer().getIndex("Lives");
	assert(nPlayerLivesId >= 0);
	const auto oPairLives = oGame.variableIdAndOwner("Lives");
	REQUIRE( oPairLives.first == nPlayerLivesId);
	REQUIRE( oPairLives.second == OwnerType::PLAYER);
	REQUIRE( oGame.hasVariableId(nPlayerLivesId, OwnerType::PLAYER) );
	REQUIRE( oGame.variable(nPlayerLivesId, 0, 0, 0).get() == 3 );
	REQUIRE( oGame.variable(nPlayerLivesId, 0, 0, 1).get() == 3 );
	}

	{
	REQUIRE_FALSE( oGame.isRunning() );
	oGame.start();
	REQUIRE( oGame.isRunning() );
	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );
	}
}

} // namespace testing

} // namespace stmg
