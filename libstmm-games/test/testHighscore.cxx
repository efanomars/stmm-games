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
 * File:   testHighscore.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "highscore.h"

#include "events/variableevent.h"

#include "stmm-games-fake/mockevent.h"
#include "stmm-games-fake/fixtureGame.h"
#include "stmm-games-fake/fixturevariantHighscoresDefinition.h"

namespace stmg
{

using std::shared_ptr;
using std::make_unique;
using std::unique_ptr;

namespace testing
{

class EndingGameFixture : public GameFixture
						, public FixtureVariantHighscoresDefinitionConsraint
						, public FixtureVariantHighscoresDefinitionDiscriminator
						, public FixtureVariantHighscoresDefinitionInclude
						, public FixtureVariantHighscoresDefinitionScoreElement
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

class OneTeamGameFixture : public EndingGameFixture
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
							//default , public FixtureVariantVariablesPlayer
							, public FixtureVariantHighscoresDefinitionConsraint_TotTeams_ExactlyOne
							, public FixtureVariantHighscoresDefinitionDiscriminator_TotTeams
							, public FixtureVariantHighscoresDefinitionInclude_TeamVarPoints_BE0
							, public FixtureVariantHighscoresDefinitionScoreElement_TeamVarPoints
{
};

class OneTeamPositivePointsGameFixture : public OneTeamGameFixture
										, public FixtureVariantVariablesGame_Time
										, public FixtureVariantVariablesTeam_Points_Plus1000
{
protected:
	void setup() override
	{
		OneTeamGameFixture::setup();

		m_nMaxScores = 5;

		std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint = FixtureVariantHighscoresDefinitionConsraint::getGameTypeConstraints();

		std::vector<HighscoresDefinition::Discriminator> aDiscriminators = FixtureVariantHighscoresDefinitionDiscriminator::getHDDiscriminator();

		// only players (teams) with non negative points are added to the highscore
		std::vector<HighscoresDefinition::Eligibility> aEligibilitys = FixtureVariantHighscoresDefinitionInclude::getHDIncludeConstraints();

		std::vector<HighscoresDefinition::ScoreElement> aScoreElements = FixtureVariantHighscoresDefinitionScoreElement::getHDScoreElements();

		m_refHighscoreDefinition = std::make_shared<HighscoresDefinition>(m_refPrefs->getStdConfig()
																		, aHighscoreConstraint, aDiscriminators, aEligibilitys
																		, aScoreElements, m_nMaxScores);
		assert(m_refHighscoreDefinition->satisfiesPreferences(*m_refPrefs));
	}
	void teardown() override
	{
		OneTeamGameFixture::teardown();
	}
public:
	int32_t m_nMaxScores;
	shared_ptr<HighscoresDefinition> m_refHighscoreDefinition;
};
TEST_CASE_METHOD(STFX<OneTeamPositivePointsGameFixture>, "PositiveConstructor")
{
	auto& oGame = *m_refGame;
	assert( oGame.isAllTeamsInOneLevel() );
	assert( oGame.getTotLevels() == 1 );
	assert( ! oGame.isRunning() );

	m_refGame->start();
	assert( m_refGame->gameElapsed() == 0 );

	m_refGame->handleTimer();
	assert( m_refGame->gameElapsed() == 1 );

	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );

