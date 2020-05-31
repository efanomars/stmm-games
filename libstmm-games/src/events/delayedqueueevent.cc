/*
 * File:   delayedqueueevent.cc
 *
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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

#include "events/delayedqueueevent.h"

#include "level.h"
#include "gameproxy.h"

#include <cassert>
#include <algorithm>
//#include <iostream>
#include <utility>


namespace stmg
{

DelayedQueueEvent::DelayedQueueEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}
void DelayedQueueEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	commonInit();
}
void DelayedQueueEvent::commonInit() noexcept
{
	assert(m_oData.m_nMaxQueueSize >= 1);

	m_nIntervalTicks = m_oData.m_nInitialIntervalTicks;
	m_nIntervalMillisecs = m_oData.m_nInitialIntervalMillisecs;
	m_nTargetTick = -1;
}
int32_t DelayedQueueEvent::getCurrentInterval(const Level& oLevel) const noexcept
{
	return std::max(1, static_cast<int32_t>(m_nIntervalTicks + m_nIntervalMillisecs / oLevel.game().gameInterval()));
}
void DelayedQueueEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
	Level& oLevel = level();
	const int32_t nTimer = oLevel.game().gameElapsed();
//std::cout << "DelayedQueueEvent::trigger() nTimer=" << nTimer << "  nMsg=" << nMsg << " nValue=" << nValue << " p0TriggeringEvent=" << reinterpret_cast<int64_t>(p0TriggeringEvent) << '\n';
	if (p0TriggeringEvent == nullptr) {
		if (! m_aQueue.empty()) {
			assert(m_nTargetTick >= nTimer);
			nValue = m_aQueue.front();
			m_aQueue.pop_front();
			m_nTargetTick = nTimer + getCurrentInterval(oLevel);
//std::cout << "DelayedQueueEvent::trigger() SET m_nTargetTick = " << m_nTargetTick << '\n';
			if (! m_aQueue.empty()) {
				oLevel.activateEvent(this, m_nTargetTick);
			}
			informListeners(LISTENER_GROUP_VALUE, nValue);
		} else {
			// initisl start: do nothing
		}
		return; //--------------------------------------------------------------
	}
	switch (nMsg) {
	case MESSAGE_QUEUE_VALUE:
	{
//std::cout << "DelayedQueueEvent::trigger() m_aQueue.size() = " << m_aQueue.size() << '\n';
		if (static_cast<int32_t>(m_aQueue.size()) == m_oData.m_nMaxQueueSize) {
			assert(m_nTargetTick >= nTimer);
			oLevel.activateEvent(this, m_nTargetTick);
			informListeners(LISTENER_GROUP_OVERFLOW, nValue);
		} else {
			m_aQueue.push_back(nValue);
			if (m_nTargetTick < nTimer) {
				m_nTargetTick = nTimer;
			}
			oLevel.activateEvent(this, m_nTargetTick);
		}
	}
	break;
	case MESSAGE_SET_INTERVAL_TICKS:
	{
		m_nIntervalTicks = nValue;
		if (m_nTargetTick >= nTimer) {
			oLevel.activateEvent(this, m_nTargetTick);
		}
	}
	break;
	case MESSAGE_SET_INTERVAL_MILLISECS:
	{
		m_nIntervalMillisecs = nValue;
		if (m_nTargetTick >= nTimer) {
			oLevel.activateEvent(this, m_nTargetTick);
		}
	}
	break;
	case MESSAGE_SET_INITIAL_INTERVAL:
	{
		m_nIntervalTicks = m_oData.m_nInitialIntervalTicks;
		m_nIntervalMillisecs = m_oData.m_nInitialIntervalMillisecs;
		if (m_nTargetTick >= nTimer) {
			oLevel.activateEvent(this, m_nTargetTick);
		}
	}
	break;
	case MESSAGE_EMPTY_QUEUE:
	{
		m_aQueue.clear();
	}
	break;
	default:
	{
		// do nothing
	}
	}
}

} // namespace stmg
