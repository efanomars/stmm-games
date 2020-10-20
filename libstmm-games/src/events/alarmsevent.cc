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
 * File:   alarmsevent.cc
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
	m_nCurrentStage = -1; // Marks as never started
	m_nInStageCounter = -1;
	m_nTimeoutCounter = -1;
}
void AlarmsEvent::checkParams() noexcept
{
	#ifndef NDEBUG
	bool bFirst = false;
	for (const auto& oAlarmsStage : m_oData.m_aAlarmsStages) {
		if (bFirst) {
			bFirst = true;
			switch (oAlarmsStage.m_eAlarmsStageType) {
				case ALARMS_STAGE_SET_TICKS:
					assert(oAlarmsStage.m_nChange > 0);
				break;
				case ALARMS_STAGE_SET_MILLISEC:
					assert(oAlarmsStage.m_nChange > 0);
				break;
				default:
					assert(false);
				break;
			}
		} else {
			switch (oAlarmsStage.m_eAlarmsStageType) {
				case ALARMS_STAGE_SET_TICKS:
					assert(oAlarmsStage.m_nChange > 0);
				break;
				case ALARMS_STAGE_INC_TICKS:
				break;
				case ALARMS_STAGE_SET_MILLISEC:
					assert(oAlarmsStage.m_nChange > 0);
				break;
				case ALARMS_STAGE_INC_MILLISEC:
				break;
				case ALARMS_STAGE_MULT_PERC:
					assert(oAlarmsStage.m_nChange > 0);
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
	++m_nInStageCounter;
	if ((nCurTimeoutRepeat > 0) && (m_nInStageCounter >= nCurTimeoutRepeat)) {
		m_nInStageCounter = 0;
		++m_nCurrentStage;
		assert(m_nCurrentStage <= static_cast<int32_t>(m_oData.m_aAlarmsStages.size()));
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
//std::cout << "AlarmsEvent::trigger TIMEOUT m_nCurrentStage=" << m_nCurrentStage << " m_nInStageCounter=" << m_nInStageCounter << '\n';
			m_nLastHandledTick = nGameTick;
			if (m_nTimeoutMillisec >= 0) {
//std::cout << "AlarmsEvent::trigger m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << " m_nTimeoutMillisec=" << m_nTimeoutMillisec << '\n';
				if (nGameMillisec >= m_nTimeoutStartMillisec + m_nTimeoutMillisec) {
					bTimeout = true;
				} else {
//std::cout << "AlarmsEvent::trigger DELAYED m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << '\n';
					oLevel.activateEvent(this, nGameTick + 1);
					return; //--------------------------------------------------
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
	}
	switch (nMsg) {
	case MESSAGE_ALARMS_FINISH: {
//std::cout << "AlarmsEvent::trigger() FINISH" << '\n';
		if (m_bStepping) {
			m_bStepping = false;
			m_nTimeoutStartTicks = -1;
			m_nTimeoutStartMillisec = -1;
			m_nCurrentStage = 0; // marks as finished (not set to -1 to avoid redoing initial start)
			informListeners(LISTENER_GROUP_FINISHED, 0);
		}
		return; //--------------------------------------------------------------
	} break;
	case MESSAGE_ALARMS_STAGE_NEXT: {
		if (! m_bStepping) {
			return; //----------------------------------------------------------
		}
		if (m_nCurrentStage >= static_cast<int32_t>(m_oData.m_aAlarmsStages.size() - 1)) {
			return; //----------------------------------------------------------
		}
		++m_nCurrentStage;
		m_nInStageCounter = 0;
		reactivate(nGameTick);
		return; //--------------------------------------------------------------
	} break;
	case MESSAGE_ALARMS_STAGE_RESET: {
		if (! m_bStepping) {
			return; //----------------------------------------------------------
		}
		if (m_nCurrentStage >= static_cast<int32_t>(m_oData.m_aAlarmsStages.size())) {
			return; //----------------------------------------------------------
		}
		m_nInStageCounter = 0;
		reactivate(nGameTick);
		return; //--------------------------------------------------------------
	} break;
	case MESSAGE_ALARMS_REDO_CURRENT: {
		if (! m_bStepping) {
			if (m_nCurrentStage < 0) {
				oLevel.activateEvent(this, nGameTick);
			}
			return; //----------------------------------------------------------
		}
		m_nTimeoutStartTicks = nGameTick;
		m_nTimeoutStartMillisec = nGameMillisec;
		m_nLastHandledTick = nGameTick;
		reactivate(nGameTick);
		return; //--------------------------------------------------------------
	} break;
	case MESSAGE_ALARMS_NEXT: {
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
		} else if (m_nCurrentStage < 0) {
			oLevel.activateEvent(this, nGameTick);
		}
		return; //--------------------------------------------------------------
	} break;
	case MESSAGE_ALARMS_RESTART: {
		if (m_nTimeoutStartTicks >= 0) {
			alreadyActive(nGameTick);
			return; //----------------------------------------------------------
		}
		m_bStepping = true;
		m_nCurrentStage = 0;
		m_nInStageCounter = 0;
		m_nTimeoutCounter = 0;
		m_nTimeoutStartTicks = nGameTick;
		m_nTimeoutStartMillisec = nGameMillisec;
		m_nLastHandledTick = nGameTick;
		AlarmsStage& oTimeout = m_oData.m_aAlarmsStages[m_nCurrentStage];
		const int32_t nInitial = m_oData.m_aAlarmsStages[m_nCurrentStage].m_nChange;
		incChangeCounter(oTimeout.m_nRepeat); // point to the next timeout
//std::cout << "AlarmsEvent::trigger RESTART m_nTimeoutStartTicks=" << m_nTimeoutStartTicks << " m_nTimeoutStartMillisec=" << m_nTimeoutStartMillisec << " nInitial=" << nInitial << '\n';
		if (oTimeout.m_eAlarmsStageType == ALARMS_STAGE_SET_TICKS) {
			m_nTimeoutTicks = nInitial;
			m_nTimeoutMillisec = -1;
			oLevel.activateEvent(this, nGameTick + m_nTimeoutTicks);
		} else {
			assert(oTimeout.m_eAlarmsStageType == ALARMS_STAGE_SET_MILLISEC);
			m_nTimeoutMillisec = nInitial;
			m_nTimeoutTicks = -1;
			oLevel.activateEvent(this, nGameTick + 1);
		}
		return; //--------------------------------------------------------------
	} break;
	default: {
	} break;
	}
	if (!bTimeout) {
		// make it resilient to unknown messages
		reactivate(nGameTick);
		return; //--------------------------------------------------------------
	}
//std::cout << "AlarmsEvent::trigger  m_nCurrentStage=" << m_nCurrentStage << "  nGameTick=" << nGameTick << '\n';
	const int32_t nElapsedTicks = nGameTick - m_nTimeoutStartTicks;
	const int32_t nElapsedMillisec = nGameMillisec - m_nTimeoutStartMillisec;
	m_nTimeoutStartTicks = -1;
	m_nTimeoutStartMillisec = -1;
	if (m_nCurrentStage >= static_cast<int32_t>(m_oData.m_aAlarmsStages.size())) {
		m_bStepping = false;
	} else {
		AlarmsStage& oChange = m_oData.m_aAlarmsStages[m_nCurrentStage];

		int32_t nNewTicks = m_nTimeoutTicks;
		int32_t nNewMillisec = m_nTimeoutMillisec;
		switch (oChange.m_eAlarmsStageType) {
			case ALARMS_STAGE_SET_TICKS:
			{
				nNewTicks = oChange.m_nChange;
				nNewMillisec = -1;
			}
			break;
			case ALARMS_STAGE_INC_TICKS:
			{
				if (nNewTicks < 0) {
					nNewTicks = nElapsedTicks;
				}
				nNewTicks += oChange.m_nChange;
				nNewMillisec = -1;
			}
			break;
			case ALARMS_STAGE_SET_MILLISEC:
			{
				nNewMillisec = oChange.m_nChange;
				nNewTicks = -1;
			}
			break;
			case ALARMS_STAGE_INC_MILLISEC:
			{
				if (nNewMillisec < 0) {
					nNewMillisec = nElapsedMillisec;
				}
				nNewMillisec += oChange.m_nChange;
				nNewTicks = -1;
			}
			break;
			case ALARMS_STAGE_MULT_PERC:
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
		m_nCurrentStage = 0; // marks as finished
		informListeners(LISTENER_GROUP_FINISHED, 0);
	}
}

} // namespace stmg
