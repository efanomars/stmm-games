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
 * File:   fixturevariantHighscoresDefinition.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_TESTING_FIXTURE_VARIANT_HIGHSCORES_DEFINITION_H
#define STMG_TESTING_FIXTURE_VARIANT_HIGHSCORES_DEFINITION_H

#include "fixturevariantOptions.h"

#include <stmm-games/highscoresdefinition.h>
#include <stmm-games/ownertype.h>

#include <limits>
#include <string>
#include <vector>
#include <cassert>

#include <stdint.h>

namespace stmg
{

namespace testing
{

///////////////////////////////////////////////////////////////////////////////
class FixtureVariantHighscoresDefinitionDiscriminator_None //DEFAULT
{
};
class FixtureVariantHighscoresDefinitionDiscriminator_TotTeams
{
};
class FixtureVariantHighscoresDefinitionDiscriminator_GameOptionDifficulty
{
};
class FixtureVariantHighscoresDefinitionDiscriminator
{
public:
	virtual ~FixtureVariantHighscoresDefinitionDiscriminator() = default;
protected:
	std::vector<HighscoresDefinition::Discriminator> getHDDiscriminator() const
	{
		std::vector<HighscoresDefinition::Discriminator> aDiscriminators;
		if (dynamic_cast<FixtureVariantHighscoresDefinitionDiscriminator_TotTeams const*>(this) != nullptr) {
			HighscoresDefinition::Discriminator oDiscr;
			oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS;
			aDiscriminators.push_back(oDiscr);
		}
		if (dynamic_cast<FixtureVariantHighscoresDefinitionDiscriminator_GameOptionDifficulty const*>(this) != nullptr) {
			#ifndef NDEBUG
			const FixtureVariantOptions_Game_Difficulty* p0GameOptionDifficulty = dynamic_cast<FixtureVariantOptions_Game_Difficulty const*>(this);
			#endif
			assert(p0GameOptionDifficulty != nullptr);
			HighscoresDefinition::Discriminator oDiscr;
			oDiscr.m_eType = HighscoresDefinition::DISCRIMINATOR_TYPE_OPTION;
			oDiscr.m_sOptionName = "Difficulty";
			oDiscr.m_eOptionOwnerType = OwnerType::GAME;
			aDiscriminators.push_back(oDiscr);
		}
		return aDiscriminators;
	}
};

////////////////////////////////////////////////////////////////////////////////
class FixtureVariantHighscoresDefinitionConsraint_None // Default
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotTeams_ExactlyOne
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotTeams_ExactlyTwo
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotHumanTeams_ExactlyOne
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotHumanTeams_ExactlyTwo
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotAITeams_Zero
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotAITeams_MaxOne
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotAITeams_ExactlyOne
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotMatesHumanTeams_ExactlyOne
{
};
class FixtureVariantHighscoresDefinitionConsraint_TotMatesHumanTeams_ExactlyTwo
{
};
class FixtureVariantHighscoresDefinitionConsraint
{
public:
	virtual ~FixtureVariantHighscoresDefinitionConsraint() = default;
protected:
	std::vector<HighscoresDefinition::HighscoreConstraint> getGameTypeConstraints() const
	{
		std::vector<HighscoresDefinition::HighscoreConstraint> aConstraints;
		if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotTeams_ExactlyOne const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS;
			oConstr.m_nMin = 1;
			oConstr.m_nMax = 1;
			aConstraints.push_back(oConstr);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotTeams_ExactlyTwo const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS;
			oConstr.m_nMin = 2;
			oConstr.m_nMax = 2;
			aConstraints.push_back(oConstr);
		}
		//
		if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotHumanTeams_ExactlyOne const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS;
			oConstr.m_nMin = 1;
			oConstr.m_nMax = 1;
			aConstraints.push_back(oConstr);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotHumanTeams_ExactlyTwo const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS;
			oConstr.m_nMin = 2;
			oConstr.m_nMax = 2;
			aConstraints.push_back(oConstr);
		}
		//
		if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotAITeams_ExactlyOne const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS;
			oConstr.m_nMin = 1;
			oConstr.m_nMax = 1;
			aConstraints.push_back(oConstr);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotAITeams_MaxOne const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS;
			oConstr.m_nMin = 0;
			oConstr.m_nMax = 1;
			aConstraints.push_back(oConstr);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotAITeams_Zero const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS;
			oConstr.m_nMin = 0;
			oConstr.m_nMax = 0;
			aConstraints.push_back(oConstr);
		}
		//
		if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotMatesHumanTeams_ExactlyOne const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM;
			oConstr.m_nMin = 1;
			oConstr.m_nMax = 1;
			aConstraints.push_back(oConstr);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionConsraint_TotMatesHumanTeams_ExactlyTwo const*>(this) != nullptr) {
			HighscoresDefinition::HighscoreConstraint oConstr;
			oConstr.m_eType = HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM;
			oConstr.m_nMin = 2;
			oConstr.m_nMax = 2;
			aConstraints.push_back(oConstr);
		}
		return aConstraints;
	}
};
////////////////////////////////////////////////////////////////////////////////
class FixtureVariantHighscoresDefinitionInclude_None // Default
{
};
class FixtureVariantHighscoresDefinitionInclude_GameVarTime_BE1 // >= 1
{
};
class FixtureVariantHighscoresDefinitionInclude_GameVarTime_BE2 // >= 2
{
};
class FixtureVariantHighscoresDefinitionInclude_TeamVarPoints_BE0 // >= 0
{
};
class FixtureVariantHighscoresDefinitionInclude_TeamVarSysRank_0 // Winner
{
};
class FixtureVariantHighscoresDefinitionInclude
{
public:
	virtual ~FixtureVariantHighscoresDefinitionInclude() = default;
protected:
	std::vector<HighscoresDefinition::Eligibility> getHDIncludeConstraints() const
	{
		std::vector<HighscoresDefinition::Eligibility> aEligibilitys;
		if (dynamic_cast<FixtureVariantHighscoresDefinitionInclude_GameVarTime_BE1 const*>(this) != nullptr) {
			HighscoresDefinition::Eligibility oEligibility;
			oEligibility.m_sVarName = "Time";
			oEligibility.m_nMin = 1;
			oEligibility.m_nMax = std::numeric_limits<int32_t>::max();
			aEligibilitys.push_back(oEligibility);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionInclude_GameVarTime_BE2 const*>(this) != nullptr) {
			HighscoresDefinition::Eligibility oEligibility;
			oEligibility.m_sVarName = "Time";
			oEligibility.m_nMin = 2;
			oEligibility.m_nMax = std::numeric_limits<int32_t>::max();
			aEligibilitys.push_back(oEligibility);
		}
		if (dynamic_cast<FixtureVariantHighscoresDefinitionInclude_TeamVarPoints_BE0 const*>(this) != nullptr) {
			HighscoresDefinition::Eligibility oEligibility;
			oEligibility.m_sVarName = "Points";
			oEligibility.m_nMin = 0;
			oEligibility.m_nMax = std::numeric_limits<int32_t>::max();
			aEligibilitys.push_back(oEligibility);
		}
		if (dynamic_cast<FixtureVariantHighscoresDefinitionInclude_TeamVarSysRank_0 const*>(this) != nullptr) {
			HighscoresDefinition::Eligibility oEligibility;
			oEligibility.m_sVarName = "__Rank";
			oEligibility.m_nMin = 0;
			oEligibility.m_nMax = 0;
			aEligibilitys.push_back(oEligibility);
		}
		return aEligibilitys;
	}
};

