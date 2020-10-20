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
 * File:   intset.cc
 */

#include "util/intset.h"

#include <algorithm>
#include <iostream>
#include <cassert>
#include <cstdint>
#include <iterator>
#include <string>

namespace stmg
{

IntSet::IntSet(int32_t nFromValue, int32_t nToValue) noexcept
: m_nFromValue(nFromValue)
, m_nToValue(nToValue)
, m_nStep(1)
{
	if (nFromValue > nToValue) {
		std::swap(m_nFromValue, m_nToValue);
	}
}
IntSet::IntSet(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
: m_nFromValue(nFromValue)
, m_nToValue(nToValue)
, m_nStep(nFromValue == nToValue ? 1 : nStep)
{
	assert(nFromValue <= nToValue ? (m_nStep > 0) : (m_nStep < 0));
	if (m_nFromValue > m_nToValue) {
		if (m_nStep < 1) {
			const int64_t nF = (static_cast<int64_t>(m_nToValue) - m_nFromValue) / m_nStep;
			m_nToValue = nF * m_nStep + m_nFromValue;
		}
		std::swap(m_nFromValue, m_nToValue);
		m_nStep = -m_nStep;
	}
}

IntSet::IntSet(const std::vector<int32_t>& aValues) noexcept
: IntSet(std::vector<int32_t>{aValues})
{
}
IntSet::IntSet(const std::vector<int32_t>& aValues, bool bIsSorted) noexcept
: IntSet(std::vector<int32_t>{aValues}, bIsSorted)
{
}
IntSet::IntSet(std::vector<int32_t>&& aValues) noexcept
: IntSet(std::move(aValues), false)
{
}
IntSet::IntSet(std::vector<int32_t>&& aValues, bool bIsSorted) noexcept
: m_nFromValue(0)
, m_nToValue(0)
, m_nStep(0)
, m_aValues(std::move(aValues))
{
	const auto nSize = m_aValues.size();
	if (nSize == 1) {
		m_nFromValue = m_aValues[0];
		m_aValues.clear();
		m_nToValue = m_nFromValue;
		m_nStep = 1;
	} else {
		if (! bIsSorted) {
			std::sort(m_aValues.begin(), m_aValues.end());
		} else {
			assert(std::is_sorted(m_aValues.begin(), m_aValues.end()));
		}
		m_aValues.erase(std::unique(m_aValues.begin(), m_aValues.end()), m_aValues.end());
	}
}
int32_t IntSet::size() const noexcept
{
	if (m_nStep == 0) {
		return m_aValues.size();
	} else {
		return (static_cast<int64_t>(m_nToValue) - m_nFromValue) / m_nStep + 1;
	}
}

int32_t IntSet::getValueByIndex(int32_t nIdx) const noexcept
{
	assert((nIdx >= 0) && (nIdx < size()));
	if (m_nStep == 0) {
		return m_aValues[nIdx];
	} else {
		return m_nFromValue + m_nStep * nIdx;
	}
}
int32_t IntSet::getIndexOfValue(int32_t nValue) const noexcept
{
	const int32_t nTraitNotInSet = -1;
	if (m_nStep == 0) {
		auto itFind = std::find(m_aValues.begin(), m_aValues.end(), nValue);
		if (itFind == m_aValues.end()) {
			return nTraitNotInSet; //-------------------------------------------
		}
		return std::distance(m_aValues.begin(), itFind); //---------------------
	}
	assert(m_nStep > 0);
	if (nValue > m_nToValue) {
		return nTraitNotInSet; //-----------------------------------------------
	}
	if (nValue < m_nFromValue) {
		return nTraitNotInSet; //-----------------------------------------------
	}
	const int64_t nRel = static_cast<int64_t>(nValue) - m_nFromValue;
	if (nRel % m_nStep != 0) {
		return nTraitNotInSet; //-----------------------------------------------
	}
	const int32_t nIdx = nRel / m_nStep;
	return nIdx;
}

static IntSet setUnion(const std::vector<int32_t>& aSetL, const std::vector<int32_t>& aSetR) noexcept
{
	std::vector<int32_t> aSetC;
	std::set_union(aSetL.begin(), aSetL.end(), aSetR.begin(), aSetR.end()
					, std::back_inserter(aSetC));
	return IntSet{aSetC};
}
static IntSet setIntersection(const std::vector<int32_t>& aSetL, const std::vector<int32_t>& aSetR) noexcept
{
	std::vector<int32_t> aSetC;
	std::set_intersection(aSetL.begin(), aSetL.end(), aSetR.begin(), aSetR.end()
						, std::back_inserter(aSetC));
	return IntSet{aSetC};
}
static IntSet setDifference(const std::vector<int32_t>& aSetL, const std::vector<int32_t>& aSetR) noexcept
{
	std::vector<int32_t> aSetC;
	std::set_difference(aSetL.begin(), aSetL.end(), aSetR.begin(), aSetR.end()
						, std::back_inserter(aSetC));
	return IntSet{aSetC};
}
static std::vector<int32_t> fromToValues(int32_t nFromValue, int32_t nToValue, int32_t nStep) noexcept
{
	assert(nStep > 0);
	std::vector<int32_t> aRes;
	for (int32_t nValue = nFromValue; nValue <= nToValue; nValue += nStep) {
		aRes.push_back(nValue);
	}
	return aRes;
}
std::vector<int32_t> IntSet::getAsVector() const noexcept
{
	const bool bIsValues = (m_nStep == 0);
	if (bIsValues) {
		return m_aValues;
	} else {
		return fromToValues(m_nFromValue, m_nToValue, m_nStep);
	}
}
IntSet IntSet::makeUnion(const IntSet& oSetL, const IntSet& oSetR) noexcept
{
	const bool bIsValuesL = (oSetL.m_nStep == 0);
	const bool bIsValuesR = (oSetR.m_nStep == 0);
	if (bIsValuesL == bIsValuesR) {
		if (bIsValuesL && bIsValuesR) {
			return setUnion(oSetL.m_aValues, oSetR.m_aValues); //---------------
		}
		std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setUnion(aSetL, aSetR); //---------------------------------------
	} else {
		if (bIsValuesR) {
			std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
			return setUnion(aSetL, oSetR.m_aValues); //-------------------------
		}
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setUnion(oSetL.m_aValues, aSetR); //-----------------------------
	}
}
IntSet IntSet::makeIntersection(const IntSet& oSetL, const IntSet& oSetR) noexcept
{
	const bool bIsValuesL = (oSetL.m_nStep == 0);
	const bool bIsValuesR = (oSetR.m_nStep == 0);
	if (bIsValuesL == bIsValuesR) {
		if (bIsValuesL && bIsValuesR) {
			return setIntersection(oSetL.m_aValues, oSetR.m_aValues); //--------
		}
		std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setIntersection(aSetL, aSetR); //-----------------------------
	} else {
		if (bIsValuesR) {
			std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
			return setIntersection(aSetL, oSetR.m_aValues); //------------------
		}
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setIntersection(oSetL.m_aValues, aSetR); //----------------------
	}
}
IntSet IntSet::makeDifference(const IntSet& oSetL, const IntSet& oSetR) noexcept
{
	const bool bIsValuesL = (oSetL.m_nStep == 0);
	const bool bIsValuesR = (oSetR.m_nStep == 0);
	if (bIsValuesL == bIsValuesR) {
		if (bIsValuesL && bIsValuesR) {
			return setDifference(oSetL.m_aValues, oSetR.m_aValues); //----------
		}
		std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setDifference(aSetL, aSetR); //----------------------------------
	} else {
		if (bIsValuesR) {
			std::vector<int32_t> aSetL = fromToValues(oSetL.m_nFromValue, oSetL.m_nToValue, oSetL.m_nStep);
			return setDifference(aSetL, oSetR.m_aValues); //--------------------
		}
		std::vector<int32_t> aSetR = fromToValues(oSetR.m_nFromValue, oSetR.m_nToValue, oSetR.m_nStep);
		return setDifference(oSetL.m_aValues, aSetR); //------------------------
	}
}

#ifndef NDEBUG
void IntSet::dump(int32_t nIndentSpaces, bool bHeader) const noexcept
{
	auto sIndent = std::string(nIndentSpaces, ' ');
	std::cout << sIndent;
	if (bHeader) {
		std::cout << "IntSet";
	}
	std::cout << "(";
	if (m_nStep == 0) {
		bool bFirst = true;
		for (const auto& nVal : m_aValues) {
			if (!bFirst) {
				std::cout << ", ";
			} else {
				bFirst = false;
			}
			std::cout << nVal;
		}
	} else {
		std::cout << "m_nFromValue=" << m_nFromValue << " m_nToValue=" << m_nToValue << " m_nStep=" << m_nStep;
	}
	std::cout << ")" << '\n';
}
#endif //NDEBUG

} // namespace stmg
