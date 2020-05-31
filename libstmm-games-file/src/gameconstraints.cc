/*
 * File:   gameconstraints.cc
 *
 * Copyright © 2019  Stefano Marsili, <stemars@gmx.ch>
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

#include "gameconstraints.h"

#include "appconstraints.h"
#include "apppreferences.h"
#include "prefselector.h"

#include <cassert>

namespace stmg
{

void GameConstraints::initFromAppConstraints(const AppConstraints& oAppConstraints) noexcept
{
	assert(oAppConstraints.isValid());
	AppConstraints& oThis = *this;
	oThis = oAppConstraints;
	// Choose All Teams In One Level as default
	m_nLevelsMin = m_nTeamsMin;
	m_nLevelsMax = m_nTeamsMin;
	m_nTeamsPerLevelMin = m_nTeamsMin;
	m_nTeamsPerLevelMax = m_nTeamsMin;
	// No AI per default
	m_nAIMatesPerTeamMax = 0;
	m_bAllowMixedAIHumanTeam = false;
	m_nAITeamsMin = 0;
	m_nAITeamsMax = 0;
	//
	m_nMatesPerTeamMin = 1;
	m_nPlayersMin = m_nTeamsMin;
	assert(GameConstraints::isValid());
}

bool GameConstraints::isValid() const noexcept
{
	if (! AppConstraints::isValid()) {
		return false;
	}
	if (! ((m_nLevelsMin >= 1) && (m_nLevelsMin <= m_nLevelsMax)) ) {
		return false;
	}
	if (! ((m_nTeamsPerLevelMin >= 1) && (m_nTeamsPerLevelMin <= m_nTeamsPerLevelMax)) ) {
		return false;
	}
	if (! ((m_nMatesPerTeamMin >= 1) && (m_nMatesPerTeamMin <= m_nMatesPerTeamMax)) ) {
		return false;
	}
	if (! ((m_nPlayersMin >= 1) && (m_nPlayersMin <= m_nPlayersMax)) ) {
		return false;
	}
	if (! ((m_nAITeamsMin >= 0) && (m_nAITeamsMin <= m_nAITeamsMax)) ) {
		return false;
	}
	if (m_nTeamsMax < m_nLevelsMin) {
		return false;
	}
	if (m_nTeamsMax < m_nTeamsPerLevelMin) {
		return false;
	}
	if (m_nTeamsMax < m_nAITeamsMin) {
		return false;
	}
	if (m_nPlayersMax < m_nTeamsMin * m_nMatesPerTeamMin) {
		return false;
	}
	if (m_nTeamsMax * m_nMatesPerTeamMax < m_nPlayersMin) {
		return false;
	}

	const bool bMultiLevel = (m_nLevelsMax > 1);
	const bool bMultiTeamPerLevel = (m_nTeamsPerLevelMax > 1);
	if (bMultiLevel && bMultiTeamPerLevel) {
		return false;
	}
	return true;
}

bool GameConstraints::isSelectedBy(const AppPreferences& oAppPreferences) const noexcept
{
	if (! GameConstraints::isValid()) {
		return false; //--------------------------------------------------------
	}
	const int32_t nTotTeams = oAppPreferences.getTotTeams();
	const int32_t nTotPlayers = oAppPreferences.getTotPlayers();
	if ((nTotTeams < m_nTeamsMin) || (nTotTeams > m_nTeamsMax)) {
		return false; //--------------------------------------------------------
	}
	if (! ((nTotPlayers >= m_nPlayersMin) && (nTotPlayers <= m_nPlayersMax))) {
		return false; //--------------------------------------------------------
	}
	if (nTotTeams == 1) {
		// can be both bOneTeamPerLevel or bAllTeamsInOneLevel
		if (! ((nTotPlayers >= m_nMatesPerTeamMin) && (nTotPlayers <= m_nMatesPerTeamMax))) {
			return false; //----------------------------------------------------
		}
		if (m_nTeamsPerLevelMin > 1) {
			return false; //----------------------------------------------------
		}
		if (m_nAITeamsMin > 1) {
			return false; //----------------------------------------------------
		}
		if (m_nLevelsMin > 1) {
			return false; //----------------------------------------------------
		}
	} else {
		const int32_t nTotAITeams = oAppPreferences.getTotAITeams();
		if (! ((nTotAITeams >= m_nAITeamsMin) && (nTotAITeams <= m_nAITeamsMax))) {
			return false; //----------------------------------------------------
		}
		const bool bAllTeamsInOneLevel = (m_nLevelsMax == 1);
		const int32_t nTotLevels = (bAllTeamsInOneLevel ? 1 : nTotTeams);
		assert(nTotLevels >= 1);
		for (int32_t nLevel = 0; nLevel < nTotLevels; ++nLevel) {
			const int32_t nTotLevelTeams = (bAllTeamsInOneLevel ? nTotTeams : 1);
			if (! ((nTotLevelTeams >= m_nTeamsPerLevelMin) && (nTotLevelTeams <= m_nTeamsPerLevelMax))) {
				return false; //----------------------------------------------------
			}
			for (int32_t nTeam = 0; nTeam < nTotLevelTeams; ++nTeam) {
				const shared_ptr<AppPreferences::PrefTeam>& refPrefTeam = oAppPreferences.getTeam(bAllTeamsInOneLevel ? nTeam : nLevel);
				//oAppPreferences.getTeam(bAllTeamsInOneLevel, nLevel, nTeam);
				const int32_t nTotTeammates = refPrefTeam->getTotMates();
				if (! ((nTotTeammates >= m_nMatesPerTeamMin) && (nTotTeammates <= m_nMatesPerTeamMax))) {
					return false; //--------------------------------------------
				}
				int32_t nTotTeamAIMates = 0;
				for (int32_t nMate = 0; nMate < nTotTeammates; ++nMate) {
					const shared_ptr<AppPreferences::PrefPlayer>& refPrefPlayer = refPrefTeam->getMate(nMate);
					if (refPrefPlayer->isAI()) {
						++nTotTeamAIMates;
					}
				}
				if (nTotTeamAIMates > m_nAIMatesPerTeamMax) {
					return false; //--------------------------------------------
				}
			}
		}
	}
	if (m_refPrefSelector) {
		return m_refPrefSelector->select(oAppPreferences);
	}
	return true;
}

bool GameConstraints::isCompatibleWith(const AppConstraints& oAppConstraints) const noexcept
{
	if (! GameConstraints::isValid()) {
		return false; //--------------------------------------------------------
	}
	if (! oAppConstraints.isValid()) {
		return false; //--------------------------------------------------------
	}
	const int32_t nAppMinTeams = oAppConstraints.m_nTeamsMin;
	const int32_t nAppMaxTeams = oAppConstraints.getMaxTeams();
	const int32_t nAppMaxMates = oAppConstraints.getMaxTeammates();
	const int32_t nAppMaxPlayers = oAppConstraints.getMaxPlayers();
	const bool bAppAllowsAI = oAppConstraints.allowsAI();
	if (m_nTeamsMax < nAppMinTeams) {
		return false; //--------------------------------------------------------
	}
	if (m_nTeamsMin > nAppMaxTeams) {
		return false; //--------------------------------------------------------
	}
	if (m_nLevelsMin > nAppMaxTeams) {
		return false; //--------------------------------------------------------
	}
	if (m_nMatesPerTeamMin > nAppMaxMates) {
		return false; //--------------------------------------------------------
	}
	if (m_nPlayersMin > nAppMaxPlayers) {
		return false; //--------------------------------------------------------
	}
	if ((m_nAITeamsMin > 0) && !bAppAllowsAI) {
		return false; //--------------------------------------------------------
	}
	return true;
}

	//bool GameConstraints::operator==(const GameConstraints& oGC) const
	//{
	//	if (! AppConstraints::operator==(oGC)) {
	//		return false;
	//	}
	//	if (oGC.m_nLevelsMin != m_nLevelsMin) {
	//		return false;
	//	}
	//	if (oGC.m_nLevelsMax != m_nLevelsMax) {
	//		return false;
	//	}
	//	if (oGC.m_nTeamsPerLevelMin != m_nTeamsPerLevelMin) {
	//		return false;
	//	}
	//	if (oGC.m_nTeamsPerLevelMax != m_nTeamsPerLevelMax) {
	//		return false;
	//	}
	//	if (oGC.m_nMatesPerTeamMin != m_nMatesPerTeamMin) {
	//		return false;
	//	}
	//	if (oGC.m_nPlayersMin != m_nPlayersMin) {
	//		return false;
	//	}
	//	if (oGC.m_nAITeamsMin != m_nAITeamsMin) {
	//		return false;
	//	}
	//	if (oGC.m_nAITeamsMax != m_nAITeamsMax) {
	//		return false;
	//	}
	//	return true;
	//}

} // namespace stmg
