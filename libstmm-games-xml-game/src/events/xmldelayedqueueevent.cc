/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xmldelayedqueueevent.cc
 */

#include "events/xmldelayedqueueevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/event.h>
#include <stmm-games/events/delayedqueueevent.h>

//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }

namespace stmg
{

static const std::string s_sEventDelayedQueueNodeName = "DelayedQueueEvent";
static const std::string s_sEventDelayedQueueIntervalTicksAttr = "intervalTicks";
static const std::string s_sEventDelayedQueueIntervalMillisecsAttr = "intervalMillisecs";
static const std::string s_sEventDelayedQueueMaxQueueSizeAttr = "maxQueueSize";

XmlDelayedQueueEventParser::XmlDelayedQueueEventParser()
: XmlEventParser(s_sEventDelayedQueueNodeName)
{
}
Event* XmlDelayedQueueEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventDelayedQueue(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlDelayedQueueEventParser::parseEventDelayedQueue(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlDelayedQueueEventParser::parseEventDelayedQueue" << '\n';
	DelayedQueueEvent::Init oDQInit;
	oCtx.addChecker(p0Element);
	parseEventBase(oCtx, p0Element, oDQInit);

	const auto oPairIntervalTicks = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventDelayedQueueIntervalTicksAttr);
	if (oPairIntervalTicks.first) {
		const std::string& sIntervalTicks = oPairIntervalTicks.second;
		oDQInit.m_nInitialIntervalTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventDelayedQueueIntervalTicksAttr, sIntervalTicks, false
															, true, 0, false, -1);
	}
	const auto oPairIntervalMillisecs = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventDelayedQueueIntervalMillisecsAttr);
	if (oPairIntervalMillisecs.first) {
		const std::string& sIntervalMillisecs = oPairIntervalMillisecs.second;
		oDQInit.m_nInitialIntervalMillisecs = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventDelayedQueueIntervalMillisecsAttr, sIntervalMillisecs, false
															, true, 0, false, -1);
	}
	const auto oPairMaxQueueSize = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventDelayedQueueMaxQueueSizeAttr);
	if (oPairMaxQueueSize.first) {
		const std::string& sMaxQueueSize = oPairMaxQueueSize.second;
		oDQInit.m_nMaxQueueSize = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventDelayedQueueMaxQueueSizeAttr, sMaxQueueSize, false
															, true, 1, false, -1);
	}

	oCtx.removeChecker(p0Element, true);
	auto refDelayedQueueEvent = std::make_unique<DelayedQueueEvent>(std::move(oDQInit));
	return refDelayedQueueEvent;
}
int32_t XmlDelayedQueueEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "QUEUE_VALUE") {
		nMsg = DelayedQueueEvent::MESSAGE_QUEUE_VALUE;
	} else if (sMsgName == "SET_INTERVAL_TICKS") {
		nMsg = DelayedQueueEvent::MESSAGE_SET_INTERVAL_TICKS;
	} else if (sMsgName == "SET_INTERVAL_MILLISECS") {
		nMsg = DelayedQueueEvent::MESSAGE_SET_INTERVAL_MILLISECS;
	} else if (sMsgName == "SET_INITIAL_INTERVAL") {
		nMsg = DelayedQueueEvent::MESSAGE_SET_INITIAL_INTERVAL;
	} else if (sMsgName == "EMPTY_QUEUE") {
		nMsg = DelayedQueueEvent::MESSAGE_EMPTY_QUEUE;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlDelayedQueueEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "VALUE") {
		nListenerGroup = DelayedQueueEvent::LISTENER_GROUP_VALUE;
	} else if (sListenerGroupName == "OVERFLOW") {
		nListenerGroup = DelayedQueueEvent::LISTENER_GROUP_OVERFLOW;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
