/*
 * File:   randomevent.h
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

#ifndef STMG_RANDOM_EVENT_H
#define STMG_RANDOM_EVENT_H

#include "event.h"

#include "util/circularbuffer.h"

#include <string>
#include <vector>
#include <limits>
#include <memory>

#include <stdint.h>

namespace stmg
{

class GameProxy;

using std::shared_ptr;
using std::weak_ptr;

/** Random number generator event.
 * Permutation example: range (m_nFrom = 0, m_nTo = 2100), s_nMaxPermutationSize = 1000
 *     m_nTotPartitions = ceil(2101 / 1000) = 3
 *     m_nPartitionMinSize = 2101 / 3 = 700
 *     Permutation sizes: 701, 700, 700
 *     First permutation emitted (unshuffled):  0,3,6,9,12, .. 2097,2100
 *     Second permutation emitted (unshuffled): 1,4,7,10,13, .. 2098
 *     Third permutation emitted (unshuffled):  2,5,8,11,14, .. 2099
 *     Fourth permutation emitted (unshuffled): 0,3,6,9,12, .. 2097,2100
 *     Fifth permutation emitted (unshuffled):  1,4,7,10,13, .. 2098
 *     And so on
 */

class RandomEvent : public Event
{
public:
	static constexpr int32_t s_nDefaultSharedBufferSize = 1000; /**< The default shared buffer size. */
	static constexpr int32_t s_nMaxPermutationSize = 200; /**< The maximum size of a permutation of the range. */
	static constexpr int32_t s_nPermutationMinValue = -32000; /**< The minimum for the Init::m_nFrom value if Init::m_bPermutations is true. */
	static constexpr int32_t s_nPermutationMaxValue = +32000; /**< The maximum for the Init::m_nTo value if Init::m_bPermutations is true. */
	struct LocalInit
	{
		int32_t m_nFrom = std::numeric_limits<int32_t>::lowest(); /**< The random number lowest value. Default is std::numeric_limits<int32_t>::lowest(). */
		int32_t m_nTo = std::numeric_limits<int32_t>::max(); /**< The random number highest value. Default is std::numeric_limits<int32_t>::max(). */
		std::string m_sSharedName; /**< The shared name. For two RandomEvent instances to share the same sequence they also
									 * must have same (m_nFrom, m_nTo) range. If empty random sequence not shared. */
		int32_t m_nBufferSize = s_nDefaultSharedBufferSize; /**< The maximal shared buffer size. Ignored if sSharedName is empty.
															 * If &lt;= 0 s_nDefaultSharedBufferSize is used. Default is s_nDefaultSharedBufferSize. */
		bool m_bPermutations = false; /**< Whether output sequence is permutations of the range [m_nFrom, m_nTo].
										 * If the range is bigger than s_nMaxPermutationSize then subsets of the range are permutated in a cyclic way. */
	};
	struct Init : public Event::Init, public LocalInit
	{
	};
	/** Constructor.
	 * If the shared random sequence with name sSharedName already exists, the
	 * buffer size is ignored since the buffer already has been created.
	 * @param oInit The parameters.
	 */
	explicit RandomEvent(Init&& oInit) noexcept;
protected:
	/** See constructor.
	 */
	void reInit(Init&& oInit) noexcept;
public:

	void trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept override;

	// input
	enum {
		MESSAGE_GENERATE = 100 /**< Generate random int. */
		, MESSAGE_GENERATE_ADD = 101 /**< Generate random int and add nValue to the result. */
	};
	// output
	enum {
		LISTENER_GROUP_RANDOM = 10 /**< The generated random. */
	};

private:
	struct RandomSequence
	{
		//
		void initPartitions() noexcept;
		void createPermutation(GameProxy& oGame) noexcept;
		int32_t nextRandom(GameProxy& oGame) noexcept;

		int32_t m_nFromValue = 0;
		int32_t m_nToValue = 0;
		int32_t m_nTotPartitions = 0;
		int32_t m_nPartitionMinSize = 0;
		int32_t m_nTotPartitionsWithRest = 0;
		//
		int32_t m_nPartitionCount = 0;
		int32_t m_nPermutationCount = 0;
		std::vector<int32_t> m_aPermutation;
	};
	struct SharedRandomSequence : public RandomSequence
	{
		SharedRandomSequence() noexcept;
		explicit SharedRandomSequence(int32_t nSize) noexcept;

		CircularBuffer<int32_t> m_oBuffer;
		int32_t m_nTotConsumed = 0;
	};

private:
	void reInitCommon() noexcept;
	shared_ptr<RandomSequence> getSharedRandomSequence(int64_t nGameId, const std::string& sSharedName, int32_t nBufferSize) noexcept;
	shared_ptr<RandomSequence> getRandomSequence() noexcept;
private:
	LocalInit m_oData;

	bool m_bShared;
	shared_ptr<RandomSequence> m_refRandomSequence;
	int32_t m_nTotRead;

	struct SharedRandom
	{
		int64_t m_nGameId;
		std::string m_sSharedName;
		weak_ptr<SharedRandomSequence> m_refSharedRandomSequence;
		int32_t m_nFrom;
		int32_t m_nTo;
	};
	static std::vector<SharedRandom> m_aSharedRandoms;

private:
	RandomEvent() = delete;
	RandomEvent(const RandomEvent& oSource) = delete;
	RandomEvent& operator=(const RandomEvent& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_RANDOM_EVENT_H */

