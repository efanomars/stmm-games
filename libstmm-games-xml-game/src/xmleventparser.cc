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
 * File:   xmleventparser.cc
 */

#include "xmleventparser.h"

#include "xmlgameparser.h"

#include <stmm-games/event.h>

#include <cassert>
//#include <iostream>
#include <utility>

namespace stmg { class Block; }
namespace stmg { class ConditionalCtx; }
namespace stmg { class GameCtx; }
namespace stmg { class XmlConditionalParser; }
namespace stmg { class XmlTraitsParser; }
namespace xmlpp { class Element; }

namespace stmg
{

XmlEventParser::XmlEventParser(const std::string& sEventName)
: m_sEventName(sEventName)
, m_p1Owner(nullptr)
, m_p0XmlConditionalParser(nullptr)
, m_p0XmlTraitsParser(nullptr)
{
	assert(!m_sEventName.empty());
}
const std::string& XmlEventParser::getEventName() const
{
	return m_sEventName;
}
void XmlEventParser::recycleEvents(std::unique_ptr<Event>& /*refEvent*/)
{
}
bool XmlEventParser::getBlock(GameCtx& oCtx, const std::string& sName, Block& oBlock)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return false;
	}
	#endif //NDEBUG
	return m_p1Owner->getBlock(oCtx, sName, oBlock);
}
bool XmlEventParser::isReservedChildElementOfEvent(const std::string& sElementName) const
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return false;
	}
	#endif //NDEBUG
	return m_p1Owner->isReservedChildElementOfEvent(sElementName);
}
int32_t XmlEventParser::parseEventAttrRepeat(GameCtx& oCtx, const xmlpp::Element* p0EventElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return 0;
	}
	#endif //NDEBUG
	return m_p1Owner->parseEventAttrRepeat(oCtx, p0EventElement);
}
int32_t XmlEventParser::parseEventAttrStep(GameCtx& oCtx, const xmlpp::Element* p0EventElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return 0;
	}
	#endif //NDEBUG
	return m_p1Owner->parseEventAttrStep(oCtx, p0EventElement);
}
int32_t XmlEventParser::parseEventAttrRandomProb(GameCtx& oCtx, const xmlpp::Element* p0EventElement)
{
	#ifndef NDEBUG
	if (m_p1Owner == nullptr) {
		return 0;
	}
	#endif //NDEBUG
	return m_p1Owner->parseEventAttrRandomProb(oCtx, p0EventElement);
}
XmlConditionalParser& XmlEventParser::getXmlConditionalParser()
{
	assert(m_p0XmlConditionalParser != nullptr);
	return *m_p0XmlConditionalParser;
}
XmlTraitsParser& XmlEventParser::getXmlTraitsParser()
{
	assert(m_p0XmlTraitsParser != nullptr);
	return *m_p0XmlTraitsParser;
}
Event* XmlEventParser::parseChildEvent(GameCtx& oCtx, const xmlpp::Element* p0Element)
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->parseEvent(oCtx, p0Element);
}
void XmlEventParser::parseEventBase(GameCtx& oCtx, const xmlpp::Element* p0Element, Event::Init& oInit)
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->parseEventBase(oCtx, p0Element, oInit);
}
Event* XmlEventParser::integrateAndAdd(GameCtx& oCtx, unique_ptr<Event> refEvent, const xmlpp::Element* p0Element)
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->integrateAndAdd(oCtx, std::move(refEvent), p0Element);
}
int32_t XmlEventParser::parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sMsgName)
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->parseEventBaseMsgName(oCtx, m_sEventName, p0Element, sAttr, sMsgName);
}
int32_t XmlEventParser::parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
											, const std::string& sListenerGroupName)
{
	assert(m_p1Owner != nullptr);
	return m_p1Owner->parseEventBaseListenerGroupName(oCtx, m_sEventName, p0Element, sAttr, sListenerGroupName);
}

} // namespace stmg
