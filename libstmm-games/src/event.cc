/*
 * File:   event.cc
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

#include "event.h"

#include "level.h"

#include <vector>
#include <cassert>
#include <algorithm>
#include <cstdint>
#include <limits>
#include <utility>

//#include <iostream>


namespace stmg
{

Event::Event(Init&& oInit) noexcept
: m_nPriority(oInit.m_nPriority)
, m_nTriggerTime(-1)
, m_nDebugTag(0)
, m_p0Level(oInit.m_p0Level)
, m_bIsActive(false)
{
	assert(oInit.m_p0Level != nullptr);
}
void Event::reInit(Init&& oInit) noexcept
{
	assert(oInit.m_p0Level != nullptr);

	m_nPriority = oInit.m_nPriority;
	m_nTriggerTime = -1;
	m_nDebugTag = 0;
	m_p0Level = oInit.m_p0Level;
	m_bIsActive = false;
	m_aListenerGroupIds.clear();
	m_aListeners.clear();
}

void Event::addListener(int32_t nGroupId, Event* p0ListenerEvent, int32_t nMsg) noexcept
{
	assert(p0ListenerEvent != nullptr);
	m_aListenerGroupIds.push_back(nGroupId);
	EventListener oEL;
	oEL.m_p0TargetEvent = p0ListenerEvent;
	oEL.m_nMsg = nMsg;
	m_aListeners.push_back(std::move(oEL));
}

void Event::informListeners(int32_t nGroupId, int32_t nValue) noexcept
{
	assert(nGroupId >= 0);
	const int32_t nTotListeners = static_cast<int32_t>(m_aListenerGroupIds.size());
	for (int32_t nIdx = 0; nIdx < nTotListeners; ++nIdx) {
		const int32_t& nCurGroupId = m_aListenerGroupIds[nIdx];
		if ((nCurGroupId < 0) || (nCurGroupId == nGroupId)) {
			const EventListener& oEL = m_aListeners[nIdx];
			int32_t nMsg = oEL.m_nMsg;
			if (nMsg < 0) {
				nMsg = nGroupId;
			}
			m_p0Level->triggerEvent(oEL.m_p0TargetEvent, nMsg, nValue, this);
		}
	}
}

void Event::addMsgFilter(int32_t nInFromMsg, int32_t nInToMsg, int32_t nInFromValue, int32_t nInToValue
						, bool bOutMsgDefined, int32_t nOutMsg
						, MSG_FILTER_VALUE_OP eOutValueOperator, int32_t nOutValueOperand, int32_t nOutValueAdd) noexcept
{
//std::cout << "Event::addMsgFilter nInFromMsg=" << nInFromMsg << " nInToMsg=" << nInToMsg;
//std::cout << "  nInFromValue=" << nInFromValue << " nInToValue=" << nInToValue;
//std::cout << "  bOutMsgDefined=" << bOutMsgDefined << " nOutMsg=" << nOutMsg;
//std::cout << "  eOutValueOperator=" << static_cast<int32_t>(eOutValueOperator);
//std::cout << "  nOutValueOperand=" << nOutValueOperand << " nOutValueAdd=" << nOutValueAdd << '\n';
	assert((eOutValueOperator >= MSG_FILTER_VALUE_OP_FIRST) && (eOutValueOperator <= MSG_FILTER_VALUE_OP_LAST));
	assert(! ((eOutValueOperator == MSG_FILTER_VALUE_OP_SET) && (nOutValueAdd != 0)));
	assert(! ((eOutValueOperator == MSG_FILTER_VALUE_OP_DIV_ADD) && (nOutValueOperand == 0)));
	assert(! ((eOutValueOperator == MSG_FILTER_VALUE_OP_MOD_ADD) && (nOutValueOperand == 0)));
	assert((nInFromMsg >= -1) && (nInFromMsg <= nInToMsg));
	assert(nInFromValue <= nInToValue);
	assert((!bOutMsgDefined) || (nOutMsg >= -1));
	MsgFilterIn oMfIn;
	oMfIn.m_nInFromMsg = nInFromMsg;
	oMfIn.m_nInToMsg = nInToMsg;
	oMfIn.m_nInFromValue = nInFromValue;
	oMfIn.m_nInToValue = nInToValue;
	m_aMsgFilterIn.push_back(oMfIn);
	MsgFilterOut oMfOut;
	oMfOut.m_bOutMsgDefined = bOutMsgDefined;
	oMfOut.m_nOutMsg = nOutMsg;
	oMfOut.m_eOutValueOperator = eOutValueOperator;
	oMfOut.m_nOutValueOperand  = nOutValueOperand;
	oMfOut.m_nOutValueAdd = nOutValueAdd;
	m_aMsgFilterOut.push_back(oMfOut);
}
static int32_t clampToInt(double fCheck)
{
	if (fCheck > std::numeric_limits<int32_t>::max()) {
		return std::numeric_limits<int32_t>::max();
	} else if (fCheck < std::numeric_limits<int32_t>::lowest()) {
		return std::numeric_limits<int32_t>::lowest();
	} else {
		return fCheck;
	}
}
bool Event::filterMsg(int32_t& nMsg, int32_t& nValue) noexcept
{
//std::cout << "Event(" << (int64_t)this << ")::filterMsg(nMsg=" << nMsg << ", nValue=" << nValue << ")" << '\n';
	const int32_t nTotFilters = static_cast<int32_t>(m_aMsgFilterIn.size());
	for (int32_t nFilterIdx = 0; nFilterIdx < nTotFilters; ++nFilterIdx) {
		const MsgFilterIn& oMfIn = m_aMsgFilterIn[nFilterIdx];
		if (! ((nMsg >= oMfIn.m_nInFromMsg) && (nMsg <= oMfIn.m_nInToMsg))) {
			// no match
			continue; // for -----
		}
		if (! ((nValue >= oMfIn.m_nInFromValue) && (nValue <= oMfIn.m_nInToValue))) {
			// no match
			continue; // for -----
		}
		// match
		MsgFilterOut& oMfOut = m_aMsgFilterOut[nFilterIdx];
		if ((!oMfOut.m_bOutMsgDefined) && (oMfOut.m_eOutValueOperator == MSG_FILTER_VALUE_OP_UNCHANGED)) {
			// blocked
//std::cout << "Event::filterMsg output= blocked" << '\n';
			return false; //----------------------------------------------------
		}
		if (oMfOut.m_bOutMsgDefined) {
			nMsg = oMfOut.m_nOutMsg;
		}
		switch (oMfOut.m_eOutValueOperator) {
			case MSG_FILTER_VALUE_OP_UNCHANGED:
			{
			} break;
			case MSG_FILTER_VALUE_OP_SET:
			{
				nValue = oMfOut.m_nOutValueOperand;
			} break;
			case MSG_FILTER_VALUE_OP_MULT_ADD:
			{
				const double fCheck = 1.0 * nValue * oMfOut.m_nOutValueOperand + oMfOut.m_nOutValueAdd;
				nValue = clampToInt(fCheck);
			} break;
			case MSG_FILTER_VALUE_OP_DIV_ADD:
			{
				if (oMfOut.m_nOutValueOperand == 0) {
					nValue = oMfOut.m_nOutValueAdd;
				} else {
					const double fCheck = 1.0 * nValue / oMfOut.m_nOutValueOperand + oMfOut.m_nOutValueAdd;
					nValue = clampToInt(fCheck);
				}
			} break;
			case MSG_FILTER_VALUE_OP_PERCENT_ADD:
			{
				const double fCheck = 1.0 * nValue * oMfOut.m_nOutValueOperand / 100 + oMfOut.m_nOutValueAdd;
				nValue = clampToInt(fCheck);
			} break;
			case MSG_FILTER_VALUE_OP_PERMILL_ADD:
			{
				const double fCheck = 1.0 * nValue * oMfOut.m_nOutValueOperand / 1000 + oMfOut.m_nOutValueAdd;
				nValue = clampToInt(fCheck);
			} break;
			case MSG_FILTER_VALUE_OP_MOD_ADD:
			{
				if (oMfOut.m_nOutValueOperand == 0) {
					nValue = oMfOut.m_nOutValueAdd;
				} else {
					const double fCheck = 1.0 * (nValue % oMfOut.m_nOutValueOperand) + oMfOut.m_nOutValueAdd;
					nValue = clampToInt(fCheck);
				}
			} break;
			//case MSG_FILTER_VALUE_OP_FLOORMOD_ADD:
			//{
			//	if (oMfOut.m_nOutValueOperand == 0) {
			//		nValue = oMfOut.m_nOutValueAdd;
			//	} else {
			//		const double fCheck = std::floor(1.0 * nValue / oMfOut.m_nOutValueOperand) * oMfOut.m_nOutValueOperand + oMfOut.m_nOutValueAdd;
			//		nValue = clampToInt(fCheck);
			//	}
			//} break;
			case MSG_FILTER_VALUE_OP_MIN_ADD:
			{
				const double fCheck = 1.0 * std::min(nValue, oMfOut.m_nOutValueOperand) + oMfOut.m_nOutValueAdd;
				nValue = clampToInt(fCheck);
			} break;
			case MSG_FILTER_VALUE_OP_MAX_ADD:
			{
				const double fCheck = 1.0 * std::max(nValue, oMfOut.m_nOutValueOperand) + oMfOut.m_nOutValueAdd;
				nValue = clampToInt(fCheck);
			} break;
		}
//std::cout << "Event::filterMsg output= (nMsg=" << nMsg << ", nValue=" << nValue << ")" << '\n';
		return true; //---------------------------------------------------------
	}
//std::cout << "Event::filterMsg output= unchanged" << '\n';
	return true;
}
void Event::setDebugTag(int32_t nDebugTag) noexcept
{
	m_nDebugTag = nDebugTag;
}
int32_t Event::getDebugTag() const noexcept
{
	return m_nDebugTag;
}

} // namespace stmg
