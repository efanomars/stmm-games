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
 * File:   xmlvariableevent.cc
 */

#include "events/xmlvariableevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/event.h>
#include <stmm-games/events/variableevent.h>
#include <stmm-games/ownertype.h>

#include <cassert>
//#include <iostream>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventVariableNodeName = "VariableEvent";
static const std::string s_sEventVariableNameAttr = "name";
static const std::string s_sEventVariableDefaultIncByAttr = "defaultIncBy";

XmlVariableEventParser::XmlVariableEventParser()
: XmlEventParser(s_sEventVariableNodeName)
{
}

Event* XmlVariableEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventVariable(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlVariableEventParser::parseEventVariable(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlVariableEventParser::parseEventVariable" << '\n';
	oCtx.addChecker(p0Element);
	VariableEvent::Init oVInit;
	parseEventBase(oCtx, p0Element, oVInit);
	//
	const auto oPairName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventVariableNameAttr);
	if (!oPairName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sEventVariableNameAttr);
	}
	const std::string& sName = oPairName.second;
	if (sName.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventVariableNameAttr);
	}
	//
	const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element);
	oVInit.m_nVarTeam = oPairOwner.first;
	oVInit.m_nVarMate = oPairOwner.second;
	const auto oPair = oCtx.getVariableIdAndOwnerTypeFromContext(sName, oVInit.m_nVarTeam, oVInit.m_nVarMate);
	oVInit.m_nVarIndex = oPair.first;
	if (oVInit.m_nVarIndex < 0) {
		throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, s_sEventVariableNameAttr, sName);
	}
	const OwnerType eOwnerType = oPair.second;
	if (eOwnerType == OwnerType::GAME) {
		oVInit.m_nVarTeam = -1;
		oVInit.m_nVarMate = -1;
	} else if (eOwnerType == OwnerType::TEAM) {
		assert(oVInit.m_nVarTeam >= 0);
		oVInit.m_nVarMate = -1;
	} else {
		assert(oVInit.m_nVarTeam >= 0);
		assert(oVInit.m_nVarMate >= 0);
	}
	const auto oPairDefaultIncBy = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventVariableDefaultIncByAttr);
	if (oPairDefaultIncBy.first) {
		const std::string& sIncBy = oPairDefaultIncBy.second;
		oVInit.m_nDefaultIncBy = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventVariableDefaultIncByAttr, sIncBy, false
															, false, -1, false, -1);
	}
	oCtx.removeChecker(p0Element, true);
	auto refVariableEvent = std::make_unique<VariableEvent>(std::move(oVInit));
	return refVariableEvent;
}
int32_t XmlVariableEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "SET_VALUE") {
		nMsg = VariableEvent::MESSAGE_SET_VALUE;
	} else if (sMsgName == "RESET") {
		nMsg = VariableEvent::MESSAGE_RESET;
	} else if (sMsgName == "ADD_VALUE") {
		nMsg = VariableEvent::MESSAGE_ADD_VALUE;
	} else if (sMsgName == "INC") {
		nMsg = VariableEvent::MESSAGE_INC;
	} else if (sMsgName == "ADD_PERC_VALUE") {
		nMsg = VariableEvent::MESSAGE_ADD_PERC_VALUE;
	} else if (sMsgName == "MUL_BY_VALUE") {
		nMsg = VariableEvent::MESSAGE_MUL_BY_VALUE;
	} else if (sMsgName == "NEG_ADD_VALUE") {
		nMsg = VariableEvent::MESSAGE_NEG_ADD_VALUE;
	} else if (sMsgName == "SUB_VALUE") {
		nMsg = VariableEvent::MESSAGE_SUB_VALUE;
	} else if (sMsgName == "DEC") {
		nMsg = VariableEvent::MESSAGE_DEC;
	} else if (sMsgName == "SUB_PERC_VALUE") {
		nMsg = VariableEvent::MESSAGE_SUB_PERC_VALUE;
	} else if (sMsgName == "DIV_BY_VALUE") {
		nMsg = VariableEvent::MESSAGE_DIV_BY_VALUE;
	} else if (sMsgName == "NEG_SUB_VALUE") {
		nMsg = VariableEvent::MESSAGE_NEG_SUB_VALUE;
	} else if (sMsgName == "MIN_VALUE") {
		nMsg = VariableEvent::MESSAGE_MIN_VALUE;
	} else if (sMsgName == "MAX_VALUE") {
		nMsg = VariableEvent::MESSAGE_MAX_VALUE;
	} else if (sMsgName == "VAR_GET_VALUE") {
		nMsg = VariableEvent::MESSAGE_VAR_GET_VALUE;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlVariableEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
															, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "VAR_CHANGED") {
		nListenerGroup = VariableEvent::LISTENER_GROUP_VAR_CHANGED;
	} else if (sListenerGroupName == "VAR_VALUE") {
		nListenerGroup = VariableEvent::LISTENER_GROUP_VAR_VALUE;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
