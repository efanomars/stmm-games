/*
 * File:   xmlselectevent.cc
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

#include "events/xmlselectevent.h"
#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games/event.h>
#include <stmm-games/events/selectevent.h>
#include <stmm-games/ownertype.h>

#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventSelectNodeName = "SelectEvent";
static const std::string s_sEventSelectNameAttr = "varName";

XmlSelectEventParser::XmlSelectEventParser()
: XmlEventParser(s_sEventSelectNodeName)
{
}

Event* XmlSelectEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventSelect(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlSelectEventParser::parseEventSelect(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlOthersEventParser::parseEventSelect" << '\n';
	oCtx.addChecker(p0Element);
	SelectEvent::Init oOSInit;
	parseEventBase(oCtx, p0Element, oOSInit);

	const auto oPairName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSelectNameAttr);
	if (oPairName.first) {
		const std::string& sName = oPairName.second;
		if (sName.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventSelectNameAttr);
		}
		//
		const auto oPairOwner = getXmlConditionalParser().parseOwner(oCtx, p0Element);
		oOSInit.m_nVarTeam = oPairOwner.first;
		oOSInit.m_nVarMate = oPairOwner.second;
		const auto oPair = oCtx.getVariableIdAndOwnerTypeFromContext(sName, oOSInit.m_nVarTeam, oOSInit.m_nVarMate);
		oOSInit.m_nVarIndex = oPair.first;
		if (oOSInit.m_nVarIndex < 0) {
			throw XmlCommonErrors::errorAttrVariableNotDefined(oCtx, p0Element, s_sEventSelectNameAttr, sName);
		}
		const OwnerType eOwnerType = oPair.second;
		if (eOwnerType == OwnerType::GAME) {
			oOSInit.m_nVarTeam = -1;
			oOSInit.m_nVarMate = -1;
		} else if (eOwnerType == OwnerType::TEAM) {
			assert(oOSInit.m_nVarTeam >= 0);
			oOSInit.m_nVarMate = -1;
		} else {
			assert(oOSInit.m_nVarTeam >= 0);
			assert(oOSInit.m_nVarMate >= 0);
		}
	} else {
		oOSInit.m_nVarIndex = -1;
		oOSInit.m_nVarTeam = -1;
		oOSInit.m_nVarMate = -1;
	}
	//
	oCtx.removeChecker(p0Element, true);

	auto refSelectEvent = std::make_unique<SelectEvent>(std::move(oOSInit));
	return refSelectEvent;
}

} // namespace stmg
