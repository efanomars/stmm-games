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
 * File:   variable.h
 */

#ifndef STMG_VARIABLE_H
#define STMG_VARIABLE_H

#include <vector>
#include <string>

#include <stdint.h>

namespace stmg { template <class T> class NamedObjIndex; }

namespace stmg
{

class Game;

class Variable
{
public:
	enum VARIABLE_TIME_BASE {
		VARIABLE_TIME_BASE_MILLISEC = 1  /**< 'milliseconds(131234) = 131234  (131234 = 2:11.234 = 2 * 60 + 11 * 1000 + 234)' */
		, VARIABLE_TIME_BASE_SEC = 2     /**< 'seconds(131234)      = 131     (131234 = 2:11.234 -> 2:11 = 2 * 60 + 11)' */
		, VARIABLE_TIME_BASE_MIN = 3     /**< 'minutes(131234)      = 2       (131234 = 2:11.234 -> 2)' */
	};
	enum VARIABLE_FORMAT {
		VARIABLE_FORMAT_NUMBER = 1               /**< '131234 -> 131234' */
		, VARIABLE_FORMAT_MILL = 2               /**< `131234 -> 131'234` */
		, VARIABLE_FORMAT_MIN_SEC = 5            /**< '131    -> 2:11' */
		, VARIABLE_FORMAT_MIN_SEC_MILLISEC = 6   /**< '131234 -> 2:11.234' */
	};

	/** The variable type. Each variable is associated with a type.
	 * Usually more variables share the same type (for example the Points variable
	 * for each team have the same type).
	 */
	struct VariableType
	{
		int32_t m_nInitialValue = 0; /**< The initial value. Default is 0.*/
		bool m_bReadOnly = false; /**< Whether the variable with this type is read-only. Default is false. */
		bool m_bTimeRelative = false; /**< Whether the variable is time relative (time passed from the start of the game). Default is false. */
		bool m_bAddTime = true; /**< Whether the passed time should be added to the variable (or subtracted. Default is true. */
		VARIABLE_TIME_BASE m_eTimeBase = VARIABLE_TIME_BASE_SEC; /**< What is the time base to be added (or subtracted). */
		VARIABLE_FORMAT m_eFormat = VARIABLE_FORMAT_NUMBER; /**< The format the variable should appear when converted to a string. */
	};

	/** The formatted representation of a variable's value.
	 */
	class Value
	{
	public:
		/** The value as an integer. */
		inline int32_t get() const noexcept { return m_nValue; }
		/** The format. */
		inline VARIABLE_FORMAT getFormat() const noexcept { return m_eFormat; }
		/** Return the formatted string. */
		std::string toFormattedString() const noexcept;
		/** Whether the variable is valid.
		 * Checks that the format is valid.
		 */
		bool isValid() const noexcept;
		/** Create a value.
		 * @param nValue The integer value.
		 * @param eFormat The format.
		 * @return The value object.
		 */
		static Value create(int32_t nValue, VARIABLE_FORMAT eFormat) noexcept;
	private:
		friend class Variable;
		int32_t m_nValue = 0;
		VARIABLE_FORMAT m_eFormat = VARIABLE_FORMAT_NUMBER;
	};

	/** Constructor.
	 * Using this instance while the variable type or the game are deleted is
	 * undefined behavior.
	 * @param p0VarType The type. Cannot be null. Must outlive the instance (usage).
	 * @param p0Game The game. Cannot be null. Must outlive the instance (usage).
	 */
	Variable(const VariableType* p0VarType, Game* p0Game) noexcept;

	/** Get the value.
	 * @return The value with format information.
	 */
	Value getValue() const noexcept;

	/** Get the value as a number.
	 * @return The value without format information.
	 */
	int32_t get() const noexcept;
	/** Set the value.
	 * If the variable is read-only, has no effect.
	 * @param nValue The new value.
	 */
	void set(int32_t nValue) noexcept;
	/** Add a value to the current value.
	 * Doesn't check for overflows.
	 * @param nInc The to be added value.
	 */
	void inc(int32_t nInc) noexcept;
	/** Tells whether the variable changed in the current game tick.
	 */
	bool isChanged() const noexcept;
	/** Returns the type of the variable.
	 * @return The type.
	 */
	const VariableType& getType() const noexcept { return *m_p0VarType; }
	/** The value as a string.
	 * @return The string representation of the value.
	 */
	std::string toFormattedString() const noexcept;
private:
	friend class Variables;
	friend class Game;
	friend class Level;

	int32_t calcElapsed() const noexcept;

	/* Value should freeze, even if time relative. */
	void inhibit() noexcept;
	/* Used by Level and Game for system variables. */
	void forceSet(int32_t nValue) noexcept;

	class Variables
	{
	public:
		Variables() noexcept {}
		void init(const NamedObjIndex<VariableType>& oTypes, Game* p0Game) noexcept;
		void inhibit() noexcept;
		Variable& getVariable(int32_t nId) noexcept;
		const Variable& getVariable(int32_t nId) const noexcept;
		int32_t getTotVariables() const noexcept;
	private:
		friend class Variable;
		std::vector<Variable> m_aVar;
	};

private:
	const VariableType* m_p0VarType;
	int32_t m_nValue;
	int32_t m_nLastChangeTime;  // in game elapsed ticks
	int32_t m_nInhibitTimeMillisec; // in game elapsed millisecs
	Game* m_p0Game;
private:
	Variable() = delete;
};

} // namespace stmg

#endif	/* STMG_VARIABLE_H */

