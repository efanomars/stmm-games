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
 * File:   highscore.cc
 */

#include "highscore.h"
#include "game.h"

#include "appconfig.h"
#include "appconstraints.h"
#include "highscoresdefinition.h"
#include "level.h"
#include "ownertype.h"

#include <cassert>
//#include <iostream>
#include <algorithm>
#include <cstdint>
#include <utility>



namespace stmg
{

Highscore::Highscore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
					, const std::string& sCodeString, const std::string& sTitleString) noexcept
{
	reInitInternal(refHighscoresDefinition, sCodeString, sTitleString, std::vector<Score>{});
}
Highscore::Highscore(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
					, const std::string& sCodeString, const std::string& sTitleString
					, const std::vector<Score>& aScores) noexcept
{
	assert(refHighscoresDefinition->isValidCode(sCodeString));
	reInitInternal(refHighscoresDefinition, sCodeString, sTitleString, aScores);
}

void Highscore::reInit(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
						, const std::string& sCodeString, const std::string& sTitleString) noexcept
{
	reInitInternal(refHighscoresDefinition, sCodeString, sTitleString, std::vector<Score>{});
}
void Highscore::reInit(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
						, const std::string& sCodeString, const std::string& sTitleString
						, const std::vector<Score>& aScores) noexcept
{
	assert(refHighscoresDefinition->isValidCode(sCodeString));
	reInitInternal(refHighscoresDefinition, sCodeString, sTitleString, aScores);
}
void Highscore::reInitInternal(const shared_ptr<HighscoresDefinition>& refHighscoresDefinition
								, const std::string& sCodeString, const std::string& sTitleString
								, const std::vector<Score>& aScores) noexcept
{
	assert(refHighscoresDefinition);
	assert(sCodeString.empty() || refHighscoresDefinition->isValidCode(sCodeString));
	assert(static_cast<int32_t>(aScores.size()) <= refHighscoresDefinition->getMaxScores());
	m_refHighscoresDefinition = refHighscoresDefinition;
	m_sCodeString = sCodeString;
	m_sTitleString = sTitleString;
	m_aScores = aScores;
	#ifndef NDEBUG
	for (auto& oScore : aScores) {
		assert(! oScore.m_sTeam.empty());
		assert(refHighscoresDefinition->getScoreElements().size() == oScore.m_aValues.size());
	}
	#endif
}

std::vector<int32_t> Highscore::includeGame(const Game& oGame) noexcept
{
	return includeGame(oGame, true);
}
std::vector<int32_t> Highscore::includeGame(const Game& oGame, bool bCheckElegibility) noexcept
{
	for (auto& oTeamScore : m_aScores) {
		oTeamScore.m_nTeam = -1;
	}
	std::vector<int32_t> aPositions;
	//assert(!oGame.isRunning());
	const auto& oAppPrefs = oGame.prefs();
	if (bCheckElegibility) {
		assert(m_refHighscoresDefinition->satisfiesPreferences(oAppPrefs));
	}
	const bool bAllTeamsInOneLevel = oGame.isAllTeamsInOneLevel();
	const int32_t nTotLevels = oGame.getTotLevels();
//std::cout << "Highscore::includeGame nTotLevels = " << nTotLevels << '\n';
	for (int32_t nLevel = 0; nLevel < nTotLevels; ++nLevel) {
//std::cout << "Highscore::includeGame    nLevel=" << nLevel << '\n';
		auto& refLevel = oGame.level(nLevel);
		const int32_t nTotLevelTeams = refLevel->getTotLevelTeams();
//std::cout << "Highscore::includeGame   nTotLevelTeams = " << nTotLevelTeams << '\n';
		for (int32_t nLevelTeam = 0; nLevelTeam < nTotLevelTeams; ++nLevelTeam) {
//std::cout << "Highscore::includeGame    nLevelTeam=" << nLevelTeam << '\n';
			const int32_t nPrefTeam = (bAllTeamsInOneLevel ? nLevelTeam : nLevel);
			auto refTeam = oAppPrefs.getTeam(nPrefTeam);
			if (bCheckElegibility) {
				if (refTeam->isAI()) {
					// Only human teams get into highscores
					continue; // for(nLevelTeam)
				}
				const bool bIsTeamEligible = isTeamIncluded(nLevel, nLevelTeam, *refTeam, oGame);
				if (! bIsTeamEligible) {
//std::cout << "Highscore::includeGame    nLevel=" << nLevel << "  nLevelTeam " << nLevelTeam << " " << refTeam->getName() << " NOT included" << '\n';
					continue; // for(nLevelTeam)
				}
			}
			const int32_t nPos = insertInScores(nLevel, nLevelTeam, *refTeam, oGame);
//std::cout << "Highscore::includeGame    nPos=" << nPos << '\n';
			if (nPos >= 0) {
				const int32_t nMaxScores = m_refHighscoresDefinition->getMaxScores();
				auto it = aPositions.begin();
				while (it != aPositions.end()) {
					int32_t& nCurPos = *it;
					if (nCurPos >= nPos) {
						++nCurPos;
						if (nCurPos >= nMaxScores) {
							it = aPositions.erase(it);
							continue; // while
						}
					}
					++it;
				}
				aPositions.push_back(nPos);
			}
		}
	}
	return aPositions;
}

