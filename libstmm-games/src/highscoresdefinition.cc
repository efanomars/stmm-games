/*
 * File:   highscoresdefinition.cc
 *
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

#include "highscoresdefinition.h"

#include "game.h"
#include "appconfig.h"
#include "appconstraints.h"
#include "apppreferences.h"
#include "option.h"
#include "util/namedobjindex.h"
#include "util/util.h"
#include "util/variant.h"

#include <iostream>
#include <cassert>
#include <algorithm>
#include <cstdint>

namespace stmg
{

HighscoresDefinition::HighscoresDefinition(const shared_ptr<AppConfig>& refAppConfig
											, const std::vector<HighscoreConstraint>& aHighscoreConstraints
											, const std::vector<Discriminator>& aDiscriminators
											, const std::vector<Eligibility>& aEligibilitys
											, const std::vector<ScoreElement>& aScoreElements, int32_t nMaxScores) noexcept
: m_refAppConfig(refAppConfig)
, m_aHighscoreConstraints(aHighscoreConstraints)
, m_aDiscriminators(aDiscriminators)
, m_aEligibilitys(aEligibilitys)
, m_aScoreElements(aScoreElements)
, m_nMaxScores(nMaxScores)
{
	assert(refAppConfig);
	assert(aScoreElements.size() > 0);
	assert(nMaxScores > 0);
	checkParams();
}

void HighscoresDefinition::reInit(const shared_ptr<AppConfig>& refAppConfig
								, const std::vector<HighscoreConstraint>& aHighscoreConstraints
								, const std::vector<Discriminator>& aDiscriminators
								, const std::vector<Eligibility>& aEligibilitys
								, const std::vector<ScoreElement>& aScoreElements, int32_t nMaxScores) noexcept
{
	assert(refAppConfig);
	assert(aScoreElements.size() > 0);
	assert(nMaxScores > 0);
	m_refAppConfig = refAppConfig;
	m_aHighscoreConstraints = aHighscoreConstraints;
	m_aDiscriminators = aDiscriminators;
	m_aEligibilitys = aEligibilitys;
	m_aScoreElements = aScoreElements;
	m_nMaxScores = nMaxScores;
	checkParams();
}
void HighscoresDefinition::checkParams() const noexcept
{
	#ifndef NDEBUG
	for (const auto& oC : m_aHighscoreConstraints) {
		assert(oC.m_nMin >= 0);
		assert(oC.m_nMin <= oC.m_nMax);
	}
	for (const auto& oI : m_aEligibilitys) {
		assert(!oI.m_sVarName.empty());
		assert(oI.m_nMin >= 0);
		assert(oI.m_nMin <= oI.m_nMax);
	}
	for (const auto& oS : m_aScoreElements) {
		assert(!oS.m_sVarName.empty());
	}
	#endif //NDEBUG
}
#ifndef NDEBUG
void HighscoresDefinition::dump() noexcept
{
	std::cout << "HighscoresDefinition::dump():" << '\n';
	std::cout << "    Max scores= " << m_nMaxScores << '\n';
	for (const auto& oHighscoreConstraint : m_aHighscoreConstraints) {
		const std::string sConstraintType = [](HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE eType)
		{
			if (eType == HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS) {
				return "TOT_TEAMS";
			} else if (eType == HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS) {
				return "TOT_HUMAN_TEAMS";
			} else if (eType == HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS) {
				return "TOT_AI_TEAMS";
			} else if (eType == HighscoresDefinition::HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM) {
				return "TOT_MATES_OF_HUMAN_TEAM";
			} else {
				return "ERROR";
			}
		}(oHighscoreConstraint.m_eType);
		std::cout << "    Constraint  type= " << sConstraintType
					<< "  min=" << oHighscoreConstraint.m_nMin << "  max=" << oHighscoreConstraint.m_nMax << '\n';
	}
	for (const auto& oDiscriminator : m_aDiscriminators) {
		const std::string sDiscriminatorType = [](HighscoresDefinition::DISCRIMINATOR_TYPE eType)
		{
			if (eType == HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_TEAMS) {
				return "TOT_TEAMS";
			} else if (eType == HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_HUMAN_TEAMS) {
				return "TOT_HUMAN_TEAMS";
			} else if (eType == HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_AI_TEAMS) {
				return "TOT_AI_TEAMS";
			} else if (eType == HighscoresDefinition::DISCRIMINATOR_TYPE_TOT_MATES_OF_HUMAN_TEAM) {
				return "TOT_MATES_OF_HUMAN_TEAM";
			} else if (eType == HighscoresDefinition::DISCRIMINATOR_TYPE_OPTION) {
				return "OPTION";
			} else {
				return "ERROR";
			}
		}(oDiscriminator.m_eType);
		std::cout << "    Discriminator  type= " << sDiscriminatorType;
		if (oDiscriminator.m_eType == HighscoresDefinition::DISCRIMINATOR_TYPE_OPTION) {
			std::cout << "  option name: " << oDiscriminator.m_sOptionName;
		}
		std::cout << '\n';
	}
	for (const auto& oEligibility : m_aEligibilitys) {
		std::cout << "    Eligibility  variable name: " << oEligibility.m_sVarName
					<< "  min=" << oEligibility.m_nMin << "  max=" << oEligibility.m_nMax << '\n';
	}
	for (const auto& oScoreElement : m_aScoreElements) {
		std::cout << "    ScoreElement  variable name: " << oScoreElement.m_sVarName
					<< "  lowest best=" << (oScoreElement.m_bLowestBest ? "True" : "False")
					<< "  desc: " << oScoreElement.m_sVarDescription << '\n';
	}
}
#endif //NDEBUG

bool HighscoresDefinition::preferencesSatisfyConstraints(const AppPreferences& oAppPreferences) const noexcept
{
	assert(m_refAppConfig == oAppPreferences.getAppConfig());
	const int32_t nTotTeams = oAppPreferences.getTotTeams();
	const int32_t nTotHumanTeams = oAppPreferences.getTotHumanTeams();
	const int32_t nTotAITeams = nTotTeams - nTotHumanTeams;
	for (auto& oHighscoreConstraints : m_aHighscoreConstraints) {
		switch(oHighscoreConstraints.m_eType) {
		case HIGHSCORE_CONSTRAINT_TYPE_TOT_TEAMS:
			if ((nTotTeams < oHighscoreConstraints.m_nMin) || (nTotTeams > oHighscoreConstraints.m_nMax)) {
				return false; //--------------------------------------------
			}
			break;
		case HIGHSCORE_CONSTRAINT_TYPE_TOT_HUMAN_TEAMS:
			if ((nTotHumanTeams < oHighscoreConstraints.m_nMin) || (nTotHumanTeams > oHighscoreConstraints.m_nMax)) {
				return false; //--------------------------------------------
			}
			break;
		case HIGHSCORE_CONSTRAINT_TYPE_TOT_AI_TEAMS:
			if ((nTotAITeams < oHighscoreConstraints.m_nMin) || (nTotAITeams > oHighscoreConstraints.m_nMax)) {
				return false; //--------------------------------------------
			}
			break;
		case HIGHSCORE_CONSTRAINT_TYPE_TOT_MATES_OF_HUMAN_TEAM:
			for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
				auto refTeam = oAppPreferences.getTeam(nTeam);
				if (refTeam->isAI()) {
					continue; // for(nTeam) ------
				}
				const int32_t nTotMates = refTeam->getTotMates();
				if ((nTotMates < oHighscoreConstraints.m_nMin) || (nTotMates > oHighscoreConstraints.m_nMax)) {
					return false; //----------------------------------------
				}
			}
			break;
		default:
			assert(false);
			return false; //------------------------------------------------
		}
	}
	return true;
}
std::pair<bool, std::string> HighscoresDefinition::getStringFromPreferences(const AppPreferences& oAppPreferences, bool bCode) const noexcept
{
//std::cout << "getStringFromPreferences 0" << '\n';
	assert(m_refAppConfig == oAppPreferences.getAppConfig());
	std::string sDiscriminationString = "";
	const int32_t nTotTeams = oAppPreferences.getTotTeams();
	for (const auto& oDiscriminator : m_aDiscriminators) {
		std::string sOptionValue;
		switch (oDiscriminator.m_eType) {
		case DISCRIMINATOR_TYPE_OPTION:
			{
				const auto& sOptionName = oDiscriminator.m_sOptionName;
				if (oDiscriminator.m_eOptionOwnerType == OwnerType::GAME) {
					auto refOption = isMasterOrEnabledSlaveOption(oAppPreferences, sOptionName, OwnerType::GAME, -1, -1);
					if (! refOption) {
						// The slave option is not enabled
						// a highscore file shouldn't be created for the current preferences
						return {false, ""}; //----------------------------------
					}
					const Variant oValue = oAppPreferences.getOptionValue(sOptionName);
					if (bCode) {
						sOptionValue = refOption->getValueAsCode(oValue);
					} else {
						sOptionValue = refOption->getDesc() + ": " + refOption->getValueAsDescriptive(oValue);
					}
				} else if (oDiscriminator.m_eOptionOwnerType == OwnerType::TEAM) {
					// There has to be exactly one team with the given option enabled
					int32_t nEnabledPrefTeam = -1;
					shared_ptr<Option> refEnabledOption;
					for (int32_t nPrefTeam = 0; nPrefTeam < nTotTeams; ++nPrefTeam) {
						auto refOption = isMasterOrEnabledSlaveOption(oAppPreferences, sOptionName, OwnerType::TEAM, nPrefTeam, -1);
						if (refOption) {
							if (nEnabledPrefTeam >= 0) {
								// already set, more than one enabled option
								return {false, ""}; //--------------------------
							}
							nEnabledPrefTeam = nPrefTeam;
							refEnabledOption = refOption;
						}
					}
					if (nEnabledPrefTeam < 0) {
						// no team has the option enabled
						return {false, ""}; //----------------------------------
					}
					assert(refEnabledOption);
					const Variant oValue = oAppPreferences.getTeam(nEnabledPrefTeam)->getOptionValue(sOptionName);
					if (bCode) {
						sOptionValue = refEnabledOption->getValueAsCode(oValue);
					} else {
						sOptionValue = refEnabledOption->getDesc() + ": " + refEnabledOption->getValueAsDescriptive(oValue);
					}
				} else if (oDiscriminator.m_eOptionOwnerType == OwnerType::PLAYER) {
					// There has to be exactly one player with the given option enabled in the entire game
					const int32_t nTotPlayers = oAppPreferences.getTotPlayers();
					int32_t nEnabledPrefPlayer = -1;
					shared_ptr<Option> refEnabledOption;
					for (int32_t nPrefPlayer = 0; nPrefPlayer < nTotPlayers; ++nPrefPlayer) {
						auto refPrefPlayer = oAppPreferences.getPlayer(nPrefPlayer);
						const int32_t nPrefTeam = refPrefPlayer->getTeam()->get();
						const int32_t nMate = refPrefPlayer->getMate();
						auto refOption = isMasterOrEnabledSlaveOption(oAppPreferences, sOptionName, OwnerType::PLAYER, nPrefTeam, nMate);
						if (refOption) {
							if (nEnabledPrefPlayer >= 0) {
								// already set, more than one enabled option
								return {false, ""}; //--------------------------
							}
							nEnabledPrefPlayer = nPrefPlayer;
							refEnabledOption = refOption;
						}
					}
					if (nEnabledPrefPlayer < 0) {
						// no team has the option enabled
						return {false, ""}; //----------------------------------
					}
					assert(refEnabledOption);
					const Variant oValue = oAppPreferences.getPlayer(nEnabledPrefPlayer)->getOptionValue(sOptionName);
					if (bCode) {
						sOptionValue = refEnabledOption->getValueAsCode(oValue);
					} else {
						sOptionValue = refEnabledOption->getDesc() + ": " + refEnabledOption->getValueAsDescriptive(oValue);
					}
				} else {
					assert(false);
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_TEAMS:
			{
				sOptionValue = std::to_string(nTotTeams);
				if (!bCode) {
					sOptionValue = "Teams: " + sOptionValue;
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_HUMAN_TEAMS:
			{
				sOptionValue = std::to_string(oAppPreferences.getTotHumanTeams());
				if (!bCode) {
					sOptionValue = "Human teams: " + sOptionValue;
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_AI_TEAMS:
			{
				sOptionValue = std::to_string(oAppPreferences.getTotAITeams());
				if (!bCode) {
					sOptionValue = "AI teams: " + sOptionValue;
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_MATES_OF_HUMAN_TEAM:
			{
				const int32_t nTotHumanTeams = oAppPreferences.getTotHumanTeams();
				if (nTotHumanTeams != 1) {
					// There can only be exactly one human team
					return {false, ""}; //--------------------------------------
				}
				// find it
				shared_ptr<AppPreferences::PrefTeam> refTeam;
				for (int32_t nTeam = 0; nTeam < nTotTeams; ++nTeam) {
					refTeam = oAppPreferences.getTeam(nTeam);
					if (!refTeam->isAI()) {
						break; // for----
					}
				}
				assert(refTeam);
				const int32_t nTotMates = refTeam->getTotMates();
				sOptionValue = std::to_string(nTotMates);
				if (!bCode) {
					sOptionValue = "Mates: " + sOptionValue;
				}
			} break;
		default:
			assert(false);
		}
		if (!sDiscriminationString.empty()) {
			if (bCode) {
				sDiscriminationString += "_";
			} else {
				sDiscriminationString += ", ";
			}
		}
		sDiscriminationString += sOptionValue;
	}

	return {true, sDiscriminationString};
}
bool HighscoresDefinition::isValidCode(const std::string& sCode) const noexcept
{
//std::cout << "HighscoresDefinition::isValidCode sCode=" << sCode << '\n';
	const auto& oAppConfig = *m_refAppConfig;

	std::size_t nCurPos = 0;
	for (const auto& oDiscriminator : m_aDiscriminators) {
		auto nEndPos = sCode.find('_', nCurPos);
		bool bTheEnd = false;
		if (nEndPos == std::string::npos) {
			nEndPos = sCode.size();
			bTheEnd = true;
		}
		const std::string sDisCode = sCode.substr(nCurPos, nEndPos - nCurPos);
		nCurPos = nEndPos + (bTheEnd ? 0 : 1);
		//
		switch (oDiscriminator.m_eType) {
		case DISCRIMINATOR_TYPE_OPTION:
			{
				auto refOption = oAppConfig.getOption(oDiscriminator.m_eOptionOwnerType, oDiscriminator.m_sOptionName);
				if (!refOption) {
					return false; //--------------------------------------------
				}
				const auto oPair = refOption->getValueFromCode(sDisCode);
				if (! oPair.second.empty()) {
					return false; //--------------------------------------------
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_TEAMS:
			{
				const auto& oAppConstraints = oAppConfig.getAppConstraints();
				const int32_t nTeamsMax = oAppConstraints.getMaxTeams();
				const int32_t nTeamsMin = oAppConstraints.m_nTeamsMin;
				auto pPairValue = Util::strToNumber<int32_t>(sDisCode, false, true, nTeamsMin, true, nTeamsMax);
				if (! pPairValue.second.empty()) {
					return false; //--------------------------------------------
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_HUMAN_TEAMS:
			{
				const auto& oAppConstraints = oAppConfig.getAppConstraints();
				const int32_t nTeamsMax = oAppConstraints.getMaxTeams();
				auto pPairValue = Util::strToNumber<int32_t>(sDisCode, false, true, 0, true, nTeamsMax);
				if (! pPairValue.second.empty()) {
					return false; //--------------------------------------------
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_AI_TEAMS:
			{
				const auto& oAppConstraints = oAppConfig.getAppConstraints();
				const int32_t nTeamsMax = oAppConstraints.getMaxTeams();
				const bool bAllowsAI = oAppConstraints.allowsAI();
				auto pPairValue = Util::strToNumber<int32_t>(sDisCode, false, true, 0, true, (bAllowsAI ? nTeamsMax : 0));
				if (! pPairValue.second.empty()) {
					return false; //--------------------------------------------
				}
			} break;
		case DISCRIMINATOR_TYPE_TOT_MATES_OF_HUMAN_TEAM:
			{
				const auto& oAppConstraints = oAppConfig.getAppConstraints();
				const int32_t nTotMatesPerTeamMax = oAppConstraints.getMaxTeammates();
				auto pPairValue = Util::strToNumber<int32_t>(sDisCode, false, true, 1, true, nTotMatesPerTeamMax);
				if (! pPairValue.second.empty()) {
					return false; //--------------------------------------------
				}
			} break;
		default:
			assert(false);
		}
	}
	return true;
}
shared_ptr<Option> HighscoresDefinition::isMasterOrEnabledSlaveOption(const AppPreferences& oAppPreferences
																	, const std::string& sOptionName, OwnerType eOwner
																	, int32_t nPrefTeam, int32_t nMate) const noexcept
{
	const auto& refAppConfig = oAppPreferences.getAppConfig();
	const auto& oNamedIdx = refAppConfig->getOptions(eOwner);
	const int32_t nOptionIdx = oNamedIdx.getIndex(sOptionName);
	assert(nOptionIdx >= 0);
	auto refOption = oNamedIdx.getObj(nOptionIdx);
	const auto& aMastersValues = refOption->getMastersValues();
	if (! aMastersValues.empty()) {
		// it's a slave option
		// to be enabled it must satisfy all its masters
		for (const auto& oPair : aMastersValues) {
			const auto& refMasterOption = oPair.first;
			const std::string& sMasterOptionName = refMasterOption->getName();
			const Variant oMasterValue = [&]() {
				switch (eOwner)	{
				case OwnerType::GAME:
				{
					return oAppPreferences.getOptionValue(sMasterOptionName);
				}
				case OwnerType::TEAM:
				{
					assert(nPrefTeam >= 0);
					return oAppPreferences.getTeam(nPrefTeam)->getOptionValue(sMasterOptionName);
				}
				case OwnerType::PLAYER:
				{
					assert((nPrefTeam >= 0) && (nMate >= 0));
					return oAppPreferences.getTeam(nPrefTeam)->getMate(nMate)->getOptionValue(sMasterOptionName);
				}
				default:
					assert(false);
					return Variant{};
				}
			}();
			const auto& aMasterValues = oPair.second;
			auto itFind = std::find(aMasterValues.begin(), aMasterValues.end(), oMasterValue);
			if (itFind == aMasterValues.end()) {
				// The slave option is not enabled
				// a highscore file shouldn't be created for the current preferences
				return shared_ptr<Option>{}; //---------------------------------
			}
		}
	}
	return refOption;
}
std::string HighscoresDefinition::isCompatibleWithGame(const Game& oGame) const noexcept
{
	for (const auto& oEligibility : m_aEligibilitys) {
		const auto oPair = oGame.variableIdAndOwner(oEligibility.m_sVarName);
		const int32_t nVarId = oPair.first;
		if (nVarId < 0) {
			const std::string sError{"HighscoreDefinition: Eligibility variable '" + oEligibility.m_sVarName + "' not found"};
			return sError; //---------------------------------------------------
		}
	}
	for (const auto& oScoreElement : m_aScoreElements) {
		const auto oPair = oGame.variableIdAndOwner(oScoreElement.m_sVarName);
		const int32_t nVarId = oPair.first;
		if (nVarId < 0) {
			const std::string sError{"HighscoreDefinition: ScoreElement variable '" + oScoreElement.m_sVarName + "' not found"};
			return sError; //---------------------------------------------------
		}
	}
	return "";
}
bool HighscoresDefinition::Discriminator::operator==(const Discriminator& oD) const noexcept
{
	if (oD.m_eType != m_eType) {
		return false;
	}
	if (oD.m_sOptionName != m_sOptionName) {
		return false;
	}
	if (oD.m_eOptionOwnerType != m_eOptionOwnerType) {
		return false;
	}
	return true;
}
bool HighscoresDefinition::HighscoreConstraint::operator==(const HighscoreConstraint& oHC) const noexcept
{
	if (oHC.m_eType != m_eType) {
		return false;
	}
	if (oHC.m_nMin != m_nMin) {
		return false;
	}
	if (oHC.m_nMax != m_nMax) {
		return false;
	}
	return true;
}
bool HighscoresDefinition::Eligibility::operator==(const Eligibility& oE) const noexcept
{
	if (oE.m_sVarName != m_sVarName) {
		return false;
	}
	if (oE.m_nMin != m_nMin) {
		return false;
	}
	if (oE.m_nMax != m_nMax) {
		return false;
	}
	return true;
}
bool HighscoresDefinition::ScoreElement::operator==(const ScoreElement& oSE) const noexcept
{
	if (! (oSE.m_bLowestBest == m_bLowestBest)) {
		return false;
	}
	if (! (oSE.m_sVarName == m_sVarName)) {
		return false;
	}
	return true;
}

bool HighscoresDefinition::operator==(const HighscoresDefinition& oHS) const noexcept
{
	if (&oHS == this) {
		return true;
	}
	if (!(m_aDiscriminators == oHS.m_aDiscriminators)) {
		return false;
	}
	if (!(m_aEligibilitys == oHS.m_aEligibilitys)) {
		return false;
	}
	if (!(m_aHighscoreConstraints == oHS.m_aHighscoreConstraints)) {
		return false;
	}
	if (!(m_aScoreElements == oHS.m_aScoreElements)) {
		return false;
	}
	if (!(m_refAppConfig == oHS.m_refAppConfig)) {
		return false;
	}
	//if (!(m_nMaxScores == oHS.m_nMaxScores)) {
	//	return false;
	//}
	return true;
}

} // namespace stmg
