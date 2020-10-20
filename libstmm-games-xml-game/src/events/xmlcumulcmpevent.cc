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
 * File:   xmlcumulcmpevent.cc
 */

#include "events/xmlcumulcmpevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/conditionalctx.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/apppreferences.h>
#include <stmm-games/event.h>
#include <stmm-games/events/cumulcmpevent.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventCumulCmpNodeName = "CumulCmpEvent";
static const std::string s_sEventCumulCmpInitialLeftAttr = "initialLeft";
static const std::string s_sEventCumulCmpInitialRightAttr = "initialRight";
static const std::string s_sEventCumulCmpInitialRightAddTotTeamsAttr = "addTotTeamsToRight";
static const std::string s_sEventCumulCmpInitialRightAddTotTeammatesAttr = "addTotMatesOfTeamToRight";
static const std::string s_sEventCumulCmpOnCompare = "onCompare";
	static const std::string s_sEventCumulCmpOnCompareAttrUnchanged = "UNCHANGED";
	static const std::string s_sEventCumulCmpOnCompareAttrResetBoth = "RESET_BOTH";
	static const std::string s_sEventCumulCmpOnCompareAttrResetLeft = "RESET_LEFT";
	static const std::string s_sEventCumulCmpOnCompareAttrResetRight = "RESET_RIGHT";
	static const std::string s_sEventCumulCmpOnCompareAttrSetToInitialBoth = "SET_TO_INITIAL_BOTH";
	static const std::string s_sEventCumulCmpOnCompareAttrSetToInitialLeft = "SET_TO_INITIAL_LEFT";
	static const std::string s_sEventCumulCmpOnCompareAttrSetToInitialRight = "SET_TO_INITIAL_RIGHT";
	static const std::string s_sEventCumulCmpOnCompareAttrCopyLeftToRight = "COPY_LEFT_TO_RIGHT";
	static const std::string s_sEventCumulCmpOnCompareAttrCopyRightToLeft = "COPY_RIGHT_TO_LEFT";

XmlCumulCmpEventParser::XmlCumulCmpEventParser()
: XmlEventParser(s_sEventCumulCmpNodeName)
{
}

