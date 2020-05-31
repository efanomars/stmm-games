/*
 * File:   mockevent.cc
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

#include "mockevent.h"

#ifdef STMG_TESTING_COPY_FROM_FAKE
#include "level.h"
#include "event.h"
#include "gameproxy.h"
#include "variable.h"
#else
#include <stmm-games/level.h>
#include <stmm-games/event.h>
#include <stmm-games/gameproxy.h>
#include <stmm-games/variable.h>
#endif

#include <utility>
#include <cassert>
//#include <iostream>


namespace stmg
{

MockEvent::MockEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_bTriggerValuesSet(false)
, m_nGroup(-1)
, m_nValue(-1)
{
}

void MockEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_bTriggerValuesSet = false;
	m_nGroup = -1;
	m_nValue = -1;
}

void MockEvent::setTriggerValue(int32_t nGroup, int32_t nValue, int32_t nSkipTicks) noexcept
{
	m_nGroup = nGroup;
	m_nValue = nValue;
	const int32_t nGameTick = level().game().gameElapsed();
	level().activateEvent(this, nGameTick + nSkipTicks);
}
void MockEvent::setVariable(int32_t nVarId, int32_t nTeam, int32_t nMate, int32_t nValue) noexcept
{
	assert(nVarId >= 0);
	assert((nMate < 0) || (nTeam >= 0));
	VarData oVarData;
	oVarData.m_nVarId = nVarId;
	oVarData.m_nTeam = nTeam;
	oVarData.m_nMate = nMate;
	oVarData.m_nValue = nValue;
	m_aVarDatas.push_back(std::move(oVarData));
}

void MockEvent::trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
	auto& oGame = level().game();
	const bool bATIOL = oGame.isAllTeamsInOneLevel();
	for (const auto& oVarData : m_aVarDatas) {
		Variable& oVar = oGame.variable(oVarData.m_nVarId, (bATIOL ? 0 : oVarData.m_nTeam), (bATIOL ? oVarData.m_nTeam : 0), oVarData.m_nMate);
		oVar.set(oVarData.m_nValue);
	}
	if (p0TriggeringEvent != nullptr) {
		return;
	}
	informListeners(m_nGroup, m_nValue);
}

} // namespace stmg
