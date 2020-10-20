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
 * File:   variableevent.h
 */

#ifndef STMG_VARIABLE_EVENT_H
#define STMG_VARIABLE_EVENT_H

#include "event.h"

#include <stdint.h>

namespace stmg
{

/** Variable event.
 * Instances of this class are associated with a specific variable and
 * by triggering them they can change its value.
 *
 * If the variable is time relative the event activates itself for
 * the next game tick automatically and LISTENER_GROUP_VAR_CHANGED is sent
 * if necessary. If it isn't time relative you need to trigger the event for it
 * to send a message to LISTENER_GROUP_VAR_CHANGED listeners. One way to do it is by
 * sending MESSAGE_ADD_VALUE with value 0.
 */
class VariableEvent : public Event
{
public:
	struct LocalInit
	{
		int32_t m_nVarTeam = -1; /**< The preferences team (not the level team) the variable belongs to or -1 if game variable. Default is -1. */
		int32_t m_nVarMate = -1; /**< The mate nr of the player the variable belongs to or -1 if a team or game variable. Default is -1. */
		int32_t m_nVarIndex = 0; /**< The variable id. Must be valid. */
		int32_t m_nDefaultIncBy = 0; /**< The value that is added to the variable when the nMsg is not in MESSAGE_ADD_TO_VALUE. Default is 0. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit VariableEvent(Init&& oInit) noexcept;
protected:
	/** Reinitialization.
	 * @param oInit The initialization data.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	/** The triggered function.
	 * @param nMsg If one MESSAGE_OP_TYPE_ENUM then operation is performed otherwise nDefaultIncBy (see constructor).
	 * @param nValue The triggered add value. Is only used if nMsg is MESSAGE_OP_TYPE_ENUM.
	 * @param p0TriggeringEvent The triggering event or null if event activated itself.
	 */
	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum MESSAGE_OP_TYPE_ENUM {
		MESSAGE_SET_VALUE = 100 /**< Variable is set to nValue. */
		, MESSAGE_RESET = 101 /**< Variable is set to 0. */
		, MESSAGE_ADD_VALUE = 110 /**< trigger()'s nValue is added to the variable. */
		, MESSAGE_ADD_PERC_VALUE = 111 /**< trigger()'s nValue% (percent) is added to the variable. */
		, MESSAGE_MUL_BY_VALUE = 112 /**< The variable is multiplied by trigger()'s nValue. */
		, MESSAGE_NEG_ADD_VALUE = 115 /**< trigger()'s nValue is added to the negated variable. */
		, MESSAGE_INC = 116 /**< Variable is incremented. */
		, MESSAGE_SUB_VALUE = 120 /**< trigger()'s nValue is subtracted from the variable. */
		, MESSAGE_SUB_PERC_VALUE = 121 /**< trigger()'s nValue% (percent) is subtracted from the variable. */
		, MESSAGE_DIV_BY_VALUE = 122 /**< The variable is divided by trigger()'s nValue. Division by 0 is 0. */
		, MESSAGE_NEG_SUB_VALUE = 125 /**< trigger()'s nValue is subtracted to the negated variable. */
		, MESSAGE_DEC = 126 /**< Variable is decremented. */
		, MESSAGE_MIN_VALUE = 150 /**< Variable is set to min(nValue, current value). */
		, MESSAGE_MAX_VALUE = 151 /**< Variable is set to max(nValue, current value). */
		, MESSAGE_VAR_GET_VALUE = 200 /**< The variable is unchanged and just output by LISTENER_GROUP_VAR_VALUE. */
	};
	// output
	enum {
		LISTENER_GROUP_VAR_CHANGED = 10 /**< Listener group triggered (with the new value) when the variable was changed. */
		, LISTENER_GROUP_VAR_VALUE = 20 /**< Listener group triggered (with the value) when MESSAGE_VAR_GET_VALUE is received. */
	};

private:
	void initCommon(int32_t nVarTeam) noexcept;

private:
	int32_t m_nVarLevel;
	int32_t m_nVarLevelTeam;
	int32_t m_nVarMate;
	int32_t m_nVarIndex;
	int32_t m_nDefaultIncBy;
	bool m_bTimeRelative;

	bool m_bOldValueInitialized;
	int32_t m_nOldValue;

private:
	VariableEvent() = delete;
	VariableEvent(const VariableEvent& oSource) = delete;
	VariableEvent& operator=(const VariableEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_VARIABLE_EVENT_H */

