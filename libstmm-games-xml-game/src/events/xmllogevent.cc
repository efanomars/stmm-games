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
 * File:   xmllogevent.cc
 */

#include "events/xmllogevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/event.h>
#include <stmm-games/events/logevent.h>

//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventLogNodeName = "LogEvent";
static const std::string s_sEventLogToStdOutAttr = "toStdOut";
static const std::string s_sEventLogElapsedMinuteFormatAttr = "elapsedMinuteFormat";
static const std::string s_sEventLogValueAsXYAttr = "valueAsXY";
static const std::string s_sEventLogTagAttr = "tag";

XmlLogEventParser::XmlLogEventParser()
: XmlEventParser(s_sEventLogNodeName)
{
}

Event* XmlLogEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventLog(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlLogEventParser::parseEventLog(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlLogEventParser::parseEventLog" << '\n';
	oCtx.addChecker(p0Element);
	LogEvent::Init oLInit;

	const auto oPairLogToStdOut = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventLogToStdOutAttr);
	if (oPairLogToStdOut.first) {
		oLInit.m_bToStdOut = XmlUtil::strToBool(oCtx, p0Element, s_sEventLogToStdOutAttr, oPairLogToStdOut.second);
	}

	const auto oPairElapsedMinuteFormat = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventLogElapsedMinuteFormatAttr);
	if (oPairElapsedMinuteFormat.first) {
		oLInit.m_bElapsedMinuteFormat = XmlUtil::strToBool(oCtx, p0Element, s_sEventLogElapsedMinuteFormatAttr, oPairElapsedMinuteFormat.second);
	}

	const auto oPairValueAsXY = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventLogValueAsXYAttr);
	if (oPairValueAsXY.first) {
		oLInit.m_bValueAsXY = XmlUtil::strToBool(oCtx, p0Element, s_sEventLogValueAsXYAttr, oPairValueAsXY.second);
	}

	const auto oPairTag = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventLogTagAttr);
	if (oPairTag.first) {
		const std::string& sTag = oPairTag.second;
		oLInit.m_nTag = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventLogTagAttr, sTag, false
															, false, -1, false, -1);
	}
	parseEventBase(oCtx, p0Element, oLInit);
	oCtx.removeChecker(p0Element, true);
	auto refLogEvent = std::make_unique<LogEvent>(std::move(oLInit));
	return refLogEvent;
}


} // namespace stmg
