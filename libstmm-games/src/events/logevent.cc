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
 * File:   logevent.cc
 */

#include "events/logevent.h"

#include "level.h"
#include "gameproxy.h"
#include "util/basictypes.h"
#include "util/util.h"

#include <vector>
#include <cassert>
#include <iostream>
#include <string>
#include <utility>


namespace stmg
{

const int32_t LogEvent::MsgLog::s_nMaxLastBufferedEntries = 100;

void LogEvent::MsgLog::reset() noexcept
{
	m_nTotEntries = 0;
	m_nLastPointer = 0;
	m_aEntry.clear();
}
void LogEvent::MsgLog::addEntry(Entry&& oEntry) noexcept
{
	const int32_t nSize = static_cast<int32_t>(m_aEntry.size());
	if (nSize == s_nMaxLastBufferedEntries) {
		++m_nLastPointer;
		if (m_nLastPointer == nSize) {
			m_nLastPointer = 0;
		}
		m_aEntry[m_nLastPointer] = std::move(oEntry);
	} else {
		m_aEntry.emplace_back(std::move(oEntry));
		m_nLastPointer = nSize;
	}
	++m_nTotEntries;
}

const LogEvent::MsgLog::Entry& LogEvent::MsgLog::last() const noexcept
{
	return last(0);
}
const LogEvent::MsgLog::Entry& LogEvent::MsgLog::last(int32_t nBack) const noexcept
{
	assert(nBack >= 0);
	static Entry s_oEmptyEntry{};
	const int32_t nSize = static_cast<int32_t>(m_aEntry.size());
	if (nBack >= nSize) {
		return s_oEmptyEntry;
	}
	int32_t nIdx = m_nLastPointer - nBack;
	if (nIdx < 0) {
		nIdx += nSize;
	}
//std::cout << "LogEvent::MsgLog::last nBack=" << nBack << " m_nLastPointer=" << m_nLastPointer << "  nIdx=" << nIdx << " nSize=" << nSize << '\n';
	return m_aEntry[nIdx];
}
#ifndef NDEBUG
void LogEvent::MsgLog::dump() const noexcept
{
	const int32_t nSize = static_cast<int32_t>(m_aEntry.size());
	std::cout << "LogEvent::MsgLog::dump() tot entries: " << m_nTotEntries << "  buffered: " << nSize << '\n';
	for (int32_t nBack = nSize - 1; nBack >= 0; --nBack) {
		std::cout << " Entry: " << ((nBack > 0) ? "-" : "") << nBack << '\n';
		const Entry& oEntry = last(nBack);
		std::cout << "   level:       " << oEntry.m_nLevel << '\n';
		std::cout << "   game tick:   " << oEntry.m_nGameTick << '\n';
		std::cout << "   elapsed:     " << oEntry.m_nGameMillisec << '\n';
		std::cout << "   tag:         " << oEntry.m_nTag << '\n';
		std::cout << "   msg:         " << oEntry.m_nMsg << '\n';
		std::cout << "   value:       " << oEntry.m_nValue << '\n';
		std::cout << "   trig. addr.: " << oEntry.m_nTriggeringEventAddr << '\n';
	}
}
#endif //NDEBUG

LogEvent::LogEvent(Init&& oInit) noexcept
: Event(std::move(oInit))
, m_oData(std::move(oInit))
, m_bManyLevels(level().game().getTotLevels() > 1)
{
	assert(m_oData.m_nTag >= 0);
}

void LogEvent::reInit(Init&& oInit) noexcept
{
	Event::reInit(std::move(oInit));
	m_oData = std::move(oInit);
	m_bManyLevels = (level().game().getTotLevels() > 1);
	assert(m_oData.m_nTag >= 0);
}

void LogEvent::trigger(int32_t nMsg, int32_t nValue, Event* p0TriggeringEvent) noexcept
{
	auto& oLevel = level();
	auto& oGame = oLevel.game();

	MsgLog::Entry oEntry;
	oEntry.m_nLevel = oLevel.getLevel();
	oEntry.m_nGameTick = oGame.gameElapsed();
	oEntry.m_nGameMillisec = oGame.gameElapsedMillisec();
	oEntry.m_nTag = m_oData.m_nTag;
	oEntry.m_nMsg = nMsg;
	oEntry.m_nValue = nValue;
	oEntry.m_nTriggeringEventAddr = reinterpret_cast<int64_t>(p0TriggeringEvent);

	if (m_oData.m_bToStdOut) {
		std::string sElapsed;
		if (m_oData.m_bElapsedMinuteFormat) {
			int32_t nElapsed = oEntry.m_nGameMillisec;
			const int32_t nMillisec = nElapsed % 1000;
			nElapsed = nElapsed / 1000;
			const int32_t nSec = nElapsed % 60;
			nElapsed = nElapsed / 60;
			const int32_t nMin = nElapsed % 60;
			nElapsed = nElapsed / 60;
			const int32_t nHour = nElapsed;
			if (nHour > 0) {
				sElapsed += std::to_string(nHour) + "h";
			}
			std::string sMin = "0" + std::to_string(nMin);
			sMin = sMin.substr(sMin.size() - 2);
			sElapsed += sMin + ":";
			std::string sSec = "0" + std::to_string(nSec);
			sSec = sSec.substr(sSec.size() - 2);
			sElapsed += sSec + ":";
			std::string sMillisec = "00" + std::to_string(nMillisec);
			sMillisec = sMillisec.substr(sMillisec.size() - 3);
			sElapsed += sMillisec;
		} else {
			sElapsed = std::to_string(oEntry.m_nGameMillisec);
		}
		std::cout
			<< "MsgLog: ";
		if (m_bManyLevels) {
			std::cout
				<< " Level:" << oEntry.m_nLevel;
		}
		std::cout
			<< " GameTick:" << oEntry.m_nGameTick
			<< " Elapsed:" << sElapsed
			<< " Tag:" << oEntry.m_nTag
			<< " Msg:" << oEntry.m_nMsg;
		if (m_oData.m_bValueAsXY) {
			const NPoint oXY = Util::unpackPointFromInt32(oEntry.m_nValue);
			std::cout
				<< " Value:(" << oXY.m_nX << "," << oXY.m_nY << ")";
		} else {
			std::cout
				<< " Value:" << oEntry.m_nValue;
		}
		std::cout
			<< " TriggeringEventAdr:" << oEntry.m_nTriggeringEventAddr
			<< '\n';
	}
	msgLog().addEntry(std::move(oEntry));
}

} // namespace stmg
