/*
 * File:   selectevent.cc
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

#include "events/selectevent.h"
#include "level.h"

#include "gameproxy.h"
#include "ownertype.h"
#include "variable.h"

#include <cassert>
//#include <iostream>
#include <utility>

namespace stmg
{

SelectEvent::SelectEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_nVarMate(oInit.m_nVarMate)
, m_nVarIndex(oInit.m_nVarIndex)
{
	initCommon(oInit.m_nVarTeam);
}

void SelectEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	//
	m_nVarMate = oInit.m_nVarMate;
	m_nVarIndex = oInit.m_nVarIndex;
	//
	initCommon(oInit.m_nVarTeam);
}
void SelectEvent::initCommon(int32_t nVarTeam) noexcept
{
	if (m_nVarIndex < 0) {
		assert((nVarTeam < 0) && (m_nVarMate < 0));
	} else {
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
	}
}

void SelectEvent::trigger(int32_t nMsg, int32_t nValue, Event* /*p0TriggeringEvent*/) noexcept
{
//std::cout << "SelectEvent::trigger nValue=" << nValue << '\n';
	if (m_nVarIndex < 0) {
		if (nValue >= 0) {
			informListeners(nValue, nMsg);
		}
	} else {
		Level& oLevel = level();
		auto& oGame = oLevel.game();
		auto& oVar = oGame.variable(m_nVarIndex, m_nVarLevel, m_nVarLevelTeam, m_nVarMate);
		const int32_t nVarValue = oVar.get();
//std::cout << "SelectEvent::trigger nVarValue=" << nVarValue << '\n';
		if (nVarValue >= 0) {
			informListeners(nVarValue, nValue);
		}
	}
}

} // namespace stmg
