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
 * File:   xmlscrollerevent.cc
 */

#include "events/xmlscrollerevent.h"

#include "gamectx.h"
#include "xmlutile/xmlnewrowsparser.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games/util/util.h>
#include <stmm-games/utile/querytileremoval.h>
#include <stmm-games/utile/tileselector.h>
#include <stmm-games/event.h>
#include <stmm-games/level.h>
#include <stmm-games/events/scrollerevent.h>
#include <stmm-games/utile/newrows.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <memory>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

const std::string XmlScrollerEventParser::s_sEventScrollerNodeName = "ScrollerEvent";
static const std::string s_sEventScrollerSlicesAttr = "slices";
static const std::string s_sEventScrollerKeepTopVisibleAttr = "keepTopVisible";
static const std::string s_sEventScrollerTopNotEmptyWaitTicksAttr = "topNotEmptyWaitTicks";
static const std::string s_sEventScrollerTopNotEmptyWaitMillisecAttr = "topNotEmptyWaitMillisec";
static const std::string s_sEventScrollerNewRowsNodeName = "NewRows";
static const std::string s_sEventScrollerNewRowCheckerNodeName = "NewRowChecker";
static const std::string s_sEventScrollerNewRowCheckerTriesAttr = "tries";
static const std::string s_sEventScrollerNewRowCheckerRemoverNodeName = "Remover";
static const std::string s_sEventScrollerNewRowCheckerRemoverFromXAttr = "fromX";
static const std::string s_sEventScrollerNewRowCheckerRemoverToXAttr = "toX";
static const std::string s_sEventScrollerNewRowCheckerInhibitorNodeName = "Inhibitor";

static const std::string s_sEventScrollerInhibitStartMessage = "INHIBIT_START_";
static const std::string s_sEventScrollerInhibitStopMessage = "INHIBIT_STOP_";

XmlScrollerEventParser::XmlScrollerEventParser()
: XmlEventParser(s_sEventScrollerNodeName)
{
}

Event* XmlScrollerEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return parseEventScroller(oCtx, p0Element);
}

Event* XmlScrollerEventParser::parseEventScroller(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "parseEventScroller" << '\n';
	oCtx.addChecker(p0Element);
	ScrollerEvent::Init oInit;
	parseEventBase(oCtx, p0Element, oInit);
	;
	oInit.m_nRepeat = parseEventAttrRepeat(oCtx, p0Element);
	oInit.m_nStep = parseEventAttrStep(oCtx, p0Element);
	;
	const auto oPairSlices = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventScrollerSlicesAttr);
	if (oPairSlices.first) {
		const std::string& sSlices = oPairSlices.second;
		oInit.m_nSlices = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventScrollerSlicesAttr, sSlices, false, true, 0, false, -1);
	}
	;
	const auto oPairKeepTopVisible = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventScrollerKeepTopVisibleAttr);
	if (oPairKeepTopVisible.first) {
		const std::string& sKeepTopVisible = oPairKeepTopVisible.second;
		oInit.m_bKeepTopVisible = XmlUtil::strToBool(oCtx, p0Element, s_sEventScrollerKeepTopVisibleAttr, sKeepTopVisible);
	}
	;
	const auto oPairTopNotEmptyWaitTicks = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventScrollerTopNotEmptyWaitTicksAttr);
	if (oPairTopNotEmptyWaitTicks.first) {
		const std::string& sTopNotEmptyWaitTicks = oPairTopNotEmptyWaitTicks.second;
		oInit.m_nTopNotEmptyWaitTicks = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventScrollerTopNotEmptyWaitTicksAttr
																			, sTopNotEmptyWaitTicks, false, true, 0, false, -1);
	}
	;
	const auto oPairTopNotEmptyWaitMillisec = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventScrollerTopNotEmptyWaitMillisecAttr);
	if (oPairTopNotEmptyWaitMillisec.first) {
		const std::string& sTopNotEmptyWaitMillisec = oPairTopNotEmptyWaitMillisec.second;
		oInit.m_nTopNotEmptyWaitMillisec = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventScrollerTopNotEmptyWaitMillisecAttr
																				, sTopNotEmptyWaitMillisec, false, true, 0, false, -1);
	}
	;
	XmlNewRowsParser oXmlNewRows(getXmlConditionalParser(), getXmlTraitsParser());
	const xmlpp::Element* p0NewRowsElement = getXmlConditionalParser().parseUniqueElement(oCtx, p0Element, s_sEventScrollerNewRowsNodeName, true);

	oInit.m_refNewRows = std::make_unique<NewRows>(oXmlNewRows.parseNewRows(oCtx, p0NewRowsElement));
	const xmlpp::Element* p0Checker = getXmlConditionalParser().parseUniqueElement(oCtx, p0Element, s_sEventScrollerNewRowCheckerNodeName, false);
	if (p0Checker != nullptr) {
		parseNewRowChecker(oCtx, p0Checker, oInit);
	}

	Level& oLevel = oCtx.level();
	if (oLevel.showGet().getH() >= oLevel.boardHeight()) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, s_sEventScrollerNodeName + " requires Show height to be smaller than board height!");
	}

	oCtx.removeChecker(p0Element, true);
	auto refScrollerEvent = std::make_unique<ScrollerEvent>(std::move(oInit));
	return integrateAndAdd(oCtx, std::move(refScrollerEvent), p0Element);
}
void XmlScrollerEventParser::parseNewRowChecker(GameCtx& oCtx, const xmlpp::Element* p0Element, ScrollerEvent::Init& oInit)
{
	oCtx.addChecker(p0Element);

	const auto oPairCheckNewRowTries = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventScrollerNewRowCheckerTriesAttr);
	if (oPairCheckNewRowTries.first) {
		const std::string& sCheckNewRowTries = oPairCheckNewRowTries.second;
		oInit.m_nCheckNewRowTries = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventScrollerNewRowCheckerTriesAttr
																			, sCheckNewRowTries, false, true, 1, true, 1000);
	}
	;
	getXmlConditionalParser().visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0EventElement)
	{
		const std::string sElementName = p0EventElement->get_name();
		if (sElementName == s_sEventScrollerNewRowCheckerInhibitorNodeName) {
			parseInhibitor(oCtx, p0EventElement, oInit);
		} else if (sElementName == s_sEventScrollerNewRowCheckerRemoverNodeName) {
			parseRemover(oCtx, p0EventElement, oInit);
		}
	});
	oCtx.removeChecker(p0Element, false, true);
}
void XmlScrollerEventParser::parseRemover(GameCtx& oCtx, const xmlpp::Element* p0Element, ScrollerEvent::Init& oInit)
{
	ScrollerEvent::NewRowCheckRemover oRemover;
	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element, "</>"
															, s_sEventScrollerNewRowCheckerRemoverFromXAttr, s_sEventScrollerNewRowCheckerRemoverToXAttr
															, false, true, 0, true, oCtx.level().boardWidth(), oRemover.m_nFrom, oRemover.m_nTo);
	;
	QueryTileRemoval* p0QueryTileRemoval = nullptr;
	getXmlConditionalParser().visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0EventElement)
	{
		if (p0QueryTileRemoval != nullptr) {
			throw XmlCommonErrors::error(oCtx, p0EventElement, Util::s_sEmptyString, "At most one QueryTileRemoval implementing event expected!");
		}
		Event* p0Event = parseChildEvent(oCtx, p0EventElement);
		assert(p0Event != nullptr);
		p0QueryTileRemoval = dynamic_cast<QueryTileRemoval*>(p0Event);
		if (p0QueryTileRemoval == nullptr) {
			throw XmlCommonErrors::error(oCtx, p0EventElement, Util::s_sEmptyString, "Event implementing interface QueryTileRemoval expected!");
		}
	});
	oRemover.m_p0TileRemover = p0QueryTileRemoval;
	;
	oInit.m_aRemovers.push_back(std::move(oRemover));
}
void XmlScrollerEventParser::parseInhibitor(GameCtx& oCtx, const xmlpp::Element* p0Element, ScrollerEvent::Init& oInit)
{
	oInit.m_aInhibitors.push_back(getXmlTraitsParser().parseTileSelectorAnd(oCtx, p0Element));
}

