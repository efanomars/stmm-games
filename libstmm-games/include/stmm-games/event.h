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
 * File:   event.h
 */

#ifndef STMG_EVENT_H
#define STMG_EVENT_H

#include <vector>

#include <stdint.h>

namespace stmg
{

class Level;

/** The level event class.
 */
class Event
{
public:
	struct Init
	{
		Level* m_p0Level; /**< The level the event will be added to. Cannot be null. */
		int32_t m_nPriority = 0; /**< The priority. The higher the number the  higher the priority. Default is 0. */
	};
	/** Constructor.
	 * The event can only be added to the level passed as parameter to this function.
	 * The level passed as oInit.m_p0Level must also be valid (already be constructed or reinitialized).
	 * @param oInit The initialization data.
	 */
	explicit Event(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * The event can only be added to the level passed as parameter to this function.
	 * The level passed as oInit.m_p0Level must also be valid (already be constructed or reinitialized).
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
public:
	virtual ~Event() noexcept = default;

	/** Value filter.
	 * @see Event::addMsgFilter().
	 */
	enum MSG_FILTER_VALUE_OP {
		MSG_FILTER_VALUE_OP_FIRST = 0
		, MSG_FILTER_VALUE_OP_UNCHANGED = 0 /**< The value is not changed. */
		, MSG_FILTER_VALUE_OP_SET = 1 /**< The value is set. */
		, MSG_FILTER_VALUE_OP_MULT_ADD = 2 /**< The value is multiplied. */
		, MSG_FILTER_VALUE_OP_DIV_ADD = 3 /**< The value is divided */
		, MSG_FILTER_VALUE_OP_PERCENT_ADD = 4 /**< The value is "percentaged". */
		, MSG_FILTER_VALUE_OP_PERMILL_ADD = 5 /**< The value is "permilled". */
		, MSG_FILTER_VALUE_OP_MOD_ADD = 6 /**< The value is "moduloed". */
		, MSG_FILTER_VALUE_OP_MIN_ADD = 7 /**< The value is minimized. */
		, MSG_FILTER_VALUE_OP_MAX_ADD = 8 /**< The value is maximized. */
		//, MSG_FILTER_VALUE_OP_MULT_INTERVAL_ADD = 8 /**< The value is multiplied by current game interval and operand. */
		//, MSG_FILTER_VALUE_OP_DIV_INTERVAL_ADD = 9 /**< The value is divided by current game interval and operand. */
		//, MSG_FILTER_VALUE_OP_FLOORMOD_ADD = 10 /**< The value is "moduloed". */
		, MSG_FILTER_VALUE_OP_LAST = 8
	};
	/** Add a message filter.
	 * All messages should be numbers &gt;= -1.
	 *
	 * The new value set by the filter is the result of the expression
	 * `nValue "MSG_FILTER_VALUE_OP" nOutValueOperand [+ nOutValueAdd]`
	 *
	 * Example: if eOutValueOperator is MSG_FILTER_VALUE_OP_MULT_ADD the new value
	 * is `nValue * nOutValueOperand + nOutValueAdd`.
	 * @param nInFromMsg The filter is applied when the incoming message is &gt;= this nInFromMsg.
	 * @param nInToMsg The filter is applied when the incoming message is &lt;= this nInToMsg.
	 * @param nInFromValue The filter is applied when the incoming value is &gt;= this nInFromValue.
	 * @param nInToValue The filter is applied when the incoming value is &lt;= this nInToValue.
	 * @param bOutMsgDefined Whether the message is overwritten.
	 * @param nOutMsg The new message.
	 * @param eOutValueOperator The operator applied to the old value and nOutValueOperand.
	 * @param nOutValueOperand The operand of eOutValueOperator.
	 * @param nOutValueAdd The value added to the new value expression.
	 */
	virtual void addMsgFilter(int32_t nInFromMsg, int32_t nInToMsg, int32_t nInFromValue, int32_t nInToValue
							, bool bOutMsgDefined, int32_t nOutMsg
							, MSG_FILTER_VALUE_OP eOutValueOperator, int32_t nOutValueOperand, int32_t nOutValueAdd) noexcept;
	/** Adds a listener to the event.
	 * @param nGroupId The listening group it is added to. If negative the listener is added to all groups.
	 * @param p0ListenerEvent The listener. Cannot be null.
	 * @param nMsg The message to send to the listener when triggered. If negative the group as passed to informListeners() is used.
	 */
	virtual void addListener(int32_t nGroupId, Event* p0ListenerEvent, int32_t nMsg) noexcept;

	/** Tells whether the event is active.
	 * To activate an event call Level::activateEvent(p0Event, nTriggerTime).
	 * To deactivate an event call Level::deactivateEvent(p0Event).
	 * The event is also deactivate by the Level::triggerEvent() function.
	 * @return whether the event is active.
	 */
	bool isActive() const noexcept { return m_bIsActive; }
	/** The triggering time.
	 * The event must be active to be triggered by the timer.
	 *
	 * The returned value might not be -1 even if the event is not active.
	 * See Level::triggerEvent().
	 * @return The time in game ticks the event should be triggered or -1.
	 */
	int32_t getTriggerTime() const noexcept { return m_nTriggerTime; }

	/** The priority.
	 * @return The priority.
	 */
	int32_t getPriority() const noexcept { return m_nPriority; }

	void setDebugTag(int32_t nDebugTag) noexcept;
	int32_t getDebugTag() const noexcept;
public:
	enum {
		LISTENER_GROUP_FINISHED = 0 /**< Reserved group that is triggered when an event has "expired". */
	};

protected:
	/** Calls the trigger function of a group of listeners.
	 * Before the trigger function of the listeners is called the listener's filters are applied.
	 * @param nGroupId The group. Must be &gt;= 0.
	 * @param nValue The value to send.
	 */
	virtual void informListeners(int32_t nGroupId, int32_t nValue) noexcept;
	/** The trigger function.
	 * This function provides the functionality of the event and can be called in two ways.
	 *
	 * It can be called by an event through informListeners() this event was listening to.
	 * Note: events can listen to themselves.
	 *
	 * It can also be called by the timer if the event was active.
	 *
	 * When this function is called the event has already been deactivated by the level
	 * (if it was active in the first place).
	 *
	 * When this function is called the filters of this event have already been
	 * applied.
	 * @param nMsg The message received.
	 * @param nValue The value received.
	 * @param p0TriggeringEvent The triggering event or null if triggered by timer.
	 */
	virtual void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept = 0;
	/** The level this event belongs to.
	 * @return The level.
	 */
	inline const Level& level() const noexcept { return *m_p0Level; }
	/** The level this event belongs to.
	 * @return The level.
	 */
	inline Level& level() noexcept { return *m_p0Level; }

private:
	friend class Level;

	struct EventListener
	{
		int32_t m_nMsg;
		Event* m_p0TargetEvent;
	};
	struct MsgFilterIn
	{
		int32_t m_nInFromMsg;
		int32_t m_nInToMsg;
		int32_t m_nInFromValue;
		int32_t m_nInToValue;
	};
	struct MsgFilterOut
	{
		bool m_bOutMsgDefined;
		int32_t m_nOutMsg;
		MSG_FILTER_VALUE_OP m_eOutValueOperator;
		int32_t m_nOutValueOperand;
		int32_t m_nOutValueAdd;
	};

	inline bool hasMsgFilters() noexcept
	{
		return !m_aMsgFilterIn.empty();
	}
	bool filterMsg(int32_t& nMsg, int32_t& nValue) noexcept;
	inline void setTriggerTime(int32_t nTime) noexcept
	{
		m_nTriggerTime = nTime;
	}
	inline void setIsActive(bool bIsActive) noexcept
	{
		m_bIsActive = bIsActive;
	}

private:
	int32_t m_nPriority;
	int32_t m_nTriggerTime;
	int32_t m_nDebugTag;
	std::vector<int32_t> m_aListenerGroupIds;
	std::vector<EventListener> m_aListeners;
	std::vector<MsgFilterIn> m_aMsgFilterIn;
	std::vector<MsgFilterOut> m_aMsgFilterOut;
	Level* m_p0Level;
	bool m_bIsActive;

private:
	Event(const Event& oSource) = delete;
	Event& operator=(const Event& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_EVENT_H */

