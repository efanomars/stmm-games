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
 * File:   mockevent.h
 */
/*   @DO_NOT_REMOVE_THIS_LINE_IT_IS_USED_BY_COMMONTESTING_CMAKE@   */

#ifndef STMG_MOCK_EVENT_H
#define STMG_MOCK_EVENT_H

#include <stmm-games/event.h>

#include <vector>
#include <functional>

#include <stdint.h>

namespace stmg
{

class Level;

class MockEvent : public Event
{
public:
	struct Init : public Event::Init
	{
		std::function<void(Level& oLevel)> m_oTriggerFunction; /**< The function called */
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit MockEvent(Init&& oInit) noexcept;
	/** Constructor with custom function.
	 * The function is called when the event is triggered. See Level::activateEvent().
	 * @param oInit The initialization data.
	 * @param oTriggerFunction The trigger function.
	 */
	MockEvent(Init&& oInit, std::function<void(Level& oLevel)>&& oTriggerFunction) noexcept;

protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
	/** Reinitialization with custom function.
	 * @param oInit The initialization data.
	 * @param oTriggerFunction The trigger function.
	 */
	void reInit(Init&& oInit, std::function<void(Level& oLevel)>&& oTriggerFunction) noexcept;
public:
	/** Sets a variable when triggered (before inform listeners is called).
	 * This is useful because variables can only be set within a game tick.
	 *
	 * If a variable is set more than once only the last matters.
	 * @param nVarId The  variable id. Must be valid.
	 * @param nTeam The team. If -1 it is a game variable.
	 * @param nMate The mate. If -1 it is a game or team variable.
	 * @param nValue The new value.
	 */
	void setVariable(int32_t nVarId, int32_t nTeam, int32_t nMate, int32_t nValue) noexcept;
	/** Set the value the trigger function should send.
	 * @param nGroup The group that should be triggered.
	 * @param nValue The value passed.
	 * @param nSkipTicks How many ticks to the triggering.
	 */
	void setTriggerValue(int32_t nGroup, int32_t nValue, int32_t nSkipTicks) noexcept;
	/** The common triggering function.
	 * Triggers the group and value set in setTriggerValue().
	 * Note that if this event is triggered by another event it won't do anything.
	 */
	void trigger(int32_t /*nMsg*/, int32_t /*nValue*/, Event* p0TriggeringEvent) noexcept override;

private:
	int32_t m_bTriggerValuesSet;
	int32_t m_nGroup;
	int32_t m_nValue;

	struct VarData
	{
		int32_t m_nVarId = -1;
		int32_t m_nTeam = -1;
		int32_t m_nMate = -1;
		int32_t m_nValue = -1;
	};
	std::vector<VarData> m_aVarDatas;

	std::function<void(Level& oLevel)> m_oTriggerFunction;
private:
	MockEvent() = delete;
	MockEvent(const MockEvent& oSource) = delete;
	MockEvent& operator=(const MockEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_MOCK_EVENT_H */