bool Highscore::isTeamIncluded(int32_t nLevel, int32_t nLevelTeam
								, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) const noexcept
{
//std::cout << "Highscore::isTeamIncluded" << '\n';
	const Variable& oFinishedVar = oGame.variable(oGame.getTeamVarIdFinishedTime(), nLevel, nLevelTeam, -1);
	if (oFinishedVar.get() < 0) {
//std::cout << "Highscore::isTeamIncluded team still playing???" << '\n';
		// team still playing
		return false; //--------------------------------------------------------
	}
	const int32_t nTotMates = oPrefTeam.getTotMates();
	const auto& aEligibilitys = m_refHighscoresDefinition->getEligibilitys();
	for (auto& oEligibility : aEligibilitys) {
		const auto oPair = oGame.variableIdAndOwner(oEligibility.m_sVarName);
		const int32_t nVarId = oPair.first;
		const OwnerType eOwnerType = oPair.second;
		int32_t nVarLevelTeam = nLevelTeam;
		int32_t nVarMate = -1;
		if (eOwnerType == OwnerType::GAME) {
			nVarLevelTeam = -1;
		} else if (eOwnerType == OwnerType::PLAYER) {
			for (nVarMate = 0; nVarMate < nTotMates; ++nVarMate) {
				if (! oPrefTeam.getMate(nVarMate)->isAI()) {
					break; // for (nVarMate)
				}
			}
			// since the team is human at least one mate has to be human.
			assert(nVarMate < nTotMates);
		}
		assert(nVarId >= 0); // Use getHighscoresDefinition()->isCompatibleWithGame() to prevent errors
		const Variable& oVar = oGame.variable(nVarId, nLevel, nVarLevelTeam, nVarMate);
		const int32_t nValue = oVar.get();
		if ((nValue < oEligibility.m_nMin) || (nValue > oEligibility.m_nMax)) {
//std::cout << "Highscore::isTeamIncluded Var not in min max " << nValue << '\n';
			return false; //----------------------------------------------------
		}
	}
	return true;
}
Highscore::Score Highscore::createScore(int32_t nLevel, int32_t nLevelTeam
							, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) const noexcept
{
	const int32_t nTotMates = oPrefTeam.getTotMates();
	Score oNewScore;
	// set its values
	const auto& aScoreElements = m_refHighscoresDefinition->getScoreElements();
	for (auto& oScoreElement : aScoreElements) {
		const auto oPair = oGame.variableIdAndOwner(oScoreElement.m_sVarName);
		const int32_t nVarId = oPair.first;
		const OwnerType eOwnerType = oPair.second;
		int32_t nVarLevelTeam = nLevelTeam;
		int32_t nVarMate = -1;
		if (eOwnerType == OwnerType::GAME) {
			// This really only makes sense if there is just one (human) team playing.
			nVarLevelTeam = -1;
		} else if (eOwnerType == OwnerType::PLAYER) {
			// The first human mate's value determines the score even if the
			// team has more than one mate!
			for (nVarMate = 0; nVarMate < nTotMates; ++nVarMate) {
				if (! oPrefTeam.getMate(nVarMate)->isAI()) {
					break; // for (nVarMate)
				}
			}
			if (nVarMate == nTotMates) {
				// this is an AI team. Just take first mate of team.
				nVarMate = 0;
			}
		}
		assert(nVarId >= 0);
		const Variable& oVar = oGame.variable(nVarId, nLevel, nVarLevelTeam, nVarMate);
		oNewScore.m_aValues.push_back(oVar.getValue());
	}
	return oNewScore;
}
int32_t Highscore::findNewScorePosition(const Score& oTheScore) const noexcept
{
	// The last score is the one the position of which has to be determined.
	const int32_t nMaxScores = m_refHighscoresDefinition->getMaxScores();
	const int32_t nTotScores = static_cast<int32_t>(m_aScores.size());
//std::cout << "Highscore::findNewScorePosition  nMaxScores=" << nMaxScores << "  nTotScores=" << nTotScores << '\n';
	const auto& aScoreElements = m_refHighscoresDefinition->getScoreElements();
	int32_t nCurPos = nTotScores - 1;
	for ( ; nCurPos >= 0; --nCurPos) {
		const Score& oCurScore = m_aScores[nCurPos];
		const int32_t nTotScoreElements = static_cast<int32_t>(aScoreElements.size());
		bool bCurIsBetter = false;
		int32_t nScoreElementIdx = 0;
		for ( ; nScoreElementIdx < nTotScoreElements; ++nScoreElementIdx) {
			auto& oScoreElement = aScoreElements[nScoreElementIdx];
			assert(oCurScore.m_aValues[nScoreElementIdx].getFormat() == oTheScore.m_aValues[nScoreElementIdx].getFormat());
			const int32_t nCurValue = oCurScore.m_aValues[nScoreElementIdx].get();
			const int32_t nTheValue = oTheScore.m_aValues[nScoreElementIdx].get();
//std::cout << "Highscore::findNewScorePosition nScoreElementIdx=" << nScoreElementIdx << " nCurValue=" << nCurValue << " nTheValue=" << nTheValue << '\n';
			if (oScoreElement.m_bLowestBest) {
				if (nCurValue < nTheValue) {
//std::cout << "Highscore::findNewScorePosition oScoreElement.m_bLowestBest prec better" << '\n';
					bCurIsBetter = true;
					break; // for(nScoreElementIdx)
				} else if (nCurValue > nTheValue) {
					break; // for(nScoreElementIdx)
				}
			} else {
				if (nCurValue > nTheValue) {
//std::cout << "Highscore::findNewScorePosition !oScoreElement.m_bLowestBest prec better" << '\n';
					bCurIsBetter = true;
					break; // for(nScoreElementIdx)
				} else if (nCurValue < nTheValue) {
					break; // for(nScoreElementIdx)
				}
			}
		}
		if ((nScoreElementIdx < nTotScoreElements) && bCurIsBetter) {
			// oCurScore is better than oTheScore, stop
			break; // for (nNewPos)
		}
		// oCurScore is equal or worse than oTheScore, try preceding
	}
	++nCurPos;
	if (nCurPos >= nMaxScores) {
		nCurPos = -1;
	}
	return nCurPos;
}
int32_t Highscore::insertInScores(int32_t nLevel, int32_t nLevelTeam
								, const AppPreferences::PrefTeam& oPrefTeam, const Game& oGame) noexcept
{
//std::cout << "Highscore::insertInScores    nLevelTeam " << nLevelTeam << '\n';
	int32_t nNewPos = -1;
	{
		Score oNewScore = createScore(nLevel, nLevelTeam, oPrefTeam, oGame);
		nNewPos = findNewScorePosition(oNewScore);
		if (nNewPos < 0) {
			// didn't make it into the highscores
			return -1; //-------------------------------------------------------
		}
		m_aScores.insert(m_aScores.begin() + nNewPos, std::move(oNewScore));
	}
	//
	const int32_t nTotMates = oPrefTeam.getTotMates();
	// fill Score with missing data
	Score& oNewScore = m_aScores[nNewPos];
	oNewScore.m_nTeam = oPrefTeam.get();
	const auto& oAppPrefs = oGame.prefs();
	const auto& oAppConstraints = oAppPrefs.getAppConfig()->getAppConstraints();
	if ((oAppConstraints.getMaxTeams() == 1) || (nTotMates == 1)) {
		for (int32_t nMate = 0; nMate < nTotMates; ++nMate) {
			auto refMate = oPrefTeam.getMate(nMate);
			if (refMate->isAI()) {
				continue; // for (nMate)
			}
			if (! oNewScore.m_sTeam.empty()) {
				oNewScore.m_sTeam += "+";
			}
			oNewScore.m_sTeam += refMate->getName();
		}
	} else {
		oNewScore.m_sTeam = oPrefTeam.getName();
	}
	const int32_t nMaxScores = m_refHighscoresDefinition->getMaxScores();
	const int32_t nTotScores = static_cast<int32_t>(m_aScores.size());
	if (nTotScores > nMaxScores) {
		m_aScores.pop_back();
	}
	return nNewPos;
}
int32_t Highscore::positionIfIncluded(const Game& oGame, int32_t nTeam) const noexcept
{
	const bool bAllTeamsInOneLevel = oGame.isAllTeamsInOneLevel();
	const auto& oAppPrefs = oGame.prefs();
	auto refPrefTeam = oAppPrefs.getTeam(nTeam);
	const int32_t nLevel = (bAllTeamsInOneLevel ? 0 : nTeam);
	const int32_t nLevelTeam = (bAllTeamsInOneLevel ? nTeam : 0);
	Score oNewScore = createScore(nLevel, nLevelTeam, *refPrefTeam, oGame);
	const int32_t nNewPos = findNewScorePosition(oNewScore);
	return nNewPos;
}

} // namespace stmg
