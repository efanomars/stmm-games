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
 * File:   speedevent.h
 */

#ifndef STMG_SPEED_EVENT_H
#define STMG_SPEED_EVENT_H

#include "event.h"

#include <vector>

#include <stdint.h>

namespace stmg
{

class SpeedEvent : public Event
{
public:
	enum SPEED_EVENT_CHANGE_TYPE {
		SPEED_EVENT_CHANGE_TYPE_NONE = 0 /**< No change. */
		, SPEED_EVENT_CHANGE_TYPE_SET = 1 /**< Set value. */
		, SPEED_EVENT_CHANGE_TYPE_INC = 2 /**< Increment current value. */
		, SPEED_EVENT_CHANGE_TYPE_MULT_PERC = 3 /**< Multiply current value by a percentage. Example: -30 means `-30%` or `-0.3`.*/
	};
	struct SpeedChange
	{
		int32_t m_nRepeat = -1; /**< The number of times the change should be repeated or -1 if forever.
								 * Default is -1 (should be the last of the vector). *///
		SPEED_EVENT_CHANGE_TYPE m_eIntervalChangeType = SPEED_EVENT_CHANGE_TYPE_NONE; /**< The game tick interval change type.
																						 * Default is SPEED_EVENT_CHANGE_TYPE_NONE. */
		int32_t m_nIntervalChange = 0; /**< The value used to compute the next game interval according to m_eIntervalChangeType.
										 * Default is 0. */
		SPEED_EVENT_CHANGE_TYPE m_eFallTicksChangeType = SPEED_EVENT_CHANGE_TYPE_NONE; /**< The fall ticks change type. Default is SPEED_EVENT_CHANGE_TYPE_NONE. */
		int32_t m_nFallTicksChange = 0; /**< The value used to compute the next number of game ticks level blocks should fall according to m_eFallTicksChangeType.
										 * Default is 0. */
	};
	struct LocalInit
	{
		std::vector<SpeedChange> m_aSpeedChanges; /**< The speed changes. Cannot be empty. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};

	/** Constructor.
	 * Note that the game tick interval change is a request for this event's level.
	 * The game will decide what the real game tick interval is taking into account
	 * the requests of all levels.
	 *
	 * A change is forced by triggering the event.
	 * @param oInit The parameters.
	 */
	explicit SpeedEvent(Init&& oInit) noexcept;
protected:
	void reInit(Init&& oInit) noexcept;
public:
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	enum {
		LISTENER_GROUP_CHANGED = 10 /**< Sent when either interval or fall ticks have changed. nValue contains the change count. */
		, LISTENER_GROUP_NOT_CHANGED = 11 /**< Sent when interval or fall ticks have not changed. nValue contains the change count. */
		, LISTENER_GROUP_CHANGED_INTERVAL = 15 /**< Sent when interval has changed. nValue contains new minus old interval. */
		, LISTENER_GROUP_CHANGED_FALL_EACH_TICKS = 16 /**< Sent when fall ticks has changed. nValue contains new minus old fall ticks. */
	};

private:
	LocalInit m_oData;
	int32_t m_nCurrentChange;
	int32_t m_nInChangeCounter;
	int32_t m_nSpeedCount;

private:
	SpeedEvent() = delete;
	SpeedEvent(const SpeedEvent& oSource) = delete;
	SpeedEvent& operator=(const SpeedEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SPEED_EVENT_H */

