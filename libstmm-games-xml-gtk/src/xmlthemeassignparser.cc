/*
 * File:   xmlthemeassignparser.cc
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

#include "xmlthemeassignparser.h"

#include "xmlutil/xmlstrconv.h"
#include "themectx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/stdtheme.h>

#include <stmm-games/traitset.h>
#include <stmm-games/traitsets/alphatraitset.h>
#include <stmm-games/traitsets/chartraitset.h>
#include <stmm-games/traitsets/colortraitset.h>
#include <stmm-games/traitsets/fonttraitset.h>
#include <stmm-games/util/intset.h>
#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>

#include <libxml++/libxml++.h>

#include <vector>
//#include <cassert>
//#include <iostream>
#include <cstdint>
#include <string>
#include <utility>

namespace stmg { class XmlThemeImagesParser; }

namespace stmg
{

static const std::string s_sAssignNodeName = "Assign";

static const std::string s_sAssignPlayersNodeName = "Players";
static const std::string s_sAssignCharsNodeName = "Chars";
static const std::string s_sAssignColorsNodeName = "Colors";
static const std::string s_sAssignFontsNodeName = "Fonts";
static const std::string s_sAssignAlphasNodeName = "Alphas";

static const std::string s_sAssignImagesArrayIdAttr = "arrayId";
static const std::string s_sAssignImagesFromIdxAttr = "imgFromIdx";
static const std::string s_sAssignAssignIdAssAttr = "idAss";


XmlThemeAssignParser::XmlThemeAssignParser(XmlConditionalParser& oXmlConditionalParser, XmlTraitsParser& oXmlTraitsParser
											, XmlThemeImagesParser& /*oXmlThemeImagesParser*/)
: m_oXmlConditionalParser(oXmlConditionalParser)
, m_oXmlTraitsParser(oXmlTraitsParser)
//, m_oXmlThemeImagesParser(oXmlThemeImagesParser)
{
}

