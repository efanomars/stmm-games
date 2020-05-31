/*
 * File:   selectevent.h
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

#ifndef STMG_SELECT_EVENT_H
#define STMG_SELECT_EVENT_H

#include "event.h"

#include <stdint.h>

namespace stmg
{

/** Select listener group by nValue or a variable's value.
 * The value sent to the listeners is the nMsg (see trigger()) if selecting by nValue,
 * the nValue if selecting by a variable's value.
 */
class SelectEvent : public Event
{
public:
	struct LocalInit
	{
		int32_t m_nVarIndex = -1; /**< The selecting variable id. Must be valid or -1 if selection is done by nValue. Default is -1. */
		int32_t m_nVarTeam = -1; /**< The preferences team (not the level team) the variable belongs to or -1 if game variable. Default is -1. */
		int32_t m_nVarMate = -1; /**< The mate nr of the player the variable belongs to or -1 if a team or game variable. Default is -1. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit SelectEvent(Init&& oInit) noexcept;
protected:
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

private:
	void initCommon(int32_t nVarTeam) noexcept;
private:
	int32_t m_nVarLevel;
	int32_t m_nVarLevelTeam;
	int32_t m_nVarMate;
	int32_t m_nVarIndex;
private:
	SelectEvent() = delete;
	SelectEvent(const SelectEvent& oSource) = delete;
	SelectEvent& operator=(const SelectEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_SELECT_EVENT_H */