	Highscore oH(m_refHighscoreDefinition, m_refHighscoreDefinition->getCodeFromPreferences(*m_refPrefs).second
				, m_refHighscoreDefinition->getTitleFromPreferences(*m_refPrefs).second, {});
	REQUIRE( oH.getTotScores() == 0 );
	REQUIRE( oH.getHighscoresDefinition() == m_refHighscoreDefinition );
	auto aPos = oH.includeGame(oGame);
	REQUIRE( aPos.size() == 1 );
	REQUIRE( aPos[0] == 0 );
	const Highscore::Score& oScore = oH.getScore(0);
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 1000 );
	REQUIRE( oScore.m_aValues[0].toFormattedString() == "1000" );
}
TEST_CASE_METHOD(STFX<OneTeamPositivePointsGameFixture>, "AddPosition")
{
	auto& oGame = *m_refGame;
	assert( oGame.isAllTeamsInOneLevel() );
	assert( oGame.getTotLevels() == 1 );
	assert( ! oGame.isRunning() );

	m_refGame->start();
	assert( m_refGame->gameElapsed() == 0 );

	m_refGame->handleTimer();
	assert( m_refGame->gameElapsed() == 1 );

	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );

	std::vector<Highscore::Score> aScores;
	std::array<int32_t, 4> aPoints = {5000, 4000, 3000, 500};
	for (auto nPoints : aPoints) {
		aScores.emplace_back();
		auto& oScore = aScores.back();
		oScore.m_sTeam = "T" + std::to_string(nPoints);
		oScore.m_aValues.push_back(Variable::Value::create(nPoints, Variable::VARIABLE_FORMAT_NUMBER));
	}
	Highscore oH(m_refHighscoreDefinition, m_refHighscoreDefinition->getCodeFromPreferences(*m_refPrefs).second
				, m_refHighscoreDefinition->getTitleFromPreferences(*m_refPrefs).second, aScores);
	REQUIRE( oH.getTotScores() == 4 );
	auto aPos = oH.includeGame(oGame);
	REQUIRE( oH.getTotScores() == 5 );
	REQUIRE( aPos.size() == 1 );
	REQUIRE( aPos[0] == 3 );
//std::cout << "------ aPos[0]=" << aPos[0] << '\n';
	{
	const Highscore::Score& oScore = oH.getScore(3);
	REQUIRE( oScore.m_nTeam == 0 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 1000 );
	REQUIRE( oScore.m_aValues[0].toFormattedString() == "1000" );
	}
	{
	const Highscore::Score& oScore = oH.getScore(2);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 3000 );
	}
	{
	const Highscore::Score& oScore = oH.getScore(4);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 500 );
	}
}
TEST_CASE_METHOD(STFX<OneTeamPositivePointsGameFixture>, "AddPolePosition")
{
	auto& oGame = *m_refGame;
	assert( oGame.isAllTeamsInOneLevel() );
	assert( oGame.getTotLevels() == 1 );
	assert( ! oGame.isRunning() );

	m_refGame->start();
	assert( m_refGame->gameElapsed() == 0 );

	m_refGame->handleTimer();
	assert( m_refGame->gameElapsed() == 1 );

	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );

	std::vector<Highscore::Score> aScores;
	std::array<int32_t, 5> aPoints = {800, 400, 300, 50, 20};
	for (auto nPoints : aPoints) {
		aScores.emplace_back();
		auto& oScore = aScores.back();
		oScore.m_sTeam = "T" + std::to_string(nPoints);
		oScore.m_aValues.push_back(Variable::Value::create(nPoints, Variable::VARIABLE_FORMAT_NUMBER));
	}
	Highscore oH(m_refHighscoreDefinition, m_refHighscoreDefinition->getCodeFromPreferences(*m_refPrefs).second
				, m_refHighscoreDefinition->getTitleFromPreferences(*m_refPrefs).second, aScores);
	REQUIRE( oH.getTotScores() == 5 );
	auto aPos = oH.includeGame(oGame);
	REQUIRE( oH.getTotScores() == 5 );
	REQUIRE( aPos.size() == 1 );
	REQUIRE( aPos[0] == 0 );
