/*
 * File:   sysevent.cc
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

#include "events/sysevent.h"
#include "level.h"

#include "apppreferences.h"
#include "gameproxy.h"
#include "variable.h"

#include <vector>
#include <cassert>
//#include <iostream>
#include <memory>
#include <string>
#include <utility>


namespace stmg
{

SysEvent::SysEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}

void SysEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	commonInit();
}
void SysEvent::commonInit() noexcept
{
	Level& oLevel = level();
	GameProxy& oGame = oLevel.game();
	assert((m_oData.m_nLevelTeam >= 0) && (m_oData.m_nLevelTeam < oLevel.getTotLevelTeams()));
	assert((m_oData.m_eType >= SYS_EVENT_TYPE_LEVEL_COMPLETED) && (m_oData.m_eType <= SYS_EVENT_TYPE_PLAYER_OUT));

	m_nGameVarIdGameOverTime = oGame.getGameVarIdGameOverTime();
	m_nTeamVarIdFinishedTime = oGame.getTeamVarIdFinishedTime();
	assert(m_nGameVarIdGameOverTime >= 0);
	const int32_t nPrefTeam = oGame.isAllTeamsInOneLevel() ? m_oData.m_nLevelTeam : oLevel.getLevel();
	const shared_ptr<AppPreferences::PrefTeam> refTeam  = oLevel.prefs().getTeam(nPrefTeam);
	m_nTotTeammates = refTeam->getTotMates();
}
void SysEvent::trigger(int32_t nMsg, int32_t /*nValue*/, Event* /*p0TriggeringEvent*/) noexcept
{
	Level& oLevel = level();
	Variable& oVarGameOverTime = oLevel.game().variable(m_nGameVarIdGameOverTime);
	const int32_t nGameOverTime = oVarGameOverTime.get();
	bool bDone = false;
	switch (m_oData.m_eType)
	{
	case SYS_EVENT_TYPE_LEVEL_COMPLETED:
		{
//std::cout << "LEVEL COMPLETED levelteam=" << m_nLevelTeam << '\n';
			bDone = oLevel.gameStatusCompleted(m_oData.m_nLevelTeam, m_oData.m_bFinishIfPossible, m_oData.m_bCreateTexts);
		}
		break;
	case SYS_EVENT_TYPE_LEVEL_FAILED:
		{
//std::cout << "LEVEL FAILED levelteam=" << m_nLevelTeam << '\n';
			bDone = oLevel.gameStatusFailed(m_oData.m_nLevelTeam, m_oData.m_bFinishIfPossible, m_oData.m_bCreateTexts);
		}
		break;
	case SYS_EVENT_TYPE_PLAYER_OUT:
		{
			const int32_t nMate = nMsg;
			if (nMate < 0) {
				oLevel.gameStatusTechnical(std::vector<std::string>{"SysEvent::trigger", "SYS_EVENT_TYPE_PLAYER_OUT: negative mate (nMsg) ???"});
			} else if (nMate < m_nTotTeammates) {
//std::cout << "LAYER OUT levelteam=" << m_nLevelTeam << "  mate=" << nMate << '\n';
				bDone = oLevel.gameStatusPlayerOut(m_oData.m_nLevelTeam, nMate, m_oData.m_bFinishIfPossible, m_oData.m_bCreateTexts);
			}
		}
		break;
	default:
		{
			assert(false);
		}
		break;
	}
	if (bDone) {
		Variable& oVarTeamFinished = oLevel.variable(m_nTeamVarIdFinishedTime, m_oData.m_nLevelTeam, -1);
		const int32_t nTeamFinishedTime = oVarTeamFinished.get();
		if (nTeamFinishedTime >= 0) {
			informListeners(LISTENER_GROUP_TEAM_FINISHED, 0);
		}
		const int32_t nNewGameOverTime = oVarGameOverTime.get();
		if (nGameOverTime != nNewGameOverTime) {
			assert(nGameOverTime < 0);
			informListeners(LISTENER_GROUP_GAME_OVER, nNewGameOverTime);
		}
	}
}

} // namespace stmg
