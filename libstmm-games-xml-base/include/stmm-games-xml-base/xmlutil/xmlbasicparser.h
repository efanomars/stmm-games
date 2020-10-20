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
 * File:   xmlbasicparser.h
 */

#ifndef STMG_XML_BASIC_PARSER_H
#define STMG_XML_BASIC_PARSER_H

#include <stmm-games/util/basictypes.h>

#include <memory>
#include <string>

namespace stmg { class ConditionalCtx; }
namespace stmg { class XmlConditionalParser; }
namespace xmlpp { class Element; }

namespace stmg
{

using std::unique_ptr;

class XmlBasicParser
{
public:
	XmlBasicParser(XmlConditionalParser& oXmlConditionalParser);

	/** Parse integer rectangle.
	 * If oInRect has non zero size and all the attributes are not defined and bMandatory is false
	 * a zero sized rectangle is returned.
	 * Otherwise, if oInRect has non zero size and any of the attributes is not defined it is inferred
	 * from the oInRect in that the resulting rectangle is extended as much as possible within it.
	 * If oInRect has zero size and bMandatory is true then all attributes must be defined.
	 * If oInRect has zero size and bMandatory is false then all attributes must be not defined.
	 *
	 * Ex. oInRect={0,0,5,4}, sAttrNameX value is 2. Result is {2,0,3,4}
	 *
	 * Ex. oInRect={10,20,5,4}, sAttrNameH value is 2. Result is {10,20,2,4}
	 *
	 * Ex. oInRect={10,20,5,4}, bMandatory is false, no attribute defined. Result is {?,?,0,0}
	 *
	 * Ex. oInRect={10,20,5,4}, bMandatory is true, no attribute defined. Result is {10,20,5,4}
	 *
	 * Ex. oInRect={?,?,0,0}, bMandatory is false, no attribute defined. Result is {?,?,0,0}
	 *
	 * Ex. oInRect={?,?,0,0}, bMandatory is true, sAttrNameH value is 2. throws an error
	 * @param oCtx The context.
	 * @param p0Element The element. Cannot be null.
	 * @param sAttrNameX The x attribute name.
	 * @param sAttrNameY The y attribute name.
	 * @param sAttrNameW The w attribute name.
	 * @param sAttrNameH The h attribute name.
	 * @param bMandatory WhetherIt must be defined
	 * @param oInRect The rectangle the parsed rectangle must fit in. If its size is 0 no limits.
	 * @param oMinSize The minimum size of the parsed rectangle. Must be positive.
	 * @return The result rectangle. Has size 0 if not defined.
	 */
	NRect parseNRect(ConditionalCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttrNameX, const std::string& sAttrNameY
					, const std::string& sAttrNameW, const std::string& sAttrNameH, bool bMandatory, const NRect& oInRect, const NSize& oMinSize);

protected:
	XmlConditionalParser& m_oXmlConditionalParser;

private:
	XmlBasicParser() = delete;
	XmlBasicParser(const XmlBasicParser& oSource) = delete;
	XmlBasicParser& operator=(const XmlBasicParser& oSource) = delete;
};

} // namespace stmg

#endif	/* STMG_XML_BASIC_PARSER_H */

