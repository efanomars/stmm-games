/*
 * File:   arrayevent.cc
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

#include "events/arrayevent.h"

#include "variable.h"

#include "level.h"
#include "gameproxy.h"
#include "ownertype.h"

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <memory>
#include <utility>


namespace stmg
{

ArrayEvent::ArrayEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}
void ArrayEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	commonInit();
}
void ArrayEvent::commonInit() noexcept
{
	auto& oGame = level().game();
	const bool bATIOL = oGame.isAllTeamsInOneLevel();

	int32_t nTotValues = 1;
	int32_t nValueDimensions = 0;
	for (Dimension& oDim : m_oData.m_aDimensions) {
		assert(oDim.m_nSize > 0);
		nTotValues = nTotValues * oDim.m_nSize;
		if (oDim.m_nVarIndex < 0) {
			++nValueDimensions;
			continue; // for oDim
		}
		int32_t nVarLevel;
		int32_t nVarLevelTeam;
		assert((oDim.m_nVarMate < 0) || (oDim.m_nVarTeam >= 0)); // (nVarMate >= 0) implies (nVarTeam >= 0)
		if (oDim.m_nVarTeam < 0) {
			assert(oGame.hasVariableId(oDim.m_nVarIndex, OwnerType::GAME));
			nVarLevel = -1;
			nVarLevelTeam = -1;
		} else {
			nVarLevel = (bATIOL ? 0 : oDim.m_nVarTeam);
			nVarLevelTeam = (bATIOL ? oDim.m_nVarTeam : 0);
			if (oDim.m_nVarMate < 0) {
				assert(oGame.hasVariableId(oDim.m_nVarIndex, OwnerType::TEAM));
			} else {
				assert(oGame.hasVariableId(oDim.m_nVarIndex, OwnerType::PLAYER));
			}
		}
		auto& oVar = oGame.variable(oDim.m_nVarIndex, nVarLevel, nVarLevelTeam, oDim.m_nVarMate);
		oDim.m_p0Variable = &oVar;
	}
	assert(nValueDimensions <= 1);

	assert(static_cast<int32_t>(m_oData.m_aValues.size()) == nTotValues);
}
void ArrayEvent::trigger(int32_t nMsg, int32_t nValue, Event* /*p0TriggeringEvent*/) noexcept
{
//std::cout << "ArrayEvent::trigger() nGameTick=" << nGameTick << " nGameMillisec=" << nGameMillisec << " nMsg=" << nMsg << " nValue=" << nValue << " p0TriggeringEvent=" << reinterpret_cast<int64_t>(p0TriggeringEvent) << '\n';
	if ((nMsg < MESSAGE_ARRAY_GET) || (nMsg > MESSAGE_ARRAY_GET_5)) {
		return; //--------------------------------------------------------------
	}
	const int32_t nChannel = nMsg - MESSAGE_ARRAY_GET;
	int32_t nReturnValue = m_oData.m_nDefaultValue;
	int32_t nTotalIdx = 0;
	const int32_t nTotDimensions = static_cast<int32_t>(m_oData.m_aDimensions.size());
	for (int32_t nCurDim = 0; nCurDim < nTotDimensions; ++nCurDim) {
		auto& oDim = m_oData.m_aDimensions[nCurDim];
		const int32_t nIdx = ((oDim.m_p0Variable == nullptr) ? nValue : oDim.m_p0Variable->get());
		if ((nIdx < 0) || (nIdx >= oDim.m_nSize)) {
			nTotalIdx = -1;
			break;
		}
		nTotalIdx = nTotalIdx * oDim.m_nSize + nIdx;
	}
	if (nTotalIdx >= 0) {
		nReturnValue = m_oData.m_aValues[nTotalIdx];
	}
	informListeners(LISTENER_GROUP_ARRAY_RESULT + nChannel, nReturnValue);
}

} // namespace stmg
