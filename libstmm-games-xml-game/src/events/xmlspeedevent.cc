/*
 * File:   xmlspeedevent.cc
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

#include "events/xmlspeedevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

#include <stmm-games/event.h>
#include <stmm-games/events/speedevent.h>
#include <stmm-games/util/util.h>

namespace stmg
{

static const std::string s_sEventSpeedNodeName = "SpeedEvent";
static const std::string s_sEventSpeedChangeNodeName = "Change";
static const std::string s_sEventSpeedChangeSetNodeName = "Set";
static const std::string s_sEventSpeedChangeIncNodeName = "Inc";
static const std::string s_sEventSpeedChangeMultNodeName = "Mult";
static const std::string s_sEventSpeedChangeAnyIntervalAttr = "interval";
static const std::string s_sEventSpeedChangeAnyFallTicksAttr = "fallEachTicks";

XmlSpeedEventParser::XmlSpeedEventParser()
: XmlEventParser(s_sEventSpeedNodeName)
{
}
Event* XmlSpeedEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventSpeed(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlSpeedEventParser::parseEventSpeed(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlSpeedEventParser::parseEventSpeed" << '\n';
	oCtx.addChecker(p0Element);
	SpeedEvent::Init oSInit;
	parseEventBase(oCtx, p0Element, oSInit);

	bool bAddedOne = false;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sEventSpeedChangeNodeName, [&](const xmlpp::Element* p0Change)
	{
		oSInit.m_aSpeedChanges.push_back(parseEventSpeedChange(oCtx, p0Change));
		bAddedOne = true;
	});

	if (!bAddedOne) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sEventSpeedChangeNodeName);
	}
	oCtx.removeChecker(p0Element, true);
	auto refSpeedEvent = std::make_unique<SpeedEvent>(std::move(oSInit));
	return refSpeedEvent;
}

SpeedEvent::SpeedChange XmlSpeedEventParser::parseEventSpeedChange(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlSpeedEventParser::parseEventSpeedChange" << '\n';
	oCtx.addChecker(p0Element);
	const int32_t nRepeat = parseEventAttrRepeat(oCtx, p0Element);

	SpeedEvent::SPEED_EVENT_CHANGE_TYPE eIntervalChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE;
	int32_t nIntervalChange = -77777;
	SpeedEvent::SPEED_EVENT_CHANGE_TYPE eFallTicksChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE;
	int32_t nFallTicksChange = -88888;

	getXmlConditionalParser().visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0Changer)
	{
		const std::string sChangerName = p0Changer->get_name();
		bool bInterval = false;
		bool bFallTicks = false;
		int32_t nInterval;
		int32_t nFallTicks;
		SpeedEvent::SPEED_EVENT_CHANGE_TYPE eChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE;
		if (sChangerName == s_sEventSpeedChangeSetNodeName) {
			parseEventSpeedChanger(oCtx, p0Changer, false, bInterval, nInterval, bFallTicks, nFallTicks);
			eChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_SET;
		} else if (sChangerName == s_sEventSpeedChangeIncNodeName) {
			parseEventSpeedChanger(oCtx, p0Changer, false, bInterval, nInterval, bFallTicks, nFallTicks);
			eChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_INC;
		} else if (sChangerName == s_sEventSpeedChangeMultNodeName) {
			parseEventSpeedChanger(oCtx, p0Changer, true, bInterval, nInterval, bFallTicks, nFallTicks);
			eChange = SpeedEvent::SPEED_EVENT_CHANGE_TYPE_MULT_PERC;
		} else {
			throw XmlCommonErrors::error(oCtx, p0Changer, Util::s_sEmptyString, Util::stringCompose(
											"Element %1 is not a changer", sChangerName));
		}
		if (eChange != SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE) {
			if (bInterval) {
				if (eIntervalChange != SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE) {
					throw XmlCommonErrors::error(oCtx, p0Changer, s_sEventSpeedChangeAnyIntervalAttr, Util::stringCompose(
												"Attribute '%1' changed more than once!", s_sEventSpeedChangeAnyIntervalAttr));
				}
				eIntervalChange = eChange;
				nIntervalChange = nInterval;
			}
			if (bFallTicks) {
				if (eFallTicksChange != SpeedEvent::SPEED_EVENT_CHANGE_TYPE_NONE) {
					throw XmlCommonErrors::error(oCtx, p0Changer, s_sEventSpeedChangeAnyFallTicksAttr, Util::stringCompose(
												"Attribute '%1' changed more than once!", s_sEventSpeedChangeAnyFallTicksAttr));
				}
				eFallTicksChange = eChange;
				nFallTicksChange = nFallTicks;
			}
		}
	});

	SpeedEvent::SpeedChange oChange;
	oChange.m_nRepeat = nRepeat;
	oChange.m_eIntervalChangeType = eIntervalChange;
	oChange.m_nIntervalChange = nIntervalChange;
	oChange.m_eFallTicksChangeType = eFallTicksChange;
	oChange.m_nFallTicksChange = nFallTicksChange;
//std::cout << "parseEventSpeedChange repeat=" << nRepeat << '\n';
//std::cout << "parseEventSpeedChange eIntervalChange=" << eIntervalChange << '\n';
//std::cout << "parseEventSpeedChange nIntervalChange=" << nIntervalChange << '\n';
//std::cout << "parseEventSpeedChange eFallTicksChange=" << eFallTicksChange << '\n';
//std::cout << "parseEventSpeedChange nFallTicksChange=" << nFallTicksChange << '\n';

	// Child elements checked in visitElementChildren
	oCtx.removeChecker(p0Element, false, true);
	return oChange;
}
void XmlSpeedEventParser::parseEventSpeedChanger(GameCtx& oCtx, const xmlpp::Element* p0Element, bool bPerc
												, bool& bInterval, int32_t& nIntervalChange, bool& bFallTicks, int32_t& nFallTicksChange)
{
//std::cout << "XmlSpeedEventParser::parseEventSpeedChanger" << '\n';
	oCtx.addChecker(p0Element);
	const auto oPairInterval = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSpeedChangeAnyIntervalAttr);
	bInterval = oPairInterval.first;
	if (bInterval) {
		const std::string& sInterval = oPairInterval.second;
		if (bPerc) {
			const float fIntervalChange = XmlUtil::strToNumber<float>(oCtx, p0Element, s_sEventSpeedChangeAnyIntervalAttr, sInterval, true
																			, false, -1, false, -1);
			nIntervalChange = fIntervalChange * 100;
		} else {
			nIntervalChange = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventSpeedChangeAnyIntervalAttr, sInterval, false
																	, false, -1, false, -1);
		}
	}
	const auto oPairFallTicks = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSpeedChangeAnyFallTicksAttr);
	bFallTicks = oPairFallTicks.first;
	if (bFallTicks) {
		const std::string& sFallTicks = oPairFallTicks.second;
		if (bPerc) {
			const float fFallTicksChange = XmlUtil::strToNumber<float>(oCtx, p0Element, s_sEventSpeedChangeAnyFallTicksAttr, sFallTicks, true
																				, false, -1, false, -1);
			nFallTicksChange = fFallTicksChange * 100;
		} else {
			nFallTicksChange = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventSpeedChangeAnyFallTicksAttr, sFallTicks, false
																	, false, -1, false, -1);
		}
	}
	if ((!bInterval) && !bFallTicks) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sEventSpeedChangeAnyIntervalAttr, s_sEventSpeedChangeAnyFallTicksAttr);
	}
	oCtx.removeChecker(p0Element, true);
}
int32_t XmlSpeedEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "CHANGED") {
		nListenerGroup = SpeedEvent::LISTENER_GROUP_CHANGED;
	} else if (sListenerGroupName == "NOT_CHANGED") {
		nListenerGroup = SpeedEvent::LISTENER_GROUP_NOT_CHANGED;
	} else if (sListenerGroupName == "CHANGED_INTERVAL") {
		nListenerGroup = SpeedEvent::LISTENER_GROUP_CHANGED_INTERVAL;
	} else if (sListenerGroupName == "CHANGED_FALL_EACH_TICKS") {
		nListenerGroup = SpeedEvent::LISTENER_GROUP_CHANGED_FALL_EACH_TICKS;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
