/*
 * File:   xmlothersevent.cc
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

#include "events/xmlothersevent.h"

#include "gamectx.h"

#include <stmm-games/event.h>
#include <stmm-games/events/othersevent.h>

//#include <cassert>
//#include <iostream>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sEventOthersSenderNodeName = "SenderEvent";
static const std::string s_sEventOthersReceiverNodeName = "ReceiverEvent";

XmlOthersSenderEventParser::XmlOthersSenderEventParser()
: XmlEventParser(s_sEventOthersSenderNodeName)
{
}

Event* XmlOthersSenderEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventOthersSender(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlOthersSenderEventParser::parseEventOthersSender(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlOthersEventParser::parseEventOthersSender" << '\n';
	oCtx.addChecker(p0Element);
	OthersSenderEvent::Init oOSInit;
	parseEventBase(oCtx, p0Element, oOSInit);
	oCtx.removeChecker(p0Element, true);
	auto refOthersSenderEvent = std::make_unique<OthersSenderEvent>(std::move(oOSInit));
	return refOthersSenderEvent;
}


XmlOthersReceiverEventParser::XmlOthersReceiverEventParser()
: XmlEventParser(s_sEventOthersReceiverNodeName)
{
}

Event* XmlOthersReceiverEventParser::parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	return integrateAndAdd(oCtx, parseEventOthersReceiver(oCtx, p0Element), p0Element);
}
unique_ptr<Event> XmlOthersReceiverEventParser::parseEventOthersReceiver(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlOthersEventParser::parseEventOthersReceiver" << '\n';
	oCtx.addChecker(p0Element);
	OthersReceiverEvent::Init oORInit;
	parseEventBase(oCtx, p0Element, oORInit);
	oCtx.removeChecker(p0Element, true);
	auto refOthersReceiverEvent = std::make_unique<OthersReceiverEvent>(std::move(oORInit));
	return refOthersReceiverEvent;
}

} // namespace stmg
