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
 * File:   cumulcmpevent.h
 */

#ifndef STMG_CUMUL_CMP_EVENT_H
#define STMG_CUMUL_CMP_EVENT_H

#include "event.h"

#include <stdint.h>

namespace stmg
{

/** Keeps two numbers and compares them if requested.
 * The two numbers are called "left" and "right" and have initial value 0.
 */
class CumulCmpEvent : public Event
{
public:
	enum ON_COMPARE_TYPE
	{
		ON_COMPARE_UNCHANGED = 0 /**< When compared both vars are unchanged. */
		, ON_COMPARE_RESET_BOTH = 1 /**< When compared both vars are reset to 0. */
		, ON_COMPARE_RESET_LEFT = 2 /**< When compared the left var is reset to 0. */
		, ON_COMPARE_RESET_RIGHT = 3 /**< When compared the right var is reset to 0. */
		, ON_COMPARE_SET_TO_INITIAL_BOTH = 5 /**< When compared both vars are set to the initial value (see ctor). */
		, ON_COMPARE_SET_TO_INITIAL_LEFT = 6 /**< When compared the left var is set to its initial value (see ctor). */
		, ON_COMPARE_SET_TO_INITIAL_RIGHT = 7 /**< When compared the right var is set to its initial value (see ctor). */
		, ON_COMPARE_COPY_LEFT_TO_RIGHT = 11 /**< When compared the right var takes the value of the left var. */
		, ON_COMPARE_COPY_RIGHT_TO_LEFT = 12 /**< When compared the left var takes the value of the right var. */
	};
	struct LocalInit
	{
		int32_t m_nInitialLeft = 0; /**< The initial value of the "left" var. Default is 0. */
		int32_t m_nInitialRight = 0; /**< The initial value of the "right" var. Default is 0. */
		ON_COMPARE_TYPE m_eOnCompareType = ON_COMPARE_UNCHANGED; /**< What is done after comparison. Default is ON_COMPARE_UNCHANGED. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit The initialization data.
	 */
	explicit CumulCmpEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum {
		//
		MESSAGE_CUMUL_COMPARE = 100 /**< Compares the "left" and "right" vars. */
		, MESSAGE_CUMUL_SET = 110 /**< Sets the "left" and "right" vars to input nValue. */
		, MESSAGE_CUMUL_SET_LEFT = 111 /**< Sets the "left" var to input nValue. */
		, MESSAGE_CUMUL_SET_RIGHT = 112 /**< Sets the "right" var to input nValue. */
		, MESSAGE_CUMUL_SET_LEFT_COMPARE = 113 /**< Sets the "left" var to input nValue and comparison is forced. */
		, MESSAGE_CUMUL_SET_RIGHT_COMPARE = 114 /**< Sets the "right" var to input nValue and comparison is forced. */
		, MESSAGE_CUMUL_ADD_TO = 120 /**< Input nValue is added to the "left" and "right" vars. */
		, MESSAGE_CUMUL_ADD_TO_LEFT = 121 /**< Input nValue is added to the "left" var. */
		, MESSAGE_CUMUL_ADD_TO_RIGHT = 122 /**< Input nValue is added to the "right" var. */
		, MESSAGE_CUMUL_ADD_TO_LEFT_COMPARE = 123 /**< Input nValue is added to the "left" var and comparison is forced. */
		, MESSAGE_CUMUL_ADD_TO_RIGHT_COMPARE = 124 /**< Input nValue is added to the "right" var and comparison is forced. */
		, MESSAGE_CUMUL_MULT = 130 /**< The "left" and "right" vars are multiplied by input nValue. */
		, MESSAGE_CUMUL_MULT_LEFT = 131 /**< The "left" var is multiplied by input nValue. */
		, MESSAGE_CUMUL_MULT_RIGHT = 132 /**< The "right" var is multiplied by input nValue. */
		, MESSAGE_CUMUL_MULT_LEFT_COMPARE = 133 /**< The "left" var is multiplied by input nValue and comparison is forced. */
		, MESSAGE_CUMUL_MULT_RIGHT_COMPARE = 134 /**< the "right" var is multiplied by input nValue and comparison is forced. */
		, MESSAGE_CUMUL_DIV = 140 /**< The "left" and "right" vars are divided by input nValue. Division by 0 is 0. */
		, MESSAGE_CUMUL_DIV_LEFT = 141 /**< The "left" var is divided by input nValue. Division by 0 is 0. */
		, MESSAGE_CUMUL_DIV_RIGHT = 142 /**< The "right" var is divided by input nValue. Division by 0 is 0. */
		, MESSAGE_CUMUL_DIV_LEFT_COMPARE = 143 /**< The "left" var is divided by input nValue and comparison is forced. Division by 0 is 0. */
		, MESSAGE_CUMUL_DIV_RIGHT_COMPARE = 144 /**< the "right" var is divided by input nValue and comparison is forced. Division by 0 is 0. */
		, MESSAGE_CUMUL_RESET = 210 /**< Sets the "left" and "right" vars to 0. */
		, MESSAGE_CUMUL_RESET_LEFT = 211 /**< Sets the "left" var to 0. */
		, MESSAGE_CUMUL_RESET_RIGHT = 212 /**< Sets the "right" var to 0. */
		, MESSAGE_CUMUL_SET_TO_INITIAL = 220 /**< Sets the "left" and "right" vars to their initial value. */
		, MESSAGE_CUMUL_SET_TO_INITIAL_LEFT = 221 /**< Sets the "left" var to its initial value. */
		, MESSAGE_CUMUL_SET_TO_INITIAL_RIGHT = 222 /**< Sets the "right" var to its initial value. */
	};
	// output
	enum {
		LISTENER_GROUP_COMPARED = 10 /**< Comparison result containing output nValue set to `"left" - "right"`. */
		, LISTENER_GROUP_COMPARED_EQUAL = 20 /**< Only fires if "left" equals "right". Output nValue is "left". */
		, LISTENER_GROUP_COMPARED_NOT_EQUAL = 21 /**< Only fires if "left" not equal "right". Output nValue is `"left" - "right"`. */
	};

private:
	LocalInit m_oData;
	int32_t m_nLeft;
	int32_t m_nRight;

private:
	CumulCmpEvent() = delete;
	CumulCmpEvent(const CumulCmpEvent& oSource) = delete;
	CumulCmpEvent& operator=(const CumulCmpEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_CUMUL_CMP_EVENT_H */
