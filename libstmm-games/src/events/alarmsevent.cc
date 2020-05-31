/*
 * File:   alarmsevent.cc
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

#include "events/alarmsevent.h"

#include "level.h"
#include "gameproxy.h"

#include <vector>
#include <cassert>
#include <algorithm>
//#include <iostream>
#include <memory>
#include <utility>


namespace stmg
{

AlarmsEvent::AlarmsEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	commonInit();
}
void AlarmsEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	commonInit();
}
void AlarmsEvent::commonInit() noexcept
{
	#ifndef NDEBUG
	checkParams();
	#endif //NDEBUG
	m_bStepping = false;
	m_nTimeoutStartTicks = -1;
	m_nTimeoutStartMillisec = -1;
	m_nLastHandledTick = -1;
	m_nTimeoutTicks = -1;
	m_nTimeoutMillisec = -1;
	m_nCurrentChange = -1; // Marks as never started

}
void AlarmsEvent::checkParams() noexcept
{
	#ifndef NDEBUG
	bool bFirst = false;
	for (const auto& oAlarmTimeout : m_oData.m_aAlarmTimeouts) {
		if (bFirst) {
			bFirst = true;
			switch (oAlarmTimeout.m_eAlarmTimeoutType) {
				case ALARMS_EVENT_SET_TICKS:
					assert(oAlarmTimeout.m_nChange > 0);
				break;
				case ALARMS_EVENT_SET_MILLISEC:
					assert(oAlarmTimeout.m_nChange > 0);
				break;
				default:
					assert(false);
				break;
			}
		} else {
			switch (oAlarmTimeout.m_eAlarmTimeoutType) {
				case ALARMS_EVENT_SET_TICKS:
					assert(oAlarmTimeout.m_nChange > 0);
				break;
				case ALARMS_EVENT_INC_TICKS:
				break;
				case ALARMS_EVENT_SET_MILLISEC:
					assert(oAlarmTimeout.m_nChange > 0);
				break;
				case ALARMS_EVENT_INC_MILLISEC:
				break;
				case ALARMS_EVENT_MULT_PERC:
					assert(oAlarmTimeout.m_nChange > 0);
				break;
				default:
					assert(false);
				break;
			}
		}
	}
	#endif //NDEBUG
}
void AlarmsEvent::reactivate(int32_t nGameTick) noexcept
{
	const bool bAlreadyTicked = (m_nLastHandledTick == nGameTick);
	if (! bAlreadyTicked) {
		m_nLastHandledTick = nGameTick;
	}
	if (m_nTimeoutTicks < 0) {
		level().activateEvent(this, nGameTick + (bAlreadyTicked ? 1 : 0));
	} else {
		level().activateEvent(this, m_nTimeoutStartTicks + m_nTimeoutTicks);
	}
}
void AlarmsEvent::alreadyActive(int32_t nGameTick) noexcept
{
	reactivate(nGameTick);
	informListeners(LISTENER_GROUP_TIMEOUT_ACTIVE, 0);
}
void AlarmsEvent::incChangeCounter(int32_t nCurTimeoutRepeat) noexcept
{
	++m_nInChangeCounter;
	if ((nCurTimeoutRepeat > 0) && (m_nInChangeCounter >= nCurTimeoutRepeat)) {
		m_nInChangeCounter = 0;
		++m_nCurrentChange;
		assert(m_nCurrentChange <= static_cast<int32_t>(m_oData.m_aAlarmTimeouts.size()));
	}
}
void AlarmsEvent::trigger(int32_t nMsg, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept
{
	Level& oLevel = level();
	const auto& oGame = oLevel.game();
	const int32_t nGameTick = oGame.gameElapsed();
	const int32_t nGameMillisec = oGame.gameElapsedMillisec();
//std::cout << "AlarmsEvent::trigger() nGameTick=" << nGameTick << " nGameMillisec=" << nGameMillisec << " nMsg=" << nMsg << " nValue=" << nValue << " p0TriggeringEvent=" << reinterpret_cast<int64_t>(p0TriggeringEvent) << '\n';
	bool bTimeout = false;
	if (p0TriggeringEvent == nullptr) {
		assert(nMsg == 0);
		if (m_bStepping) {
			assert(m_nTimeoutStartTicks >= 0);
			assert(m_nTimeoutStartMillisec >= 0);
//std::cout << "AlarmsEvent::trigger TIMEOUT m_nCurrentChange=" << m_nCurrentChange << " m_nInChangeCounter=" << m_nInChangeCounter << '\n';
			m_nLastHandledTick = nGameTick;
			if (m_nTimeoutMillisec >= 0) {
//std::cout << "AlarmsEvent::trigger m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << " m_nTimeoutMillisec=" << m_nTimeoutMillisec << '\n';
				if (nGameMillisec >= m_nTimeoutStartMillisec + m_nTimeoutMillisec) {
					bTimeout = true;
				} else {
//std::cout << "AlarmsEvent::trigger DELAYED m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << '\n';
					oLevel.activateEvent(this, nGameTick + 1);
				}
			} else {
				bTimeout = true;
			}
		} else {
			// Initial start
			assert(m_nTimeoutStartTicks < 0);
			assert(m_nTimeoutStartMillisec < 0);
			//assert(nValue == 0);
			nMsg = MESSAGE_ALARMS_RESTART;
		}
		//
	} else if (nMsg == MESSAGE_ALARMS_FINISH) {
//std::cout << "AlarmsEvent::trigger() FINISH" << '\n';
		if (m_bStepping) {
			m_bStepping = false;
			m_nTimeoutStartTicks = -1;
			m_nTimeoutStartMillisec = -1;
			m_nCurrentChange = 0; // marks as finished
			informListeners(LISTENER_GROUP_FINISHED, 0);
		}
		return; //--------------------------------------------------------------
	}
	if (nMsg == MESSAGE_ALARMS_RESTART) {
		if (m_nTimeoutStartTicks >= 0) {
			alreadyActive(nGameTick);
			return; //----------------------------------------------------------
		}
		m_bStepping = true;
		m_nCurrentChange = 0;
		m_nInChangeCounter = 0;
		m_nTimeoutCounter = 0;
		m_nTimeoutStartTicks = nGameTick;
		m_nTimeoutStartMillisec = nGameMillisec;
		m_nLastHandledTick = nGameTick;
		AlarmTimeout& oTimeout = m_oData.m_aAlarmTimeouts[m_nCurrentChange];
		const int32_t nInitial = m_oData.m_aAlarmTimeouts[m_nCurrentChange].m_nChange;
		incChangeCounter(oTimeout.m_nRepeat); // point to the next timeout
//std::cout << "AlarmsEvent::trigger RESTART m_nTimeoutStartTicks=" << m_nTimeoutStartTicks << " m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << " nInitial=" << nInitial << '\n';
		if (oTimeout.m_eAlarmTimeoutType == ALARMS_EVENT_SET_TICKS) {
			m_nTimeoutTicks = nInitial;
			m_nTimeoutMillisec = -1;
			oLevel.activateEvent(this, nGameTick + m_nTimeoutTicks);
		} else {
			assert(oTimeout.m_eAlarmTimeoutType == ALARMS_EVENT_SET_MILLISEC);
			m_nTimeoutMillisec = nInitial;
			m_nTimeoutTicks = -1;
			oLevel.activateEvent(this, nGameTick + 1);
		}
		return; //--------------------------------------------------------------
	}
	if (nMsg == MESSAGE_ALARMS_NEXT) {
//std::cout << "AlarmsEvent::trigger NEXT m_nTimeoutStartTicks=" << m_nTimeoutStartTicks << '\n';
		// activate next alarm
		if (m_nTimeoutStartTicks >= 0) {
			alreadyActive(nGameTick);
			return; //----------------------------------------------------------
		}
		if (m_bStepping) {
			m_nTimeoutStartTicks = nGameTick;
			m_nTimeoutStartMillisec = nGameMillisec;
			m_nLastHandledTick = nGameTick;
//std::cout << "AlarmsEvent::trigger NEXT m_nTimeoutStartTicks=" << m_nTimeoutStartTicks << " m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << '\n';
			reactivate(nGameTick);
		} else if (m_nCurrentChange < 0) {
			oLevel.activateEvent(this, nGameTick);
		}
		return; //--------------------------------------------------------------
	}
	if (nMsg == MESSAGE_ALARMS_REDO_CURRENT) {
//std::cout << "AlarmsEvent::trigger  MESSAGE_ALARMS_REDO_CURRENT  m_nTimeoutStartTicks=" << m_nTimeoutStartTicks << "  nGameTick=" << nGameTick << '\n';
		if (! m_bStepping) {
			if (m_nCurrentChange < 0) {
				oLevel.activateEvent(this, nGameTick);
			}
			return; //----------------------------------------------------------
		}
		m_nTimeoutStartTicks = nGameTick;
		m_nTimeoutStartMillisec = nGameMillisec;
		m_nLastHandledTick = nGameTick;
		reactivate(nGameTick);
		return; //--------------------------------------------------------------
	}
	if (!bTimeout) {
		return; //--------------------------------------------------------------
	}
//std::cout << "AlarmsEvent::trigger  m_nCurrentChange=" << m_nCurrentChange << "  nGameTick=" << nGameTick << '\n';
	const int32_t nElapsedTicks = nGameTick - m_nTimeoutStartTicks;
	const int32_t nElapsedMillisec = nGameMillisec - m_nTimeoutStartMillisec;
	m_nTimeoutStartTicks = -1;
	m_nTimeoutStartMillisec = -1;
	if (m_nCurrentChange >= static_cast<int32_t>(m_oData.m_aAlarmTimeouts.size())) {
		m_bStepping = false;
	} else {
		AlarmTimeout& oChange = m_oData.m_aAlarmTimeouts[m_nCurrentChange];

		int32_t nNewTicks = m_nTimeoutTicks;
		int32_t nNewMillisec = m_nTimeoutMillisec;
		switch (oChange.m_eAlarmTimeoutType) {
			case ALARMS_EVENT_SET_TICKS:
			{
				nNewTicks = oChange.m_nChange;
				nNewMillisec = -1;
			}
			break;
			case ALARMS_EVENT_INC_TICKS:
			{
				if (nNewTicks < 0) {
					nNewTicks = nElapsedTicks;
				}
				nNewTicks += oChange.m_nChange;
				nNewMillisec = -1;
			}
			break;
			case ALARMS_EVENT_SET_MILLISEC:
			{
				nNewMillisec = oChange.m_nChange;
				nNewTicks = -1;
			}
			break;
			case ALARMS_EVENT_INC_MILLISEC:
			{
				if (nNewMillisec < 0) {
					nNewMillisec = nElapsedMillisec;
				}
				nNewMillisec += oChange.m_nChange;
				nNewTicks = -1;
			}
			break;
			case ALARMS_EVENT_MULT_PERC:
			{
				if (nNewTicks >= 0) {
					nNewTicks = nNewTicks * oChange.m_nChange / 100;
				} else {
					assert(nNewMillisec >= 0);
					nNewMillisec = nNewMillisec * oChange.m_nChange / 100;
				}
			}
			break;
			default:
			{
				assert(false);
			}
		}
		if (nNewTicks >= 0) {
			nNewTicks = std::max<int32_t>(nNewTicks, 1);
			m_nTimeoutTicks = nNewTicks;
			m_nTimeoutMillisec = -1;
		} else {
			nNewMillisec = std::max<int32_t>(nNewMillisec, 1);
			m_nTimeoutMillisec = nNewMillisec;
			m_nTimeoutTicks = -1;
		}
		incChangeCounter(oChange.m_nRepeat);
	}

//std::cout << "AlarmsEvent::trigger (TIMEOUT) m_nTimeoutTicks=" << m_nTimeoutTicks << " m_nTimeoutMillisec=" << m_nTimeoutMillisec << '\n';
	const int32_t nTimeoutCounter = m_nTimeoutCounter;
	++m_nTimeoutCounter;
//std::cout << "AlarmsEvent::trigger  LISTENER_GROUP_TIMEOUT    nGameTick=" << nGameTick << '\n';
	informListeners(LISTENER_GROUP_TIMEOUT, nTimeoutCounter);

	if (! m_bStepping) {
		m_nCurrentChange = 0; // marks as finished
		informListeners(LISTENER_GROUP_FINISHED, 0);
	}
}

} // namespace stmg
