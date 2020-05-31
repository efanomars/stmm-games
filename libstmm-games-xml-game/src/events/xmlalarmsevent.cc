/*
 * File:   xmlalarmsevent.cc
 *
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

#include "events/xmlalarmsevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/event.h>
#include <stmm-games/events/alarmsevent.h>
#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }

namespace stmg
{

static const std::string s_sEventAlarmsNodeName = "AlarmsEvent";
static const std::string s_sEventAlarmsTicksSetTicksNodeName = "SetTicks";
static const std::string s_sEventAlarmsTicksIncTicksNodeName = "IncTicks";
static const std::string s_sEventAlarmsTicksSetMillisecNodeName = "SetMillisec";
static const std::string s_sEventAlarmsTicksIncMillisecNodeName = "IncMillisec";
static const std::string s_sEventAlarmsTicksMultNodeName = "Mult";
static const std::string s_sEventAlarmsTicksAnyChangeAttr = "value";

XmlAlarmsEventParser::XmlAlarmsEventParser()
: XmlEventParser(s_sEventAlarmsNodeName)
{
}
Event* XmlAlarmsEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventAlarms(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlAlarmsEventParser::parseEventAlarms(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlAlarmsEventParser::parseEventAlarms" << '\n';
	AlarmsEvent::Init oAInit;
	oCtx.addChecker(p0Element);
	parseEventBase(oCtx, p0Element, oAInit);

	getXmlConditionalParser().visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Changer)
	{
		const std::string sChangerName = p0Changer->get_name();
		int32_t nValue;
		int32_t nRepeat;
		AlarmsEvent::ALARMS_EVENT_TYPE eChange = AlarmsEvent::ALARMS_EVENT_INVALID;
		if (sChangerName == s_sEventAlarmsTicksSetTicksNodeName) {
			parseEventAlarmsChanger(oCtx, p0Changer, false, nValue, nRepeat);
			eChange = AlarmsEvent::ALARMS_EVENT_SET_TICKS;
		} else if (sChangerName == s_sEventAlarmsTicksIncTicksNodeName) {
			parseEventAlarmsChanger(oCtx, p0Changer, false, nValue, nRepeat);
			eChange = AlarmsEvent::ALARMS_EVENT_INC_TICKS;
		} else if (sChangerName == s_sEventAlarmsTicksSetMillisecNodeName) {
			parseEventAlarmsChanger(oCtx, p0Changer, false, nValue, nRepeat);
			eChange = AlarmsEvent::ALARMS_EVENT_SET_MILLISEC;
		} else if (sChangerName == s_sEventAlarmsTicksIncMillisecNodeName) {
			parseEventAlarmsChanger(oCtx, p0Changer, false, nValue, nRepeat);
			eChange = AlarmsEvent::ALARMS_EVENT_INC_MILLISEC;
		} else if (sChangerName == s_sEventAlarmsTicksMultNodeName) {
			parseEventAlarmsChanger(oCtx, p0Changer, true, nValue, nRepeat);
			eChange = AlarmsEvent::ALARMS_EVENT_MULT_PERC;
		} else if (isReservedChildElementOfEvent(sChangerName)) {
			//
		} else {
			throw XmlCommonErrors::error(oCtx, p0Changer, Util::s_sEmptyString, Util::stringCompose(
											"Element %1 is not a alarms changer", sChangerName));
		}
		if (eChange != AlarmsEvent::ALARMS_EVENT_INVALID) {
			AlarmsEvent::AlarmTimeout oChange;
			oChange.m_nRepeat = nRepeat;
			oChange.m_eAlarmTimeoutType = eChange;
			oChange.m_nChange = nValue;
			oAInit.m_aAlarmTimeouts.push_back(std::move(oChange));
		}
	});
	oCtx.removeChecker(p0Element, false, true);
	auto refAlarmsEvent = std::make_unique<AlarmsEvent>(std::move(oAInit));
	return refAlarmsEvent;
}
void XmlAlarmsEventParser::parseEventAlarmsChanger(GameCtx& oCtx, const xmlpp::Element* p0Element, bool bPerc, int32_t& nValue, int32_t& nRepeat)
{
//std::cout << "XmlAlarmsEventParser::parseEventAlarmsChanger" << '\n';
	oCtx.addChecker(p0Element);
	const auto oPairValue = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventAlarmsTicksAnyChangeAttr);
	if (!oPairValue.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sEventAlarmsTicksAnyChangeAttr);
	}
	const std::string& sValue = oPairValue.second;
	if (bPerc) {
		const float fValueChange = XmlUtil::strToNumber<float>(oCtx, p0Element, s_sEventAlarmsTicksAnyChangeAttr, sValue, true
													, false, -1, false, -1);
		nValue = fValueChange * 100;
	} else {
		nValue = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventAlarmsTicksAnyChangeAttr, sValue, false
									, false, -1, false, -1);
	}

	nRepeat = parseEventAttrRepeat(oCtx, p0Element);
	oCtx.removeChecker(p0Element, true);
}
int32_t XmlAlarmsEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "ALARMS_NEXT") {
		nMsg = AlarmsEvent::MESSAGE_ALARMS_NEXT;
	} else if (sMsgName == "ALARMS_FINISH") {
		nMsg = AlarmsEvent::MESSAGE_ALARMS_FINISH;
	} else if (sMsgName == "ALARMS_RESTART") {
		nMsg = AlarmsEvent::MESSAGE_ALARMS_RESTART;
	} else if (sMsgName == "ALARMS_REDO_CURRENT") {
		nMsg = AlarmsEvent::MESSAGE_ALARMS_REDO_CURRENT;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlAlarmsEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "TIMEOUT") {
		nListenerGroup = AlarmsEvent::LISTENER_GROUP_TIMEOUT;
	} else if (sListenerGroupName == "TIMEOUT_ACTIVE") {
		nListenerGroup = AlarmsEvent::LISTENER_GROUP_TIMEOUT_ACTIVE;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
