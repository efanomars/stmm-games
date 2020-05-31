/*
 * File:   intset.h
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

#ifndef STMG_INT_SET_H
#define STMG_INT_SET_H

#include <vector>
#include <utility>

#include <stdint.h>

namespace stmg
{

/** Smallish set of integers.
 */
class IntSet
{
public:
	/** Constructs empty integer set.
	 */
	IntSet() noexcept
	: m_nFromValue(0)
	, m_nToValue(0)
	, m_nStep(0)
	{
	}
	/** Constructs integer set with one value.
	 * @param nValue The value.
	 */
	explicit IntSet(int32_t nValue) noexcept
	: m_nFromValue(nValue)
	, m_nToValue(nValue)
	, m_nStep(1)
	{
	}
	/** Constructs integer set with ordered value range.
	 * Value at index 0 is nFromValue, the last is nToValue.
	 * Parameter nFromValue can be &gt; nToValue.
	 * @param nFromValue The start value.
	 * @param nToValue The end value.
	 */
	IntSet(int32_t nFromValue, int32_t nToValue) noexcept;
	/** Constructs integer set with ordered stepped value range.
	 * Value at index 0 is `nFromValue`, value at index 1 is `nFromValue + nStep`
	 * provided it isn't past nToValue, etc.
	 * 
	 * Parameter `nStep` can only be 0 if `nFromValue == nToValue`.
	 * @param nFromValue The start value.
	 * @param nToValue The end value.
	 * @param nStep The step between generated values. Can be &lt; 0.
	 */
	IntSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept;
	/** Constructs integer set with array of unsorted values.
	 * If values are repeated they are ignored.
	 * @param aValues The values.
	 */
	explicit IntSet(const std::vector<int32_t>& aValues) noexcept;
	/** Constructs integer set with array of values.
	 * If values are repeated they are ignored.
	 * @param aValues The values.
	 * @param bIsSorted Whether the values are already sorted.
	 */
	explicit IntSet(const std::vector<int32_t>& aValues, bool bIsSorted) noexcept;
	/** Constructs integer set with array of unsorted values.
	 * If values are repeated they are ignored.
	 * @param aValues The values.
	 */
	explicit IntSet(std::vector<int32_t>&& aValues) noexcept;
	/** Constructs integer set with array of values.
	 * If values are repeated they are ignored.
	 * @param aValues The values.
	 * @param bIsSorted Whether the values are already sorted.
	 */
	explicit IntSet(std::vector<int32_t>&& aValues, bool bIsSorted) noexcept;

	IntSet(const IntSet& oSource) noexcept
	: m_nFromValue(oSource.m_nFromValue)
	, m_nToValue(oSource.m_nToValue)
	, m_nStep(oSource.m_nStep)
	, m_aValues(oSource.m_aValues)
	{
	}
	IntSet(IntSet&& oSource) noexcept
	: m_nFromValue(std::move(oSource.m_nFromValue))
	, m_nToValue(std::move(oSource.m_nToValue))
	, m_nStep(std::move(oSource.m_nStep))
	, m_aValues(std::move(oSource.m_aValues))
	{
	}
	IntSet& operator=(const IntSet& oSource) noexcept
	{
		m_nFromValue = oSource.m_nFromValue;
		m_nToValue = oSource.m_nToValue;
		m_nStep = oSource.m_nStep;
		m_aValues = oSource.m_aValues;
		return *this;
	}
	IntSet& operator=(IntSet&& oSource) noexcept
	{
		m_nFromValue = std::move(oSource.m_nFromValue);
		m_nToValue = std::move(oSource.m_nToValue);
		m_nStep = std::move(oSource.m_nStep);
		m_aValues = std::move(oSource.m_aValues);
		return *this;
	}

	/** The size of the integer set.
	 * @return The size.
	 */
	int32_t size() const noexcept;
	/** The value at a given index.
	 * @param nIdx The index. Must be &gt;= 0 and &lt; size().
	 * @return The value.
	 */
	int32_t getValueByIndex(int32_t nIdx) const noexcept;
	/** The index of the value.
	 * @param nValue The value to look for.
	 * @return The index (&gt;= 0) or -1 if value not in the set.
	 */
	int32_t getIndexOfValue(int32_t nValue) const noexcept;
	/** Get as vector of ordered unique values.
	 * Be aware that if size() is a big number, this function might terminate the program
	 * because of out of memory exception.
	 * @return The new vector.
	 */
	std::vector<int32_t> getAsVector() const noexcept;

	#ifndef NDEBUG
	void dump(int32_t nIndentSpaces, bool bHeader) const noexcept;
	#endif //NDEBUG
	/** Union of two IntSet.
	 * @param oSetL The first.
	 * @param oSetR The second.
	 * @return The result.
	 */
	static IntSet makeUnion(const IntSet& oSetL, const IntSet& oSetR) noexcept;
	/** Intersection of two IntSet.
	 * @param oSetL The first.
	 * @param oSetR The second.
	 * @return The result.
	 */
	static IntSet makeIntersection(const IntSet& oSetL, const IntSet& oSetR) noexcept;
	/** Difference of two IntSet.
	 * @param oSet The main int set.
	 * @param oSetExcl The set the values of which should not appear in the result.
	 * @return The set.
	 */
	static IntSet makeDifference(const IntSet& oSet, const IntSet& oSetExcl) noexcept;
private:
	int32_t m_nFromValue;
	int32_t m_nToValue;
	int32_t m_nStep;
	std::vector<int32_t> m_aValues;
};


} // namespace stmg

#endif	/* STMG_INT_SET_H */

