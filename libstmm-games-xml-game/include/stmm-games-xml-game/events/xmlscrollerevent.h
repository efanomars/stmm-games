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
 * File:   xmlscrollerevent.h
 */

#ifndef STMG_XML_SCROLLER_EVENT_H
#define STMG_XML_SCROLLER_EVENT_H

#include "xmleventparser.h"

#include <stmm-games/events/scrollerevent.h>

#include <string>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class Event; }
namespace stmg { class GameCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

class XmlScrollerEventParser : public XmlEventParser
{
public:
	XmlScrollerEventParser();

	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) override;

	int32_t parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
							, const std::string& sMsgName) override;
	int32_t parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sListenerGroupName) override;

	static const std::string s_sEventScrollerNodeName;

	using XmlEventParser::getXmlConditionalParser;
	using XmlEventParser::getXmlTraitsParser;
	using XmlEventParser::parseEventAttrRandomProb;
private:
	Event* parseEventScroller(GameCtx& oCtx, const xmlpp::Element* p0Element);
	void parseNewRowChecker(GameCtx& oCtx, const xmlpp::Element* p0Element, ScrollerEvent::Init& oInit);

private:
	XmlScrollerEventParser(const XmlScrollerEventParser& oSource) = delete;
	XmlScrollerEventParser& operator=(const XmlScrollerEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_SCROLLER_EVENT_H */

