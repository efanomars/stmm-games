/*
 * File:   alarmsevent.h
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

#ifndef STMG_ALARMS_EVENT_H
#define STMG_ALARMS_EVENT_H

#include "event.h"

#include <vector>

#include <stdint.h>

namespace stmg
{

/** Alarms event.
 * Allows one to set a sequence of alarms.
 * Each alarm timeout of the sequence can be defined in relation to the preceding
 * except for the first.
 *
 * When a timeout expires the next is only started as soon as MESSAGE_ALARMS_NEXT is received.
 * A typical instance will therefore send a MESSAGE_ALARMS_NEXT to itself (triggered by
 * LISTENER_GROUP_TIMEOUT) to start the next alarm.
 *
 * Input MESSAGE_ALARMS_RESTART restarts the whole sequence, but only if no alarm is
 * active.
 */
class AlarmsEvent : public Event
{
public:
	enum ALARMS_EVENT_TYPE
	{
		ALARMS_EVENT_INVALID = -1,
		ALARMS_EVENT_SET_TICKS = 1, /**< Set the next alarm timeout to the m_nChange value
										 * (in game ticks). */
		ALARMS_EVENT_INC_TICKS = 2, /**< Set the next alarm timeout by adding m_nChange to the current
										 * (in game ticks). */
		ALARMS_EVENT_SET_MILLISEC = 3, /**< Set the next alarm timeout to the m_nChange value
										 * (in milliseconds). */
		ALARMS_EVENT_INC_MILLISEC = 4, /**< Set the next alarm timeout by adding m_nChange to the current
										 * (in milliseconds). */
		ALARMS_EVENT_MULT_PERC = 5, /**< Set the next alarm timeout by multiplying the current
										 * timeout by m_nChange and dividing by 100. */
	};
	struct AlarmTimeout
	{
		int32_t m_nRepeat = -1; /**< The number of repetitions or -1 forever (if present should be the last passed to the constructor). Default is -1 (forever). */
		ALARMS_EVENT_TYPE m_eAlarmTimeoutType = ALARMS_EVENT_INVALID;
							/**< The operation applied to the current game ticks interval */
		int32_t m_nChange = 1; /**< The operand used by m_eAlarmTimeoutType. Must be &gt;= 1 for ALARMS_EVENT_SET. */
	};

	struct LocalInit
	{
		std::vector<AlarmTimeout> m_aAlarmTimeouts; /**< The timeouts. The first m_eAlarmTimeoutType must be either
													 * ALARMS_EVENT_SET_TICKS or ALARMS_EVENT_SET_MILLISEC. Cannot be empty. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * Whatever the operation to calculate the new timeout is, the result is guaranteed
	 * to be &gt; 0.
	 * @param oInit Initialization data.
	 */
	explicit AlarmsEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Inputs
	enum {
		MESSAGE_ALARMS_NEXT = 100 /**< Start next alarm. The current must not be active (LISTENER_GROUP_TIMEOUT was already fired). */
		, MESSAGE_ALARMS_FINISH = 110 /**< Finishes the whole alarms sequence. */
		, MESSAGE_ALARMS_RESTART = 120 /**< Restarts the alarms with nValue initial ticks if &gt;=0 or nInitialTicks otherwise (see constructor). */
		, MESSAGE_ALARMS_REDO_CURRENT = 130 /**< Restarts the currently active timeout from now. If no timeout is active same as MESSAGE_ALARMS_NEXT. */
	};
	// Outputs
	enum {
		LISTENER_GROUP_TIMEOUT = 10 /**< The timeout signal. nValue contains a counter incremented at each timeout (starts from 0). */
		, LISTENER_GROUP_TIMEOUT_ACTIVE = 12 /**< Emitted if MESSAGE_ALARMS_NEXT or MESSAGE_ALARMS_RESTART is received while alarms still active. */
	};

private:
	void commonInit() noexcept;
	void checkParams() noexcept;
	void reactivate(int32_t nGameTick) noexcept;
	void alreadyActive(int32_t nGameTick) noexcept;
	void incChangeCounter(int32_t nCurChangeRepeat) noexcept;

private:
	LocalInit m_oData;

	bool m_bStepping;
	int32_t m_nTimeoutStartTicks; // -1 if no timeout active
	int32_t m_nTimeoutStartMillisec; // -1 if no timeout active
	int32_t m_nLastHandledTick; //to reactivate whenout of sync messages
	int32_t m_nTimeoutTicks; // if not negative then m_nTimeoutMillisec is -1
	int32_t m_nTimeoutMillisec; // if not negative then m_nTimeoutTicks is -1
	int32_t m_nCurrentChange;
	int32_t m_nInChangeCounter;
	int32_t m_nTimeoutCounter;
private:
	AlarmsEvent() = delete;
	AlarmsEvent(const AlarmsEvent& oSource) = delete;
	AlarmsEvent& operator=(const AlarmsEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_ALARMS_EVENT_H */

