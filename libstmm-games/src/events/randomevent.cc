/*
 * File:   randomevent.cc
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

#include "events/randomevent.h"

#include "level.h"
#include "gameproxy.h"

#include <cassert>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <cstdint>
#include <utility>


namespace stmg
{

std::vector<RandomEvent::SharedRandom> RandomEvent::m_aSharedRandoms{};

void RandomEvent::RandomSequence::initPartitions() noexcept
{
	const int32_t nTotValues = m_nToValue - m_nFromValue + 1;
	m_nTotPartitions = std::ceil(1.0 * nTotValues / s_nMaxPermutationSize);
	m_nPartitionMinSize = nTotValues / m_nTotPartitions;
	m_nTotPartitionsWithRest = nTotValues % m_nTotPartitions;
	m_nPartitionCount = 0;
	m_nPermutationCount = 0;
	m_aPermutation.reserve(m_nPartitionMinSize + 1);
}
void RandomEvent::RandomSequence::createPermutation(GameProxy& oGame) noexcept
{
	const int32_t nPartitionSize = m_nPartitionMinSize + ((m_nPartitionCount < m_nTotPartitionsWithRest) ? 1 : 0);
	m_aPermutation.resize(nPartitionSize);
	int32_t nCurValue = m_nFromValue + m_nPermutationCount - m_nTotPartitions;
	std::generate(m_aPermutation.begin(), m_aPermutation.end(), [&]()
		{
			nCurValue += m_nTotPartitions;
			return nCurValue;
		});
	// copied from deprecated std::random_shuffle
	for (int32_t nIdx = nPartitionSize - 1; nIdx > 0; --nIdx) {
		const int32_t nOtherIdx = oGame.random(0, nIdx);
		std::swap(m_aPermutation[nIdx], m_aPermutation[nOtherIdx]);
	}
}
int32_t RandomEvent::RandomSequence::nextRandom(GameProxy& oGame) noexcept
{
	if (m_nTotPartitions == 0) {
		return oGame.random(m_nFromValue, m_nToValue); //-----------------------
	}
	const int32_t nRand = m_aPermutation[m_nPermutationCount];
	++m_nPermutationCount;
	if (m_nPermutationCount >= static_cast<int32_t>(m_aPermutation.size())) {
		m_nPermutationCount = 0;
		++m_nPartitionCount;
		if (m_nPartitionCount == m_nTotPartitions) {
			m_nPartitionCount = 0;
		}
		createPermutation(oGame);
	}
	return nRand;
}
RandomEvent::SharedRandomSequence::SharedRandomSequence() noexcept
: m_oBuffer(s_nDefaultSharedBufferSize)
{
}
RandomEvent::SharedRandomSequence::SharedRandomSequence(int32_t nSize) noexcept
: m_oBuffer(nSize)
{
}

RandomEvent::RandomEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
{
	reInitCommon();
}
void RandomEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_refRandomSequence.reset();
	reInitCommon();
}
void RandomEvent::reInitCommon() noexcept
{
	assert(m_oData.m_nFrom <= m_oData.m_nTo);
	if (! m_oData.m_sSharedName.empty()) {
		if (m_oData.m_nBufferSize <= 0) {
			m_oData.m_nBufferSize = s_nDefaultSharedBufferSize;
		}
	}
	if (m_oData.m_bPermutations) {
		assert(m_oData.m_nFrom >= s_nPermutationMinValue);
		assert(m_oData.m_nTo <= s_nPermutationMaxValue);
	}
	m_bShared = ! m_oData.m_sSharedName.empty();
	m_nTotRead = 0;
}
void RandomEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
//std::cout << "RandomEvent::trigger" << '\n';
	if (p0TriggeringEvent == nullptr) {
		return;
	}
	const bool bAddValue = (nMsg == MESSAGE_GENERATE_ADD);
	if (! ((nMsg == MESSAGE_GENERATE) || bAddValue)) {
		return; //--------------------------------------------------------------
	}
	Level& oLevel = level();
	auto& oGame = oLevel.game();
	if (! m_refRandomSequence) {
		if (m_bShared) {
			m_refRandomSequence = getSharedRandomSequence(oGame.getGameId(), m_oData.m_sSharedName, m_oData.m_nBufferSize);
		} else {
			m_refRandomSequence = getRandomSequence();
		}
		if (m_oData.m_bPermutations) {
			m_refRandomSequence->initPartitions();
		}
	}
	if (m_oData.m_bPermutations && m_refRandomSequence->m_aPermutation.empty()) {
		m_refRandomSequence->createPermutation(oGame);
	}
	int32_t nRand;
	if (m_bShared) {
		SharedRandomSequence& oSRS = *(static_cast<SharedRandomSequence*>(m_refRandomSequence.get()));
		const int32_t nTotConsumed = oSRS.m_nTotConsumed;
		if (m_nTotRead < nTotConsumed) {
			oLevel.gameStatusTechnical(std::vector<std::string>{"RandomEvent::trigger()", "Sorry event's shared buffer is full."
																, "Discrepancy between events reading from the same random buffer too high."});
			return; //----------------------------------------------------------
		}
		const int32_t nTotBufferedRead = m_nTotRead - nTotConsumed;
		CircularBuffer<int32_t>& oBuffer = oSRS.m_oBuffer;
		if (nTotBufferedRead == oBuffer.size()) {
			// add a new value to the buffer
			if (oBuffer.isFull()) {
				// make place
				oBuffer.read();
				++(oSRS.m_nTotConsumed);
			}
			nRand = oSRS.nextRandom(oGame);
			oBuffer.write(nRand);
		} else {
			nRand = oBuffer.peekValue(nTotBufferedRead);
		}
		++m_nTotRead;
	} else {
		nRand = m_refRandomSequence->nextRandom(oGame);
	}
	if (bAddValue) {
		nRand += nValue;
	}
	informListeners(LISTENER_GROUP_RANDOM, nRand);
}
shared_ptr<RandomEvent::RandomSequence> RandomEvent::getRandomSequence() noexcept
{
	auto refRandomSequence = std::make_shared<SharedRandomSequence>();
	refRandomSequence->m_nFromValue = m_oData.m_nFrom;
	refRandomSequence->m_nToValue = m_oData.m_nTo;
	return refRandomSequence;
}
shared_ptr<RandomEvent::RandomSequence> RandomEvent::getSharedRandomSequence(int64_t nGameId, const std::string& sSharedName, int32_t nBufferSize) noexcept
{
	int32_t nFreeIdx = -1;
	const int32_t nSize = static_cast<int32_t>(m_aSharedRandoms.size());
	for (int32_t nIdx = 0; nIdx < nSize; ++nIdx) {
		SharedRandom& oSR = m_aSharedRandoms[nIdx];
		bool bExpiredChecked = false;
		bool bExpired = false;
		if (nFreeIdx < 0) {
			bExpired = oSR.m_refSharedRandomSequence.expired();
			bExpiredChecked = true;
			if (bExpired) {
				nFreeIdx = nIdx;
			}
		}
		if ((oSR.m_nGameId == nGameId) && (oSR.m_sSharedName == sSharedName)) {
			if (! bExpiredChecked) {
				bExpired = oSR.m_refSharedRandomSequence.expired();
			}
			if (! bExpired) {
				if (! ((oSR.m_nTo == m_oData.m_nTo) && (oSR.m_nFrom == m_oData.m_nFrom))) {
					m_oData.m_nFrom = oSR.m_nFrom;
					m_oData.m_nTo = oSR.m_nTo;
					std::cout << "Warning! RandomEvent: shared random '" << sSharedName << "' has different range" << '\n';
				}
				return oSR.m_refSharedRandomSequence.lock(); //-----------------------
			}
		}
	}
	if (nFreeIdx < 0) {
		m_aSharedRandoms.emplace_back();
		nFreeIdx = nSize;
	}
	SharedRandom& oSR = m_aSharedRandoms[nFreeIdx];
	oSR.m_nGameId = nGameId;
	oSR.m_sSharedName = sSharedName;
	oSR.m_nFrom = m_oData.m_nFrom;
	oSR.m_nTo = m_oData.m_nTo;
	auto refRandomSequence = ((nBufferSize > 0) ? std::make_shared<SharedRandomSequence>(nBufferSize) : std::make_shared<SharedRandomSequence>());
	refRandomSequence->m_nFromValue = m_oData.m_nFrom;
	refRandomSequence->m_nToValue = m_oData.m_nTo;
	oSR.m_refSharedRandomSequence = refRandomSequence;
	return refRandomSequence;
}

} // namespace stmg
