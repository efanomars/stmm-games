/*
 * File:   arrayevent.h
 *
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

#ifndef STMG_ARRAY_EVENT_H
#define STMG_ARRAY_EVENT_H

#include "event.h"

#include <vector>

#include <stdint.h>

namespace stmg { class Variable; }

namespace stmg
{

/** Array event.
 * Allows to index values in a possibly multi dimensional array.
 * The index can only be a variable or nValue passed to trigger() function.
 * The addressed value is sent to listeners.
 *
 * If any index is out of bounds the default value is returned.
 *
 * Five channels are available.
 *
 * Example: if an instance is triggered with MESSAGE_ARRAY_GET_3, the result
 * will be sent to LISTENER_GROUP_ARRAY_RESULT_3 listeners.
 */
class ArrayEvent : public Event
{
public:
	struct Dimension
	{
		int32_t m_nSize = 0; /**< The size of the array for this index. */
		int32_t m_nVarIndex = -1; /**< Either the variable id or -1 if nValue is used as index.  */
		int32_t m_nVarTeam = -1; /**< The preferences team of the variable or -1 if a game variable (or nValue is used). */
		int32_t m_nVarMate = -1; /**< The mate of the variable or -1 if a non player variable (or nValue is used). */
	private:
		friend class ArrayEvent;
		Variable* m_p0Variable = nullptr;
	};

	struct LocalInit
	{
		std::vector<Dimension> m_aDimensions; /**< The dimensions of m_oArray. Only one of the dimensions can have m_nVarIndex -1. */
		std::vector<int32_t> m_aValues; /**< The array of values. Its size  must be the multiplication of the Dimensions sizes. */
		int32_t m_nDefaultValue = 0; /**< The default value if any index is outside the allowed range. Default is 0. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * @param oInit Initialization data.
	 */
	explicit ArrayEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// Inputs
	enum {
		MESSAGE_ARRAY_GET = 100 /**< Get value. Channel 0. */
		, MESSAGE_ARRAY_GET_1 = 101 /**< Get value. Channel 1. */
		, MESSAGE_ARRAY_GET_2 = 102 /**< Get value. Channel 2. */
		, MESSAGE_ARRAY_GET_3 = 103 /**< Get value. Channel 3. */
		, MESSAGE_ARRAY_GET_4 = 104 /**< Get value. Channel 4. */
		, MESSAGE_ARRAY_GET_5 = 105 /**< Get value. Channel 5. */
	};
	// Outputs
	enum {
		LISTENER_GROUP_ARRAY_RESULT = 10 /**< The value. Channel 0. */
		, LISTENER_GROUP_ARRAY_RESULT_1 = 11 /**< The value. Channel 1. */
		, LISTENER_GROUP_ARRAY_RESULT_2 = 12 /**< The value. Channel 2. */
		, LISTENER_GROUP_ARRAY_RESULT_3 = 13 /**< The value. Channel 3. */
		, LISTENER_GROUP_ARRAY_RESULT_4 = 14 /**< The value. Channel 4. */
		, LISTENER_GROUP_ARRAY_RESULT_5 = 15 /**< The value. Channel 5. */
	};

private:
	void commonInit() noexcept;
	int32_t checkArray() noexcept;

private:
	LocalInit m_oData;
private:
	ArrayEvent() = delete;
	ArrayEvent(const ArrayEvent& oSource) = delete;
	ArrayEvent& operator=(const ArrayEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_ARRAY_EVENT_H */