//std::cout << "------ aPos[0]=" << aPos[0] << '\n';
	{
	const Highscore::Score& oScore = oH.getScore(0);
	REQUIRE( oScore.m_nTeam == 0 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 1000 );
	}
	{
	const Highscore::Score& oScore = oH.getScore(1);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 800 );
	}
	{
	const Highscore::Score& oScore = oH.getScore(4);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 50 );
	}
}
TEST_CASE_METHOD(STFX<OneTeamPositivePointsGameFixture>, "AddNotGoodEnough")
{
	auto& oGame = *m_refGame;
	assert( oGame.isAllTeamsInOneLevel() );
	assert( oGame.getTotLevels() == 1 );
	assert( ! oGame.isRunning() );

	m_refGame->start();
	assert( m_refGame->gameElapsed() == 0 );

	m_refGame->handleTimer();
	assert( m_refGame->gameElapsed() == 1 );

	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );

	std::vector<Highscore::Score> aScores;
	std::array<int32_t, 5> aPoints = {8000, 4000, 3000, 2500, 1500};
	for (auto nPoints : aPoints) {
		aScores.emplace_back();
		auto& oScore = aScores.back();
		oScore.m_sTeam = "T" + std::to_string(nPoints);
		oScore.m_aValues.push_back(Variable::Value::create(nPoints, Variable::VARIABLE_FORMAT_NUMBER));
	}
	Highscore oH(m_refHighscoreDefinition, m_refHighscoreDefinition->getCodeFromPreferences(*m_refPrefs).second
				, m_refHighscoreDefinition->getTitleFromPreferences(*m_refPrefs).second, aScores);
	REQUIRE( oH.getTotScores() == 5 );
	auto aPos = oH.includeGame(oGame);
	REQUIRE( oH.getTotScores() == 5 );
	REQUIRE( aPos.size() == 0 );
//std::cout << "------ aPos[0]=" << aPos[0] << '\n';
	{
	const Highscore::Score& oScore = oH.getScore(0);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 8000 );
	}
	{
	const Highscore::Score& oScore = oH.getScore(4);
	REQUIRE( oScore.m_nTeam == -1 );
	REQUIRE( oScore.m_aValues.size() == 1 );
	REQUIRE( oScore.m_aValues[0].get() == 1500 );
	}
}

class OneTeamNegativePointsGameFixture : public OneTeamGameFixture
										, public FixtureVariantVariablesGame_Time
										, public FixtureVariantVariablesTeam_Points_Minus1000
{
};
TEST_CASE_METHOD(STFX<OneTeamNegativePointsGameFixture>, "NegativeConstructor")
{
	auto& oGame = *m_refGame;
	assert( oGame.isAllTeamsInOneLevel() );
	assert( oGame.getTotLevels() == 1 );
	assert( ! oGame.isRunning() );

	m_refGame->start();
	assert( m_refGame->gameElapsed() == 0 );

	m_refGame->handleTimer();
	assert( m_refGame->gameElapsed() == 1 );

	oGame.end();
	REQUIRE_FALSE( oGame.isRunning() );

	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint = FixtureVariantHighscoresDefinitionConsraint::getGameTypeConstraints();
	assert(aHighscoreConstraint.size() == 1);

	std::vector<HighscoresDefinition::Discriminator> aDiscriminators = FixtureVariantHighscoresDefinitionDiscriminator::getHDDiscriminator();
	assert(aDiscriminators.size() == 1);

	// only players (teams) with non negative points are added to the highscore
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys = FixtureVariantHighscoresDefinitionInclude::getHDIncludeConstraints();
	assert(aEligibilitys.size() == 1);

	std::vector<HighscoresDefinition::ScoreElement> aScoreElements = FixtureVariantHighscoresDefinitionScoreElement::getHDScoreElements();
	assert(aScoreElements.size() == 1);

	auto refHD = std::make_shared<HighscoresDefinition>(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	assert(refHD->satisfiesPreferences(*m_refPrefs));

	Highscore oH(refHD, refHD->getCodeFromPreferences(*m_refPrefs).second, refHD->getTitleFromPreferences(*m_refPrefs).second);
	REQUIRE( oH.getTotScores() == 0 );
	REQUIRE( oH.getHighscoresDefinition() == refHD );
	auto aPos = oH.includeGame(oGame);
	REQUIRE( aPos.size() == 0 );
}

} // namespace testing

} // namespace stmg
