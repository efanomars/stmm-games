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
 * File:   variableevent.cc
 */

#include "events/variableevent.h"

#include "variable.h"
#include "level.h"
#include "gameproxy.h"
#include "ownertype.h"

#include <cassert>
#include <algorithm>
//#include <iostream>
#include <utility>


namespace stmg
{

VariableEvent::VariableEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_nVarMate(oInit.m_nVarMate)
, m_nVarIndex(oInit.m_nVarIndex)
, m_nDefaultIncBy(oInit.m_nDefaultIncBy)
{
	initCommon(oInit.m_nVarTeam);
}

void VariableEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	//
	m_nVarMate = oInit.m_nVarMate;
	m_nVarIndex = oInit.m_nVarIndex;
	m_nDefaultIncBy = oInit.m_nDefaultIncBy;
	//
	initCommon(oInit.m_nVarTeam);
}
void VariableEvent::initCommon(int32_t nVarTeam) noexcept
{
//std::cout << "VariableEvent(" << reinterpret_cast<int64_t>(this) <<  ")::initCommon" << '\n';
	m_bOldValueInitialized = false;
	m_nOldValue = 0;
	//
	auto& oGame = level().game();
	assert((m_nVarMate < 0) || (nVarTeam >= 0)); // (nVarMate >= 0) implies (nVarTeam >= 0)
	const bool bATIOL = oGame.isAllTeamsInOneLevel();
	if (nVarTeam < 0) {
		assert(oGame.hasVariableId(m_nVarIndex, OwnerType::GAME));
		m_nVarLevel = -1;
		m_nVarLevelTeam = -1;
	} else {
		m_nVarLevel = (bATIOL ? 0 : nVarTeam);
		m_nVarLevelTeam = (bATIOL ? nVarTeam : 0);
		if (m_nVarMate < 0) {
			assert(oGame.hasVariableId(m_nVarIndex, OwnerType::TEAM));
		} else {
			assert(oGame.hasVariableId(m_nVarIndex, OwnerType::PLAYER));
		}
	}
	auto& oVar = oGame.variable(m_nVarIndex, m_nVarLevel, m_nVarLevelTeam, m_nVarMate);
	m_bTimeRelative = oVar.getType().m_bTimeRelative;
}
void VariableEvent::trigger(int32_t nMsg, int32_t nValue, Event* /*p0TriggeringEvent*/) noexcept
{
//if (!m_bTimeRelative) {
//std::cout << "VariableEvent::trigger adr=" << reinterpret_cast<int64_t>(this) << "  nMsg=" << nMsg << "  nValue=" << nValue << '\n';
//std::cout << "                       m_nVarLevel=" << m_nVarLevel << "  m_nVarLevelTeam=" << m_nVarLevelTeam << "  m_nVarMate=" << m_nVarMate << '\n';
//std::cout << "                       m_nVarIndex=" << m_nVarIndex << '\n';
//}
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	auto& oVar = oGame.variable(m_nVarIndex, m_nVarLevel, m_nVarLevelTeam, m_nVarMate);
	const int32_t nCurValue = oVar.get();
	if (!m_bOldValueInitialized) {
		m_nOldValue = nCurValue;
		m_bOldValueInitialized = true;
	}
	int32_t nNewValue = nCurValue;
	switch (nMsg) {
		case MESSAGE_SET_VALUE:
		{
			nNewValue = nValue;
		}
		break;
		case MESSAGE_ADD_VALUE:
		{
			nNewValue += nValue;
		}
		break;
		case MESSAGE_INC:
		{
			nNewValue += 1;
		}
		break;
		case MESSAGE_SUB_VALUE:
		{
			nNewValue -= nValue;
		}
		break;
		case MESSAGE_DEC:
		{
			nNewValue -= 1;
		}
		break;
		case MESSAGE_ADD_PERC_VALUE:
		{
			nNewValue = nNewValue * (100 + nValue) / 100;
		}
		break;
		case MESSAGE_SUB_PERC_VALUE:
		{
			nNewValue = nNewValue * (100 - nValue) / 100;
		}
		break;
		case MESSAGE_MUL_BY_VALUE:
		{
			nNewValue = nNewValue * nValue;
		}
		break;
		case MESSAGE_DIV_BY_VALUE:
		{
			if (nValue == 0) {
				nNewValue = 0;
			} else {
				nNewValue = nNewValue / nValue;
			}
		}
		break;
		case MESSAGE_NEG_ADD_VALUE:
		{
			nNewValue = nValue - nNewValue;
		}
		break;
		case MESSAGE_NEG_SUB_VALUE:
		{
			nNewValue = - nValue - nNewValue;
		}
		break;
		case MESSAGE_MIN_VALUE:
		{
			nNewValue = std::min(nValue, nNewValue);
		}
		break;
		case MESSAGE_MAX_VALUE:
		{
			nNewValue = std::max(nValue, nNewValue);
		}
		break;
		case MESSAGE_RESET:
		{
			nNewValue = 0;
		}
		break;
		case MESSAGE_VAR_GET_VALUE:
		{
			informListeners(LISTENER_GROUP_VAR_VALUE, nCurValue);
			return; //----------------------------------------------------------
		}
		break;
		default:
		{
			nNewValue += m_nDefaultIncBy;
		}
	}
//std::cout << "                       nCurValue=" << nCurValue << "  nNewValue=" << nNewValue << '\n';
	if (nNewValue != nCurValue) {
		oVar.set(nNewValue);
	}
	if (m_nOldValue != nNewValue) {
//std::cout << "VariableEvent::trigger  m_nOldValue=" << m_nOldValue << "  nNewValue=" << nNewValue << '\n';
		m_nOldValue = nNewValue;
		informListeners(LISTENER_GROUP_VAR_CHANGED, nNewValue);
	}
	if (m_bTimeRelative) {
		const int32_t nGameTick = oGame.gameElapsed();
		oLevel.activateEvent(this, nGameTick + 1);
	}
}

} // namespace stmg