void XmlThemeAssignParser::parseAssigns(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeParser::parseAssigns()" << '\n';
	oCtx.addChecker(p0Element);
	m_oXmlConditionalParser.visitNamedElementChildren(oCtx, p0Element, s_sAssignNodeName, [&](const xmlpp::Element* p0AssignElement)
	{
		parseAssign(oCtx, p0AssignElement);
	});
	oCtx.removeChecker(p0Element, true);
}
void XmlThemeAssignParser::parseAssign(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
//std::cout << "XmlThemeParser::parseAssign()" << '\n';
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();

	const auto oPairIdAss = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sAssignAssignIdAssAttr);
	if (! oPairIdAss.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sAssignAssignIdAssAttr);
	}
	const std::string& sIdAss = oPairIdAss.second;
	if (sIdAss.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sAssignAssignIdAssAttr);
	}
	if (oCtx.isMain() && oTheme.hasAssignId(sIdAss)) {
		throw XmlCommonErrors::errorAttrWithValueAlreadyDefined(oCtx, p0Element, s_sAssignAssignIdAssAttr, sIdAss);
	}

	std::vector< std::unique_ptr<TraitSet> > aTileTraitSets;
	std::unique_ptr<IntSet> refPlayerTraitSet;
	bool bPlayersFirst = false;
	//
	bool bPlayersSet = false;
	//
	bool bTileTraitSet = false;
	bool bCharsTraitSet = false;
	bool bColorsTraitSet = false;
	bool bFontsTraitSet = false;
	bool bAlphasTraitSet = false;
	m_oXmlConditionalParser.visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0TraitSetElement)
	{
		oCtx.addChecker(p0TraitSetElement);
		const std::string sElementName = p0TraitSetElement->get_name();
		if (sElementName == s_sAssignPlayersNodeName) {
			if (bPlayersSet) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0TraitSetElement, s_sAssignPlayersNodeName);
			}
			refPlayerTraitSet = std::make_unique<IntSet>();
			m_oXmlTraitsParser.parsePlayers(oCtx, p0TraitSetElement, *refPlayerTraitSet);
			bPlayersFirst = !bTileTraitSet;
			bPlayersSet = true;
		} else {
			if (bPlayersSet && !bPlayersFirst) {
				throw XmlCommonErrors::error(oCtx, p0TraitSetElement, sElementName, Util::stringCompose(
										"Attribute %1 must be either first or last", s_sAssignPlayersNodeName));
			}
			if (sElementName == s_sAssignCharsNodeName) {
				if (bCharsTraitSet) {
					throw XmlCommonErrors::errorElementTooMany(oCtx, p0TraitSetElement, s_sAssignCharsNodeName);
				}
				auto refCharTraitSet = std::make_unique<CharTraitSet>();
				m_oXmlTraitsParser.parseChars(oCtx, p0TraitSetElement, *refCharTraitSet);
				aTileTraitSets.push_back(std::move(refCharTraitSet));
				bCharsTraitSet = true;
				bTileTraitSet = true;
			} else if (sElementName == s_sAssignColorsNodeName) {
				if (bColorsTraitSet) {
					throw XmlCommonErrors::errorElementTooMany(oCtx, p0TraitSetElement, s_sAssignColorsNodeName);
				}
				auto refColorTraitSet = std::make_unique<ColorTraitSet>();
				m_oXmlTraitsParser.parseColors(oCtx, p0TraitSetElement, true, *refColorTraitSet);
				aTileTraitSets.push_back(std::move(refColorTraitSet));
				bColorsTraitSet = true;
				bTileTraitSet = true;
			} else if (sElementName == s_sAssignFontsNodeName) {
				if (bFontsTraitSet) {
					throw XmlCommonErrors::errorElementTooMany(oCtx, p0TraitSetElement, s_sAssignFontsNodeName);
				}
				auto refFontTraitSet = std::make_unique<FontTraitSet>();
				m_oXmlTraitsParser.parseFonts(oCtx, p0TraitSetElement, *refFontTraitSet);
				aTileTraitSets.push_back(std::move(refFontTraitSet));
				bFontsTraitSet = true;
				bTileTraitSet = true;
			} else if (sElementName == s_sAssignAlphasNodeName) {
				if (bAlphasTraitSet) {
					throw XmlCommonErrors::errorElementTooMany(oCtx, p0TraitSetElement, s_sAssignAlphasNodeName);
				}
				auto refAlphaTraitSet = std::make_unique<AlphaTraitSet>();
				m_oXmlTraitsParser.parseAlphas(oCtx, p0TraitSetElement, *refAlphaTraitSet);
				aTileTraitSets.push_back(std::move(refAlphaTraitSet));
				bAlphasTraitSet = true;
				bTileTraitSet = true;
			} else {
				throw XmlCommonErrors::errorElementInvalid(oCtx, p0TraitSetElement, sElementName);
			}
		}
		oCtx.removeChecker(p0TraitSetElement, true);
	});
	if ((!bPlayersSet) && !bTileTraitSet) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, "At least one child element needed");
	}

	const auto oPairArrayId = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sAssignImagesArrayIdAttr);
	if (! oPairArrayId.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sAssignImagesArrayIdAttr);
	}
	const std::string& sArrayId = oPairArrayId.second;
	if (!oTheme.hasArray(sArrayId)) {
		throw XmlCommonErrors::error(oCtx, p0Element, s_sAssignImagesArrayIdAttr, Util::stringCompose(
										"Image Array Id '%1' not defined", sArrayId));
	}
	const int32_t nArrSize = oTheme.arraySize(sArrayId);
	//
	int32_t nArrayFrom = 0; //default
	const auto oPairFromIdx = m_oXmlConditionalParser.getAttributeValue(oCtx, p0Element, s_sAssignImagesFromIdxAttr);
	if (oPairFromIdx.first) {
		const std::string& sFromIdx = oPairFromIdx.second;
		nArrayFrom = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sAssignImagesFromIdxAttr, sFromIdx, false
															, true, 0, true, nArrSize - 1);
	}
	// child elements are checked in the visitor
	oCtx.removeChecker(p0Element, false, true);
	oCtx.theme().addAssign(sIdAss, std::move(refPlayerTraitSet), bPlayersFirst, std::move(aTileTraitSets), sArrayId, nArrayFrom);
}

} // namespace stmg
