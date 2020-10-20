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
 * File:   xmlpositionerevent.h
 */

#ifndef STMG_XML_POSITIONER_EVENT_H
#define STMG_XML_POSITIONER_EVENT_H

#include "xmleventparser.h"

#include <stmm-games-xml-base/xmlcommonparser.h>

#include <memory>
#include <string>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class Event; }
namespace stmg { class GameCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlPositionerEventParser : public XmlEventParser
{
public:
	XmlPositionerEventParser();

	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) override;

	//TODO void recycleEvents(std::unique_ptr<Event>& refEvent) override;
	int32_t parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
							, const std::string& sMsgName) override;
private:
	unique_ptr<Event> parseEventPositioner(GameCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlPositionerEventParser(const XmlPositionerEventParser& oSource) = delete;
	XmlPositionerEventParser& operator=(const XmlPositionerEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_POSITIONER_EVENT_H */