Event* XmlCumulCmpEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventCumulCmp(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlCumulCmpEventParser::parseEventCumulCmp(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::parseEventCumulCmp" << '\n';
	oCtx.addChecker(p0Element);
	CumulCmpEvent::Init oCCInit;
	parseEventBase(oCtx, p0Element, oCCInit);

	const auto oPairInitialLeft = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventCumulCmpInitialLeftAttr);
	if (oPairInitialLeft.first) {
		const std::string& sInitialLeft = oPairInitialLeft.second;
		oCCInit.m_nInitialLeft = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventCumulCmpInitialLeftAttr, sInitialLeft, false
																		, false, -1, false, -1);
	}
	const auto oPairInitialRight = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventCumulCmpInitialRightAttr);
	if (oPairInitialRight.first) {
		const std::string& sInitialRight = oPairInitialRight.second;
		oCCInit.m_nInitialRight = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventCumulCmpInitialRightAttr, sInitialRight, false
																		, false, -1, false, -1);
	}
	const auto oPairAddTotTeams = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventCumulCmpInitialRightAddTotTeamsAttr);
	if (oPairAddTotTeams.first) {
		const std::string& sAddTotTeams = oPairAddTotTeams.second;
		const bool bAddTotTeams = XmlUtil::strToBool(oCtx, p0Element, s_sEventCumulCmpInitialRightAddTotTeamsAttr, sAddTotTeams);
		if (bAddTotTeams) {
			oCCInit.m_nInitialRight += oCtx.appPreferences()->getTotTeams();
		}
	}

	const auto oPairAddTotTeammates = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventCumulCmpInitialRightAddTotTeammatesAttr);
	if (oPairAddTotTeammates.first) {
		const std::string& sTeam = oPairAddTotTeammates.second;
		const int32_t nTotTeams = oCtx.appPreferences()->getTotTeams();
		const int32_t nTeam = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventCumulCmpInitialRightAddTotTeammatesAttr, sTeam, false
																			, true, 0, true, nTotTeams - 1);
		const int32_t nTotTeammates = oCtx.appPreferences()->getTeam(nTeam)->getTotMates();
		oCCInit.m_nInitialRight += nTotTeammates;
	}

	static std::vector<char const *> s_aOnCompareEnumString{
		s_sEventCumulCmpOnCompareAttrUnchanged.c_str()
		, s_sEventCumulCmpOnCompareAttrResetBoth.c_str()
		, s_sEventCumulCmpOnCompareAttrResetLeft.c_str()
		, s_sEventCumulCmpOnCompareAttrResetRight.c_str()
		, s_sEventCumulCmpOnCompareAttrSetToInitialBoth.c_str()
		, s_sEventCumulCmpOnCompareAttrSetToInitialLeft.c_str()
		, s_sEventCumulCmpOnCompareAttrSetToInitialRight.c_str()
		, s_sEventCumulCmpOnCompareAttrCopyLeftToRight.c_str()
		, s_sEventCumulCmpOnCompareAttrCopyRightToLeft.c_str()
	};
	static const std::vector<CumulCmpEvent::ON_COMPARE_TYPE> s_aOnCompareEnumValue{
		CumulCmpEvent::ON_COMPARE_UNCHANGED
		, CumulCmpEvent::ON_COMPARE_RESET_BOTH
		, CumulCmpEvent::ON_COMPARE_RESET_LEFT
		, CumulCmpEvent::ON_COMPARE_RESET_RIGHT
		, CumulCmpEvent::ON_COMPARE_SET_TO_INITIAL_BOTH
		, CumulCmpEvent::ON_COMPARE_SET_TO_INITIAL_LEFT
		, CumulCmpEvent::ON_COMPARE_SET_TO_INITIAL_RIGHT
		, CumulCmpEvent::ON_COMPARE_COPY_LEFT_TO_RIGHT
		, CumulCmpEvent::ON_COMPARE_COPY_RIGHT_TO_LEFT
	};
	//
	const int32_t nIdxOnCompare = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventCumulCmpOnCompare, s_aOnCompareEnumString);
	if (nIdxOnCompare >= 0) {
		oCCInit.m_eOnCompareType = s_aOnCompareEnumValue[nIdxOnCompare];
	}

	oCtx.removeChecker(p0Element, true);
	auto refCumulCmpEvent = std::make_unique<CumulCmpEvent>(std::move(oCCInit));
	return refCumulCmpEvent;
}
int32_t XmlCumulCmpEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "CUMUL_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_COMPARE;
	} else if (sMsgName == "CUMUL_SET") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET;
	} else if (sMsgName == "CUMUL_SET_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_LEFT;
	} else if (sMsgName == "CUMUL_SET_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_RIGHT;
	} else if (sMsgName == "CUMUL_SET_LEFT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_LEFT_COMPARE;
	} else if (sMsgName == "CUMUL_SET_RIGHT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_RIGHT_COMPARE;
	} else if (sMsgName == "CUMUL_ADD_TO") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_ADD_TO;
	} else if (sMsgName == "CUMUL_ADD_TO_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_ADD_TO_LEFT;
	} else if (sMsgName == "CUMUL_ADD_TO_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_ADD_TO_RIGHT;
	} else if (sMsgName == "CUMUL_ADD_TO_LEFT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_ADD_TO_LEFT_COMPARE;
	} else if (sMsgName == "CUMUL_ADD_TO_RIGHT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_ADD_TO_RIGHT_COMPARE;
	} else if (sMsgName == "CUMUL_MULT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_MULT;
	} else if (sMsgName == "CUMUL_MULT_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_MULT_LEFT;
	} else if (sMsgName == "CUMUL_MULT_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_MULT_RIGHT;
	} else if (sMsgName == "CUMUL_MULT_LEFT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_MULT_LEFT_COMPARE;
	} else if (sMsgName == "CUMUL_MULT_RIGHT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_MULT_RIGHT_COMPARE;
	} else if (sMsgName == "CUMUL_DIV") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_DIV;
	} else if (sMsgName == "CUMUL_DIV_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_DIV_LEFT;
	} else if (sMsgName == "CUMUL_DIV_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_DIV_RIGHT;
	} else if (sMsgName == "CUMUL_DIV_LEFT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_DIV_LEFT_COMPARE;
	} else if (sMsgName == "CUMUL_DIV_RIGHT_COMPARE") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_DIV_RIGHT_COMPARE;
	} else if (sMsgName == "CUMUL_RESET") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_RESET;
	} else if (sMsgName == "CUMUL_RESET_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_RESET_LEFT;
	} else if (sMsgName == "CUMUL_RESET_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_RESET_RIGHT;
	} else if (sMsgName == "CUMUL_SET_TO_INITIAL") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_TO_INITIAL;
	} else if (sMsgName == "CUMUL_SET_TO_INITIAL_LEFT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_TO_INITIAL_LEFT;
	} else if (sMsgName == "CUMUL_SET_TO_INITIAL_RIGHT") {
		nMsg = CumulCmpEvent::MESSAGE_CUMUL_SET_TO_INITIAL_RIGHT;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlCumulCmpEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
															, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "COMPARED") {
		nListenerGroup = CumulCmpEvent::LISTENER_GROUP_COMPARED;
	} else if (sListenerGroupName == "COMPARED_EQUAL") {
		nListenerGroup = CumulCmpEvent::LISTENER_GROUP_COMPARED_EQUAL;
	} else if (sListenerGroupName == "COMPARED_NOT_EQUAL") {
		nListenerGroup = CumulCmpEvent::LISTENER_GROUP_COMPARED_NOT_EQUAL;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
