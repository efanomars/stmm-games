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
 * File:   xmlarrayevent.h
 */

#ifndef STMG_XML_ARRAY_EVENT_H
#define STMG_XML_ARRAY_EVENT_H

#include "xmleventparser.h"

#include <stmm-games/events/arrayevent.h>

#include <memory>
#include <string>
#include <tuple>
#include <vector>

#include <stdint.h>

namespace stmg { class ConditionalCtx; }
namespace stmg { class Event; }
namespace stmg { class GameCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;


class XmlArrayEventParser : public XmlEventParser
{
public:
	XmlArrayEventParser();

	Event* parseEvent(GameCtx& oCtx, const xmlpp::Element* p0Element) override;

	//TODO void recycleEvents(std::unique_ptr<Event>& refEvent) override;
	int32_t parseEventMsgName(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
							, const std::string& sMsgName) override;
	int32_t parseEventListenerGroupName(GameCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr
										, const std::string& sListenerGroupName) override;
private:
	unique_ptr<Event> parseEventArray(GameCtx& oCtx, const xmlpp::Element* p0Element);
	void parseEventArrayDimensions(GameCtx& oCtx, const xmlpp::Element* p0Element, std::vector<ArrayEvent::Dimension>& aDimensions);
	// returns (varId, pref team, mate)
	std::tuple<int32_t, int32_t, int32_t> parseVariableAndOwner(GameCtx& oCtx, const xmlpp::Element* p0Element, bool bMandatory);
	enum ARRAY_VALUE_TYPE {
		  ARRAY_VALUE_TYPE_VALUE = 0
		, ARRAY_VALUE_TYPE_IMAGE = 1
		, ARRAY_VALUE_TYPE_SOUND = 2
		, ARRAY_VALUE_TYPE_TILEANI = 3
	};

	void parseEventArrayArray(GameCtx& oCtx, const xmlpp::Element* p0Element, ArrayEvent::Init& oAInit
								, ARRAY_VALUE_TYPE eValueType);
	void parseEventArraySubArray(GameCtx& oCtx, const xmlpp::Element* p0Element
								, ArrayEvent::Init& oAInit, std::vector<int32_t>& aIdxs
								, ARRAY_VALUE_TYPE eValueType);
	void parseEventArrayArrayEl(GameCtx& oCtx, const xmlpp::Element* p0Element
								, std::vector<int32_t>& aValues, int32_t nFromIdx, int32_t nSize
								, ARRAY_VALUE_TYPE eValueType);

private:
	XmlArrayEventParser(const XmlArrayEventParser& oSource) = delete;
	XmlArrayEventParser& operator=(const XmlArrayEventParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_ARRAY_EVENT_H */

