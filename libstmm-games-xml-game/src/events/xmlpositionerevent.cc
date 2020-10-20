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
 * File:   xmlpositionerevent.cc
 */

#include "events/xmlpositionerevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlutil/xmlbasicparser.h>

#include <stmm-games/level.h>
#include <stmm-games/event.h>
#include <stmm-games/events/positionerevent.h>
#include <stmm-games/levelshow.h>
#include <stmm-games/util/basictypes.h>

//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventPositionerNodeName = "PositionerEvent";
static const std::string s_sEventPositionerTrackAreaXAttr = "areaX";
static const std::string s_sEventPositionerTrackAreaYAttr = "areaY";
static const std::string s_sEventPositionerTrackAreaWAttr = "areaW";
static const std::string s_sEventPositionerTrackAreaHAttr = "areaH";
static const std::string s_sEventPositionerCheckEachTicksAttr = "checkEachTicks";
static const std::string s_sEventPositionerTransitionTicksAttr = "transitionTicks";


XmlPositionerEventParser::XmlPositionerEventParser()
: XmlEventParser(s_sEventPositionerNodeName)
{
}
Event* XmlPositionerEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventPositioner(oCtx, p0Element), p0Element);
}

unique_ptr<Event> XmlPositionerEventParser::parseEventPositioner(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlPositionerEventParser::parseEventPositioner" << '\n';
	oCtx.addChecker(p0Element);
	PositionerEvent::Init oInit;
	parseEventBase(oCtx, p0Element, oInit);

	Level& oLevel = oCtx.level();
	//const bool bATIOL = oLevel.game().isAllTeamsInOneLevel();
	const bool bSubshows = oLevel.subshowMode();
//std::cout << "XmlPositionerEventParser::parseEventPositioner: bSubshows = " << bSubshows << '\n';
	NSize oParentSize;
	if (bSubshows) {
		oParentSize.m_nW = oLevel.subshowGet(0).getW();
		oParentSize.m_nH = oLevel.subshowGet(0).getH();
//std::cout << "XmlPositionerEventParser::parseEventPositioner: oParentSize.m_nW = " << oParentSize.m_nW << '\n';
	} else {
		oParentSize.m_nW = oLevel.showGet().getW();
		oParentSize.m_nH = oLevel.showGet().getH();
	}
	XmlBasicParser oXmlBasicParser(getXmlConditionalParser());
	oInit.m_oTrackingRect = oXmlBasicParser.parseNRect(oCtx, p0Element
												, s_sEventPositionerTrackAreaXAttr, s_sEventPositionerTrackAreaYAttr, s_sEventPositionerTrackAreaWAttr, s_sEventPositionerTrackAreaHAttr
												, true, NRect{0, 0, oParentSize.m_nW, oParentSize.m_nH}
												, NSize{1, 1});

	const auto oPairCheckEachTicks = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventPositionerCheckEachTicksAttr);
	if (oPairCheckEachTicks.first) {
		const std::string& sCheckEachTicks = oPairCheckEachTicks.second;
		oInit.m_nCheckEachTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventPositionerCheckEachTicksAttr, sCheckEachTicks, false
																, true, 1, false, -1);
	}
	const auto oPairTransitionTicks =  getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventPositionerTransitionTicksAttr);
	if (oPairTransitionTicks.first) {
		const std::string& sTransitionTicks = oPairTransitionTicks.second;
		oInit.m_nTransitionTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventPositionerTransitionTicksAttr, sTransitionTicks, false
																, true, 1, false, -1);
	}
	oCtx.removeChecker(p0Element, true);
	return std::make_unique<PositionerEvent>(std::move(oInit));
}
int32_t XmlPositionerEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
																	, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "PAUSE_TRACKING") {
		nMsg = PositionerEvent::MESSAGE_PAUSE_TRACKING;
	} else if (sMsgName == "RESUME_TRACKING") {
		nMsg = PositionerEvent::MESSAGE_RESUME_TRACKING;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}

} // namespace stmg