////////////////////////////////////////////////////////////////////////////////
class FixtureVariantHighscoresDefinitionScoreElement_None // Default
{
};
class FixtureVariantHighscoresDefinitionScoreElement_GameVarTime_LowestWins
{
};
class FixtureVariantHighscoresDefinitionScoreElement_GameVarTime_HighestWins
{
};
class FixtureVariantHighscoresDefinitionScoreElement_TeamVarPoints
{
};
class FixtureVariantHighscoresDefinitionScoreElement
{
public:
	virtual ~FixtureVariantHighscoresDefinitionScoreElement() = default;
protected:
	std::vector<HighscoresDefinition::ScoreElement> getHDScoreElements() const
	{
		std::vector<HighscoresDefinition::ScoreElement> aScoreEls;
		if (dynamic_cast<FixtureVariantHighscoresDefinitionScoreElement_GameVarTime_LowestWins const*>(this) != nullptr) {
			HighscoresDefinition::ScoreElement oScoreEl;
			oScoreEl.m_sVarName = "Time";
			oScoreEl.m_sVarDescription = "Time";
			oScoreEl.m_bLowestBest = true;
			aScoreEls.push_back(oScoreEl);
		} else if (dynamic_cast<FixtureVariantHighscoresDefinitionScoreElement_GameVarTime_HighestWins const*>(this) != nullptr) {
			HighscoresDefinition::ScoreElement oScoreEl;
			oScoreEl.m_sVarName = "Time";
			oScoreEl.m_sVarDescription = "Time";
			oScoreEl.m_bLowestBest = false;
			aScoreEls.push_back(oScoreEl);
		}
		//
		if (dynamic_cast<FixtureVariantHighscoresDefinitionScoreElement_TeamVarPoints const*>(this) != nullptr) {
			HighscoresDefinition::ScoreElement oScoreEl;
			oScoreEl.m_sVarName = "Points";
			oScoreEl.m_sVarDescription = "Points";
			oScoreEl.m_bLowestBest = false;
			aScoreEls.push_back(oScoreEl);
		}
		return aScoreEls;
	}
};


} // namespace testing

} // namespace stmg

#endif	/* STMG_TESTING_FIXTURE_VARIANT_HIGHSCORES_DEFINITION_H */
