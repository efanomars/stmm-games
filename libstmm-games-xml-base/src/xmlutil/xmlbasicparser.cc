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
 * File:   xmlbasicparser.cc
 */

#include "xmlutil/xmlbasicparser.h"

#include "xmlcommonerrors.h"
#include "conditionalctx.h"
#include "xmlconditionalparser.h"
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/util/basictypes.h>
#include <stmm-games/util/util.h>

#include <cassert>
//#include <iostream>
#include <string>
#include <cstdint>
#include <stdexcept>
#include <utility>

namespace stmg { class ParserCtx; }
namespace xmlpp { class Element; }

namespace stmg
{

XmlBasicParser::XmlBasicParser(XmlConditionalParser& oXmlConditionalParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
{
}

static std::runtime_error errorRectangleCantFit(ParserCtx& oCtx, const xmlpp::Element* p0Element, const std::string& sAttr, const NRect& oInRect)
{
	return XmlCommonErrors::error(oCtx, p0Element, sAttr, Util::stringCompose("rectangle defined by attribute '%1'"
															" can't fit into rectangle (%2, %3, %4, %5)", sAttr
															, oInRect.m_nX, oInRect.m_nY, oInRect.m_nW, oInRect.m_nH));
}

NRect XmlBasicParser::parseNRect(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
								, const std::string& sAttrNameX, const std::string& sAttrNameY
								, const std::string& sAttrNameW, const std::string& sAttrNameH
								, bool bMandatory, const NRect& oInRect, const NSize& oMinSize)
{
	assert((oMinSize.m_nW > 0) && (oMinSize.m_nH > 0));
	const bool bInRect = (oInRect.m_nW > 0) && (oInRect.m_nH > 0);
	assert(bInRect || ((oInRect.m_nW == 0) && (oInRect.m_nH == 0)));

	const auto oPairRectX = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sAttrNameX);
	const auto oPairRectY = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sAttrNameY);
	const auto oPairRectW = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sAttrNameW);
	const auto oPairRectH = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sAttrNameH);

	const bool bAnyDefined = (oPairRectX.first || oPairRectY.first || oPairRectW.first || oPairRectH.first);
	if (! bAnyDefined) {
		if (bInRect) {
			if (! bMandatory) {
				return NRect{}; //-----------------------------------------------
			}
			// since rectangle is mandatory deduce it from oInRect even if no attr defined
		} else {
			if (! bMandatory) {
				return NRect{}; //-----------------------------------------------
			}
			// all attributes must be defined: handled further down
		}
	}
	NRect oRect;
	if (bInRect) {
		oRect.m_nX = oInRect.m_nX;
		oRect.m_nY = oInRect.m_nY;
	}
	if (oPairRectX.first) {
		oRect.m_nX = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sAttrNameX, oPairRectX.second, false
												, bInRect, oInRect.m_nX, bInRect, oInRect.m_nX + oInRect.m_nW - oMinSize.m_nW);
	} else if (bInRect) {
		oRect.m_nX = oInRect.m_nX;
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sAttrNameX);
	}
	//
	if (oPairRectY.first) {
		oRect.m_nY = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sAttrNameY, oPairRectY.second, false
												, bInRect, oInRect.m_nY, bInRect, oInRect.m_nY + oInRect.m_nH - oMinSize.m_nH);
	} else if (bInRect) {
		oRect.m_nY = oInRect.m_nY;
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sAttrNameY);
	}
	//
	int32_t nMaxW;
	if (bInRect) {
		nMaxW = oInRect.m_nX + oInRect.m_nW - oRect.m_nX;
		if (nMaxW < oMinSize.m_nW) {
			throw errorRectangleCantFit(oCtx, p0Element, sAttrNameW, oInRect);
		}
	} else {
		nMaxW = -1;
	}
	if (oPairRectW.first) {
		oRect.m_nW = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sAttrNameW, oPairRectW.second, false
												, true, oMinSize.m_nW, bInRect, nMaxW);
	} else if (bInRect) {
		oRect.m_nW = nMaxW;
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sAttrNameW);
	}
	//
	int32_t nMaxH;
	if (bInRect) {
		nMaxH = oInRect.m_nY + oInRect.m_nH - oRect.m_nY;
		if (nMaxH < oMinSize.m_nH) {
			throw errorRectangleCantFit(oCtx, p0Element, sAttrNameH, oInRect);
		}
	} else {
		nMaxH = -1;
	}
	if (oPairRectH.first) {
		oRect.m_nH = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, sAttrNameH, oPairRectH.second, false
												, true, oMinSize.m_nH, bInRect, nMaxH);
	} else if (bInRect) {
		oRect.m_nH = nMaxH;
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sAttrNameH);
	}
	return oRect;
}

} // namespace stmg
