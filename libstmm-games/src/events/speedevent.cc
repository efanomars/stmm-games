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
 * File:   speedevent.cc
 */

#include "events/speedevent.h"

#include "level.h"

#include <vector>
#include <cassert>
#include <algorithm>

//#include <iostream>

#include <utility>

namespace stmg
{

SpeedEvent::SpeedEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
, m_nCurrentChange(0)
, m_nInChangeCounter(0)
, m_nSpeedCount(0)
{
	assert(m_oData.m_aSpeedChanges.size() > 0);
}

void SpeedEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_nCurrentChange = 0;
	m_nInChangeCounter = 0;
	m_nSpeedCount = 0;
	assert(m_oData.m_aSpeedChanges.size() > 0);
}

void SpeedEvent::trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* /*p0TriggeringEvent*/) noexcept
{
//std::cout << "SpeedEvent::trigger nMsg=" << nMsg << "  nValue=" << nValue << '\n';
	if (m_nCurrentChange >= static_cast<int32_t>(m_oData.m_aSpeedChanges.size())) {
		return;
	}
	Level& oLevel = level();
	++m_nSpeedCount;
	bool bChanged = false;
	SpeedChange& oChange = m_oData.m_aSpeedChanges[m_nCurrentChange];
	if (oChange.m_eIntervalChangeType != SPEED_EVENT_CHANGE_TYPE_NONE) {
		const int32_t nOldInterval = oLevel.getInterval();
		int32_t nInterval = nOldInterval;
		switch (oChange.m_eIntervalChangeType) {
			case SPEED_EVENT_CHANGE_TYPE_SET:
			{
				nInterval = oChange.m_nIntervalChange;
			}
			break;
			case SPEED_EVENT_CHANGE_TYPE_INC:
			{
				nInterval += oChange.m_nIntervalChange;
			}
			break;
			case SPEED_EVENT_CHANGE_TYPE_MULT_PERC:
			{
				nInterval *= oChange.m_nIntervalChange / 100;
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
		nInterval = std::max<int32_t>(nInterval, 1);
		oLevel.setInterval(nInterval);
		if (nInterval != nOldInterval) {
			bChanged = true;
			informListeners(LISTENER_GROUP_CHANGED_INTERVAL, nInterval - nOldInterval);
		}
	}
	if (oChange.m_eFallTicksChangeType != SPEED_EVENT_CHANGE_TYPE_NONE) {
		const int32_t nOldFallTicks = oLevel.getFallEachTicks();
		int32_t nFallTicks = nOldFallTicks;
		switch (oChange.m_eFallTicksChangeType) {
			case SPEED_EVENT_CHANGE_TYPE_SET:
			{
				nFallTicks = oChange.m_nFallTicksChange;
			}
			break;
			case SPEED_EVENT_CHANGE_TYPE_INC:
			{
				nFallTicks += oChange.m_nFallTicksChange;
			}
			break;
			case SPEED_EVENT_CHANGE_TYPE_MULT_PERC:
			{
				nFallTicks = nFallTicks * oChange.m_nFallTicksChange / 100;
			}
			break;
			default:
			{
				assert(false);
			}
			break;
		}
		nFallTicks = std::max(nFallTicks, 1);
		oLevel.setFallEachTicks(nFallTicks);
		if (nFallTicks != nOldFallTicks) {
			bChanged = true;
			informListeners(LISTENER_GROUP_CHANGED_FALL_EACH_TICKS, nFallTicks - nOldFallTicks);
		}
	}

	if (bChanged) {
		informListeners(LISTENER_GROUP_CHANGED, m_nSpeedCount);
	} else {
		informListeners(LISTENER_GROUP_NOT_CHANGED, m_nSpeedCount);
	}

	++m_nInChangeCounter;
	if ((oChange.m_nRepeat > 0) && (m_nInChangeCounter >= oChange.m_nRepeat)) {
		m_nInChangeCounter = 0;
		++m_nCurrentChange;
		if (m_nCurrentChange >= static_cast<int32_t>(m_oData.m_aSpeedChanges.size())) {
			informListeners(LISTENER_GROUP_FINISHED, 0);
		}
	}
}

} // namespace stmg