int32_t XmlScrollerEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "PUSH_ROW") {
		nMsg = ScrollerEvent::MESSAGE_PUSH_ROW;
	} else if (sMsgName == "PAUSE_TICKS") {
		nMsg = ScrollerEvent::MESSAGE_PAUSE_TICKS;
	} else if (sMsgName == "PAUSE_MILLISEC") {
		nMsg = ScrollerEvent::MESSAGE_PAUSE_MILLISEC;
	} else if (sMsgName == "SET_SLICES") {
		nMsg = ScrollerEvent::MESSAGE_SET_SLICES;
	} else if (sMsgName == "SET_NEW_ROW_GEN") {
		nMsg = ScrollerEvent::MESSAGE_SET_NEW_ROW_GEN;
	} else if (sMsgName == "NEXT_NEW_ROW_GEN") {
		nMsg = ScrollerEvent::MESSAGE_NEXT_NEW_ROW_GEN;
	} else if (sMsgName == "PREV_NEW_ROW_GEN") {
		nMsg = ScrollerEvent::MESSAGE_PREV_NEW_ROW_GEN;
	} else {
		const auto nStartMsgSize = s_sEventScrollerInhibitStartMessage.size();
		const auto nStopMsgSize = s_sEventScrollerInhibitStopMessage.size();
		const int32_t nMaxInhibitors = ScrollerEvent::MESSAGE_INHIBIT_STOP_INDEX_BASE - ScrollerEvent::MESSAGE_INHIBIT_START_INDEX_BASE;
		if (sMsgName.substr(0, nStartMsgSize) == s_sEventScrollerInhibitStartMessage) {
			std::string sNr = sMsgName.substr(nStartMsgSize);
			const int32_t nNr = XmlUtil::strToNumber(oCtx, p0Element, sAttr, sNr, false, true, 0, true, nMaxInhibitors - 1);
			nMsg = ScrollerEvent::MESSAGE_INHIBIT_START_INDEX_BASE + nNr;
		} else if (sMsgName.substr(0, nStopMsgSize) == s_sEventScrollerInhibitStopMessage) {
			std::string sNr = sMsgName.substr(nStopMsgSize);
			const int32_t nNr = XmlUtil::strToNumber(oCtx, p0Element, sAttr, sNr, false, true, 0, true, nMaxInhibitors - 1);
			nMsg = ScrollerEvent::MESSAGE_INHIBIT_STOP_INDEX_BASE + nNr;
		} else {
			nMsg = XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
		}
	}
	return nMsg;
}
int32_t XmlScrollerEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "PUSHED") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_PUSHED;
	} else if (sListenerGroupName == "TOP_COLUMN") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_TOP_COLUMN;
	} else if (sListenerGroupName == "TOP_DESTROYED") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_TOP_DESTROYED;
	} else if (sListenerGroupName == "TOP_NOT_EMPTY_START") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_TOP_NOT_EMPTY_START;
	} else if (sListenerGroupName == "TOP_NOT_EMPTY_END") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_TOP_NOT_EMPTY_END;
	} else if (sListenerGroupName == "TOP_NOT_EMPTY_TICK") {
		nListenerGroup = ScrollerEvent::LISTENER_GROUP_TOP_NOT_EMPTY_TICK;
	} else {
		nListenerGroup = XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
