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
 * File:   xmllogevent.h
 */

#ifndef STMG_XML_LOG_EVENT_H
#define STMG_XML_LOG_EVENT_H

#include "xmleventparser.h"

#include <memory>

namespace stmg { class Event; }
namespace stmg { class GameCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlLogEventParser : public XmlEventParser
{
public:
	XmlLogEventParser();

	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) override;

	//TODO void recycleEvents(std::unique_ptr<Event>& refEvent) override;
private:
	unique_ptr<Event> parseEventLog(GameCtx& oCtx, const xmlpp::Element* p0Element);

private:
	XmlLogEventParser(const XmlLogEventParser& oSource) = delete;
	XmlLogEventParser& operator=(const XmlLogEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_LOG_EVENT_H */

