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
 * File:   randomparts.h
 */

#ifndef STMG_RANDOM_PARTS_H
#define STMG_RANDOM_PARTS_H

#include <cassert>
#include <iostream>
#include <limits>

#include <vector>

#include <stdint.h>

namespace stmg
{

template <class T>
class RandomParts
{
public:
	RandomParts() noexcept : m_nRandomPartsTotProb(0) {};

	//TODO take RandomPart away from method names
	/** Removes all parts.
	 */
	void clear() noexcept
	{
		m_nRandomPartsTotProb = 0;
		m_aRandomPart.clear();
		m_aData.clear();
	}
	/** Adds a random part.
	 * @param nProb The weight of the part (must be non negative).
	 * @param oData The associated data. A copy is made.
	 * @return The index of the added part.
	 */
	int32_t addRandomPart(int32_t nProb, const T& oData) noexcept
	{
		assert(nProb >= 0);
		assert(m_nRandomPartsTotProb <= std::numeric_limits<int32_t>::max() - nProb);
		m_nRandomPartsTotProb += nProb;
		RandomPart oPart;
		oPart.m_nProb = nProb;
		oPart.m_nCumulProb = m_nRandomPartsTotProb;
		m_aRandomPart.push_back(oPart);
		m_aData.push_back(oData);
		return static_cast<int32_t>(m_aData.size()) - 1;
	}
	/** Adds a random part.
	 * @param nProb The weight of the part (must be non negative).
	 * @param oData The associated data.
	 * @return The index of the added part.
	 */
	int32_t addRandomPart(int32_t nProb, T&& oData) noexcept
	{
		assert(nProb >= 0);
		assert(m_nRandomPartsTotProb <= std::numeric_limits<int32_t>::max() - nProb);
		m_nRandomPartsTotProb += nProb;
		//
		m_aRandomPart.emplace_back();
		RandomPart& oPart = m_aRandomPart.back();
		oPart.m_nProb = nProb;
		oPart.m_nCumulProb = m_nRandomPartsTotProb;
		m_aData.push_back(std::move(oData));
		return static_cast<int32_t>(m_aData.size()) - 1;
	}
	/** Get random part object.
	 * @param nIdx The index of the random part. Must be &gt;= 0 and &lt; getTotRandomParts().
	 * @return The object associated with the part.
	 */
	T& getRandomPart(int32_t nIdx) noexcept
	{
		assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aData.size())));
		return m_aData[nIdx];
	}
	/** Get random part object and info.
	 * @param nIdx The index of the random part. Must be &gt;= 0 and &lt; getTotRandomParts().
	 * @param nProb [output] The weight of the part.
	 * @return The object associated with the part.
	 */
	T& getRandomPart(int32_t nIdx, int32_t& nProb) noexcept
	{
		assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aData.size())));
		nProb = m_aRandomPart[nIdx].m_nProb;
		return m_aData[nIdx];
	}
	/** Get random part object and info.
	 * @param nIdx The index of the random part. Must be &gt;= 0 and &lt; getTotRandomParts().
	 * @param nProb [output] The weight of the part.
	 * @param nPrecProb [output] The sum of the weights of the preceding parts.
	 * @return The object associated with the part.
	 */
	T& getRandomPart(int32_t nIdx, int32_t& nProb, int32_t& nPrecProb) noexcept
	{
		assert((nIdx >= 0) && (nIdx < static_cast<int32_t>(m_aData.size())));
		nProb = m_aRandomPart[nIdx].m_nProb;
		nPrecProb = m_aRandomPart[nIdx].m_nCumulProb - nProb;
		return m_aData[nIdx];
	}
	/** The number of added parts.
	 * @return The total number of parts.
	 */
	int32_t getTotRandomParts() const noexcept
	{
		return static_cast<int32_t>(m_aData.size());
	}
	/** The sum of all the parts weights.
	 * @return The total weight.
	 */
	int32_t getRandomRange() const noexcept
	{
		return m_nRandomPartsTotProb;
	}
	int32_t getRandomPartProbIdx(int32_t nRandom) noexcept
	{
		assert((nRandom >= 0) && (nRandom < m_nRandomPartsTotProb));
		// find idx with binary search
		int32_t nIdx;
		int32_t nLeft = 0;
		int32_t nRight = static_cast<int32_t>(m_aData.size()) - 1;
		assert(nRight >= 0);

		while (nLeft + 1 < nRight) {
			nIdx = (nLeft + nRight) / 2;
			if (nRandom < m_aRandomPart[nIdx].m_nCumulProb) {
				nRight = nIdx;
			} else {
				nLeft = nIdx;
			}
		}

		if (nRandom < m_aRandomPart[nLeft].m_nCumulProb) {
			nIdx = nLeft;
		} else {
			nIdx = nRight;
		}
		return nIdx;
	}
	/** Get the part given the probability.
	 * nRel is set by this function to the value relative to the part 0 .. nProb-1,
	 * where nProb is what is returned by getRandomPart(nIdx, nProb)
	 * @param nRandom The probability. Must be &gt;= 0 and &lt; getRandomRange().
	 * @param nIdx [output] The index of the selected part.
	 * @param nRel [output] The probability within the weight of the part.
	 * @return The object associated with the selected part.
	 */
	T& getRandomPartProb(int32_t nRandom, int32_t& nIdx, int32_t& nRel) noexcept
	{
		nIdx = getRandomPartProbIdx(nRandom);
		if (nIdx == 0) {
			nRel = nRandom;
		} else {
			nRel = nRandom - m_aRandomPart[nIdx - 1].m_nCumulProb;
		}
		return m_aData[nIdx];
	}
	/** Get the part given the probability.
	 * @param nRandom The probability. Must be &gt;= 0 and &lt; getRandomRange().
	 * @param nIdx [output] The index of the selected part.
	 * @return The object associated with the selected part.
	 */
	T& getRandomPartProb(int32_t nRandom, int32_t& nIdx) noexcept
	{
		nIdx = getRandomPartProbIdx(nRandom);
		return m_aData[nIdx];
	}
	/** Get the part given the probability.
	 * @param nRandom The probability. Must be &gt;= 0 and &lt; getRandomRange().
	 * @return The object associated with the selected part.
	 */
	T& getRandomPartProb(int32_t nRandom) noexcept
	{
		int32_t nIdx;
		return getRandomPartProb(nRandom, nIdx);
	}
private:
	struct RandomPart
	{
		int32_t m_nProb;
		int32_t m_nCumulProb;
	};

	int32_t m_nRandomPartsTotProb;
	std::vector<RandomPart> m_aRandomPart;
	std::vector<T> m_aData;

private:
	RandomParts(const RandomParts& oSource) = delete;
	RandomParts& operator=(const RandomParts& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_RANDOM_PARTS_H */

