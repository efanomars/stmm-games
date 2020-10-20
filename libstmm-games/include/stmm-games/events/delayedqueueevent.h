/*
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
/*
 * File:   delayedqueueevent.h
 */

#ifndef STMG_DELAYED_QUEUE_EVENT_H
#define STMG_DELAYED_QUEUE_EVENT_H

#include "event.h"

#include <deque>

#include <stdint.h>

namespace stmg { class Level; }

namespace stmg
{

/** Delayed queue event.
 * Allows to queue incoming values (messages) and output them one by one
 * separated by an interval (with game tick granularity). If the queue exceeds
 * its max size new values are dropped.
 *
 * The interval is the sum of two components, one is in game ticks, the other is in
 * milliseconds, transformed in game ticks according to the current game interval
 * GameProxy::gameInterval(). The interval is always at least one game tick.
 *
 * If no value was output in the last 'interval' ticks, an incoming value is output
 * in the same game tick it is received.
 *
 * The interval components can be changed at runtime.
 */
class DelayedQueueEvent : public Event
{
public:
	struct LocalInit
	{
		int32_t m_nInitialIntervalTicks = 0; /**< The interval component in game ticks. Default is 0. */
		int32_t m_nInitialIntervalMillisecs = 0; /**< The interval component in milliseconds. Default is 0. */
		int32_t m_nMaxQueueSize = 10; /**< The maximum queue size. Must be positive. Default is 10. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit Initialization data.
	 */
	explicit DelayedQueueEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Inputs
	enum {
		MESSAGE_QUEUE_VALUE = 100 /**< The value to (possibly) queue. */
		, MESSAGE_SET_INTERVAL_TICKS = 101 /**< Sets the game ticks interval component. */
		, MESSAGE_SET_INTERVAL_MILLISECS = 102 /**< Sets the milliseconds interval component. */
		, MESSAGE_SET_INITIAL_INTERVAL = 103 /**< Sets both the interval components to their initial value. */
		, MESSAGE_EMPTY_QUEUE = 104 /**< Empties the queue of values. */
	};
	// Outputs
	enum {
		LISTENER_GROUP_VALUE = 10 /**< The value. */
		, LISTENER_GROUP_OVERFLOW = 20 /**< Sent when an incoming value is dropped because
										* the queue is full. */
	};

private:
	void commonInit() noexcept;
	int32_t getCurrentInterval(const Level& oLevel) const noexcept;

private:
	LocalInit m_oData;
	int32_t m_nIntervalTicks;
	int32_t m_nIntervalMillisecs;
	std::deque<int32_t> m_aQueue;
	int32_t m_nTargetTick; // If positive the earliest the next value can be output
private:
	DelayedQueueEvent() = delete;
	DelayedQueueEvent(const DelayedQueueEvent& oSource) = delete;
	DelayedQueueEvent& operator=(const DelayedQueueEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_DELAYED_QUEUE_EVENT_H */

