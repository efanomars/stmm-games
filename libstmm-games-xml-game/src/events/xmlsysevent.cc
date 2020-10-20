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
 * File:   xmlsysevent.cc
 */

#include "events/xmlsysevent.h"

#include "gamectx.h"

#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games/event.h>
#include <stmm-games/events/sysevent.h>
#include <stmm-games/game.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventSysNodeName = "SysEvent";
static const std::string s_sEventSysTypeAttr = "type";
static const std::string s_sEventSysTypeAttrLevelCompleted = "LEVEL_COMPLETED";
static const std::string s_sEventSysTypeAttrLevelFailed = "LEVEL_FAILED";
static const std::string s_sEventSysTypeAttrPlayerOut = "PLAYER_OUT";
static const std::string s_sEventSysFinishIfPossibleAttr = "finishIfPossible";
static const std::string s_sEventSysCreateTextsAttr = "createTexts";

XmlSysEventParser::XmlSysEventParser()
: XmlEventParser(s_sEventSysNodeName)
{
}
Event* XmlSysEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventSys(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlSysEventParser::parseEventSys(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlGameParser::parseEventSys" << '\n';
	oCtx.addChecker(p0Element);
	SysEvent::Init oSInit;
	parseEventBase(oCtx, p0Element, oSInit);

	static std::vector<char const *> s_aEventTypeEnumString{s_sEventSysTypeAttrLevelCompleted.c_str(), s_sEventSysTypeAttrLevelFailed.c_str(), s_sEventSysTypeAttrPlayerOut.c_str()};
	static const std::vector<SysEvent::SYS_EVENT_TYPE> s_aEventTypeEnumValue{SysEvent::SYS_EVENT_TYPE_LEVEL_COMPLETED, SysEvent::SYS_EVENT_TYPE_LEVEL_FAILED, SysEvent::SYS_EVENT_TYPE_PLAYER_OUT};
	//
	const int32_t nIdxType = getXmlConditionalParser().getEnumAttributeValue(oCtx, p0Element, s_sEventSysTypeAttr, s_aEventTypeEnumString);
	if (nIdxType >= 0) {
		oSInit.m_eType = s_aEventTypeEnumValue[nIdxType];
	}
	const auto oPairFinish = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSysFinishIfPossibleAttr);
	if (oPairFinish.first) {
		oSInit.m_bFinishIfPossible = XmlUtil::strToBool(oCtx, p0Element, s_sEventSysFinishIfPossibleAttr, oPairFinish.second);
	}
	const auto oPairCreateTexts = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventSysCreateTextsAttr);
	if (oPairCreateTexts.first) {
		oSInit.m_bCreateTexts = XmlUtil::strToBool(oCtx, p0Element, s_sEventSysCreateTextsAttr, oPairCreateTexts.second);
	}
	if (oCtx.game().isAllTeamsInOneLevel()) {
		const auto oPairTeam = getXmlConditionalParser().parseTeamExists(oCtx, p0Element);
		if (!oPairTeam.first) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, XmlConditionalParser::s_sConditionalOwnerTeamAttr);
		}
		oSInit.m_nLevelTeam = oPairTeam.second;
	}
	oCtx.removeChecker(p0Element, true);

	auto refSysEvent = std::make_unique<SysEvent>(std::move(oSInit));
	return refSysEvent;
}

int32_t XmlSysEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
														, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "GAME_OVER") {
		nListenerGroup = SysEvent::LISTENER_GROUP_GAME_OVER;
	} else if (sListenerGroupName == "TEAM_FINISHED") {
		nListenerGroup = SysEvent::LISTENER_GROUP_TEAM_FINISHED;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
