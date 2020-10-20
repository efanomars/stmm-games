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
 * File:   xmlimageparser.cc
 */


#include "xmlutil/xmlimageparser.h"

#include "conditionalctx.h"
#include "xmlcommonerrors.h"
#include "xmlconditionalparser.h"

#include <stmm-games/named.h>
#include <stmm-games/util/namedindex.h>
#include <stmm-games/util/util.h>

//#include <cassert>
//#include <iostream>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

const std::string XmlImageParser::s_sImagesImageIdAttr = "imgId";

XmlImageParser::XmlImageParser(XmlConditionalParser& oXmlConditionalParser)
: m_oXmlConditionalParser(oXmlConditionalParser)
{
}
int32_t XmlImageParser::parseImageId(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
									, bool bAttrMandatory)
{
	return parseImageId(oCtx, p0Element, s_sImagesImageIdAttr, bAttrMandatory);
}
int32_t XmlImageParser::parseImageId(ConditionalCtx& oCtx, const xmlpp::Element* p0Element
															, const std::string& sImgIdAttr
															, bool bAttrMandatory)
{
	oCtx.addChecker(p0Element);
	int32_t nImgId = -1;
	const auto oPairImgId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, sImgIdAttr);
	if (oPairImgId.first) {
		const std::string& sImgId = oPairImgId.second;
		nImgId = oCtx.named().images().getIndex(sImgId);
		if (nImgId < 0) {
			throw XmlCommonErrors::error(oCtx, p0Element, sImgIdAttr, Util::stringCompose(
									"Attribute '%1': image id '%2' not defined", sImgIdAttr, sImgId));
		}
	}
	if (bAttrMandatory && (nImgId < 0)) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, sImgIdAttr);
	}
	oCtx.removeChecker(p0Element, false, true);
	return nImgId;
}

} // namespace stmg
