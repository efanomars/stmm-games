/*
 * File:   xmlrandomevent.cc
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

#include "events/xmlrandomevent.h"

#include "gamectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/event.h>
#include <stmm-games/events/randomevent.h>

//#include <cassert>
//#include <iostream>
#include <limits>
#include <cstdint>
#include <utility>

namespace stmg { class ConditionalCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventRandomNodeName = "RandomEvent";
static const std::string s_sEventRandomFromAttr = "from";
static const std::string s_sEventRandomToAttr = "to";
static const std::string s_sEventRandomSharedNameAttr = "sharedName";
static const std::string s_sEventRandomBufferSizeAttr = "bufferSize";
static const std::string s_sEventRandomPermutationsAttr = "permutations";

XmlRandomEventParser::XmlRandomEventParser()
: XmlEventParser(s_sEventRandomNodeName)
{
}

Event* XmlRandomEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventRandom(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlRandomEventParser::parseEventRandom(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlRandomEventParser::parseEventRandom" << '\n';
	oCtx.addChecker(p0Element);
	RandomEvent::Init oRInit;
	parseEventBase(oCtx, p0Element, oRInit);

	const auto oPairPermutations = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventRandomPermutationsAttr);
	if (oPairPermutations.first) {
		oRInit.m_bPermutations = XmlUtil::strToBool(oCtx, p0Element, s_sEventRandomPermutationsAttr, oPairPermutations.second);
	}

	int32_t nMinValue = std::numeric_limits<int32_t>::min();
	int32_t nMaxValue = std::numeric_limits<int32_t>::max();
	if (oRInit.m_bPermutations) {
		nMinValue = RandomEvent::s_nPermutationMinValue;
		nMaxValue = RandomEvent::s_nPermutationMaxValue;
	}

	getXmlConditionalParser().parseAttributeFromTo<int32_t>(oCtx, p0Element, "</>" // impossible attribute name
															, s_sEventRandomFromAttr, s_sEventRandomToAttr
															, false, true, nMinValue, true, nMaxValue, oRInit.m_nFrom, oRInit.m_nTo);

	const auto oPairSharedName = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventRandomSharedNameAttr);
	if (oPairSharedName.first) {
		oRInit.m_sSharedName = oPairSharedName.second;
		if (oRInit.m_sSharedName.empty()) {
			throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sEventRandomSharedNameAttr);
		}
	}

	const auto oPairBufferSize = getXmlConditionalParser().getAttributeValue(oCtx, p0Element, s_sEventRandomBufferSizeAttr);
	if (oPairBufferSize.first) {
		oRInit.m_nBufferSize = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sEventRandomBufferSizeAttr, oPairBufferSize.second, false
																	, true, 1, false, -1);
	}

	oCtx.removeChecker(p0Element, true);

	auto refRandomEvent = std::make_unique<RandomEvent>(std::move(oRInit));
	return refRandomEvent;
}
int32_t XmlRandomEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
												, const std::string& sMsgName)
{
	int32_t nMsg;
	if (sMsgName == "GENERATE") {
		nMsg = RandomEvent::MESSAGE_GENERATE;
	} else if (sMsgName == "GENERATE_ADD") {
		nMsg = RandomEvent::MESSAGE_GENERATE_ADD;
	} else {
		return XmlEventParser::parseEventMsgName(oCtx, p0Element, sAttr, sMsgName);
	}
	return nMsg;
}
int32_t XmlRandomEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
															, const std::string& sListenerGroupName)
{
	int32_t nListenerGroup;
	if (sListenerGroupName == "RANDOM") {
		nListenerGroup = RandomEvent::LISTENER_GROUP_RANDOM;
	} else {
		return XmlEventParser::parseEventListenerGroupName(oCtx, p0Element, sAttr, sListenerGroupName);
	}
	return nListenerGroup;
}

} // namespace stmg
