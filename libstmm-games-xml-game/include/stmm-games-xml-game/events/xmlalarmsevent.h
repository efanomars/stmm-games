/*
 * File:   xmlalarmsevent.h
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

#ifndef STMG_XML_ALARMS_EVENT_H
#define STMG_XML_ALARMS_EVENT_H

#include "xmleventparser.h"

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


class XmlAlarmsEventParser : public XmlEventParser
{
public:
	XmlAlarmsEventParser();

	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) override;

	//TODO void recycleEvents(std::unique_ptr<Event>& refEvent) override;
	int32_t parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
							, const std::string& sMsgName) override;
	int32_t parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sListenerGroupName) override;
private:
	unique_ptr<Event> parseEventAlarms(GameCtx& oCtx, const xmlpp::Element* p0Element);
	void parseEventAlarmsChanger(GameCtx& oCtx, const xmlpp::Element* p0Element, bool bPerc, int32_t& nValue, int32_t& nRepeat);

private:
	XmlAlarmsEventParser(const XmlAlarmsEventParser& oSource) = delete;
	XmlAlarmsEventParser& operator=(const XmlAlarmsEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ALARMS_EVENT_H */
