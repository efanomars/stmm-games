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
 * File:   testHighscoresDefinition.cxx
 */

#define CATCH_CONFIG_MAIN
#include "catch2/catch.hpp"

#include "highscoresdefinition.h"

#include "stmm-games-fake/fixtureStdPreferences.h"

namespace stmg
{

namespace testing
{

class HiDefPreferencesFixture : public StdPreferencesFixture
								//default , public FixtureVariantDevicesKeys_Two, public FixtureVariantDevicesJoystick_Two
								, public FixtureVariantPrefsTeams<1>
								, public FixtureVariantOptions_Game_Difficulty
								, public FixtureVariantTeamsMax_Two
								//default , public FixtureVariantPrefsMates<0,2>
								//default , public FixtureVariantMatesPerTeamMax_Three, public FixtureVariantAIMatesPerTeamMax_Zero
								//default , public FixtureVariantAllowMixedAIHumanTeam_False, public FixtureVariantPlayersMax_Six
								//default , public FixtureVariantTeamsMin_One, public FixtureVariantTeamsMax_Two
								//default , public FixtureVariantAllCapabilityClassesDefaults_All
{
protected:
	void setup() override
	{
		StdPreferencesFixture::setup();
		assert(m_refPrefs);
		assert(m_refPrefs->getTotTeams() == 1);
		assert(m_refPrefs->getTeamFull(0)->getTotMates() == 2);
	}
	void teardown() override
	{
		StdPreferencesFixture::teardown();
	}
private:
};

TEST_CASE_METHOD(STFX<HiDefPreferencesFixture>, "Constructor")
{
	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint;
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	HighscoresDefinition::ScoreElement oScoreEl;
	oScoreEl.m_sVarDescription = "Time";
	oScoreEl.m_sVarName = "Time";
	oScoreEl.m_bLowestBest = true;
	aScoreElements.push_back(oScoreEl);
	HighscoresDefinition oHD(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	REQUIRE(oHD.getDiscriminators().size() == 0);
	REQUIRE(oHD.getEligibilitys().size() == 0);
	REQUIRE(oHD.getScoreElements().size() == 1);
	REQUIRE(oHD.getMaxScores() == 10);
	REQUIRE(oHD.satisfiesPreferences(*m_refPrefs));
	auto oPair = oHD.getCodeFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
	REQUIRE(oPair.second == "");
	oPair = oHD.getTitleFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
	REQUIRE(oPair.second == "");
}

TEST_CASE_METHOD(STFX<HiDefPreferencesFixture>, "Discriminator")
{
	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint;
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	HighscoresDefinition::Discriminator oDiscr;
	oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
	aDiscriminators.push_back(oDiscr);
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	HighscoresDefinition::ScoreElement oScoreEl;
	oScoreEl.m_sVarDescription = "Best time";
	oScoreEl.m_sVarName = "Time";
	oScoreEl.m_bLowestBest = true;
	aScoreElements.push_back(oScoreEl);
	HighscoresDefinition oHD(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	REQUIRE(oHD.getDiscriminators().size() == 1);
	REQUIRE(oHD.getEligibilitys().size() == 0);
	REQUIRE(oHD.getScoreElements().size() == 1);
	REQUIRE(oHD.getMaxScores() == 10);
	REQUIRE(oHD.satisfiesPreferences(*m_refPrefs));
	auto oPair = oHD.getCodeFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
	REQUIRE(oPair.second == "1");
	oPair = oHD.getTitleFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
	REQUIRE(oPair.second == "Teams: 1");
}

TEST_CASE_METHOD(STFX<HiDefPreferencesFixture>, "HighscoreConstraints")
{
	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint;
	aHighscoreConstraint.push_back({HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS, 2, 111});
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	HighscoresDefinition::Discriminator oDiscr;
	oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
	aDiscriminators.push_back(oDiscr);
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	HighscoresDefinition::ScoreElement oScoreEl;
	oScoreEl.m_sVarDescription = "Time";
	oScoreEl.m_sVarName = "Time";
	oScoreEl.m_bLowestBest = true;
	aScoreElements.push_back(oScoreEl);
	HighscoresDefinition oHD(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	REQUIRE_FALSE(oHD.satisfiesPreferences(*m_refPrefs));
	auto oPair = oHD.getCodeFromPreferences(*m_refPrefs);
	REQUIRE_FALSE(oPair.first);
	oPair = oHD.getTitleFromPreferences(*m_refPrefs);
	REQUIRE_FALSE(oPair.first);
}

TEST_CASE_METHOD(STFX<HiDefPreferencesFixture>, "Eligibilitys")
{
	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint;
	aHighscoreConstraint.push_back({HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS, 1, 1});
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	{
	HighscoresDefinition::Discriminator oDiscr;
	oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
	aDiscriminators.push_back(oDiscr);
	}
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	HighscoresDefinition::Eligibility oEligibility;
	oEligibility.m_sVarName = "Time";
	oEligibility.m_nMin = 1;
	oEligibility.m_nMax = 1;
	aEligibilitys.push_back(oEligibility);
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	HighscoresDefinition::ScoreElement oScoreEl;
	oScoreEl.m_sVarDescription = "Time";
	oScoreEl.m_sVarName = "Time";
	oScoreEl.m_bLowestBest = true;
	aScoreElements.push_back(oScoreEl);
	HighscoresDefinition oHD(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	REQUIRE(oHD.satisfiesPreferences(*m_refPrefs));
	auto oPair = oHD.getCodeFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
	oPair = oHD.getTitleFromPreferences(*m_refPrefs);
	REQUIRE(oPair.first);
}

TEST_CASE_METHOD(STFX<HiDefPreferencesFixture>, "Codes")
{
	std::vector<HighscoresDefinition::HighscoreConstraint> aHighscoreConstraint;
	std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
	HighscoresDefinition::Discriminator oDiscr;
	{
	HighscoresDefinition::Discriminator oDiscr;
	oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
	aDiscriminators.push_back(oDiscr);
	}
	{
	HighscoresDefinition::Discriminator oDiscr;
	oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_OPTION;
	oDiscr.m_eOptionOwnerType = OwnerType::GAME;
	oDiscr.m_sOptionName = "Difficulty";
	aDiscriminators.push_back(oDiscr);
	}
	std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
	std::vector<HighscoresDefinition::ScoreElement> aScoreElements;
	HighscoresDefinition::ScoreElement oScoreEl;
	oScoreEl.m_sVarDescription = "Best time";
	oScoreEl.m_sVarName = "Time";
	oScoreEl.m_bLowestBest = true;
	aScoreElements.push_back(oScoreEl);
	HighscoresDefinition oHD(m_refPrefs->getStdConfig(), aHighscoreConstraint, aDiscriminators, aEligibilitys, aScoreElements, 10);
	REQUIRE(oHD.isValidCode("1_Easy"));
	REQUIRE(oHD.isValidCode("2_Hard"));
	REQUIRE_FALSE(oHD.isValidCode("3_Hard"));
	REQUIRE_FALSE(oHD.isValidCode("0_Medium"));
	REQUIRE_FALSE(oHD.isValidCode("1_EASY"));
	REQUIRE_FALSE(oHD.isValidCode("1"));
	REQUIRE_FALSE(oHD.isValidCode("Easy_1"));
	REQUIRE_FALSE(oHD.isValidCode(""));
	REQUIRE_FALSE(oHD.isValidCode("2_"));
}

} // namespace testing

} // namespace stmg
