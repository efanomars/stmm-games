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
 * File:   xmltextmodifier.cc
 */

#include "modifiers/xmltextmodifier.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>
#include <stmm-games-xml-base/xmltraitsparser.h>

#include <stmm-games-gtk/stdthememodifier.h>
#include <stmm-games-gtk/modifiers/textmodifier.h>

#include <cstdint>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierTextNodeName = "Text";
static const std::string s_sModifierTextTextAttr = "text";
static const std::string s_sModifierTextSizeFactorAttr = "sizeFactor";
static const std::string s_sModifierTextUseTileColorAttr = "useTileColor";
static const std::string s_sModifierTextUseTileFontAttr = "useTileFont";
static const std::string s_sModifierTextAddToCharAttr = "addToChar";
static const std::string s_sModifierTextStretchAttr = "stretch";

XmlTextModifierParser::XmlTextModifierParser()
: XmlModifierParser(s_sModifierTextNodeName)
{
}

unique_ptr<StdThemeModifier> XmlTextModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	TextModifier::Init oInit;

	oCtx.addChecker(p0Element);
	auto oPairText = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextTextAttr);
	oInit.m_sText = std::move(oPairText.second);

	const auto oPairUseTileColor = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextUseTileColorAttr);
	if (oPairUseTileColor.first) {
		const std::string& sUseTileColor = oPairUseTileColor.second;
		oInit.m_bUseTileColor = XmlUtil::strToBool(oCtx, p0Element, s_sModifierTextUseTileColorAttr, sUseTileColor);
	}

	getXmlTraitsParser()->parseColor(oCtx, p0Element, oInit.m_oColor);
	//
	const auto oPairUseTileFont = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextUseTileFontAttr);
	if (oPairUseTileFont.first) {
		const std::string& sUseTileFont = oPairUseTileFont.second;
		oInit.m_bUseTileFont = XmlUtil::strToBool(oCtx, p0Element, s_sModifierTextUseTileFontAttr, sUseTileFont);
	}

	getXmlTraitsParser()->parseFont(oCtx, p0Element, oInit.m_oFont);
	//
	const auto oPairStretch = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextStretchAttr);
	if (oPairStretch.first) {
		const std::string& sStretch = oPairStretch.second;
		oInit.m_bStretch = XmlUtil::strToBool(oCtx, p0Element, s_sModifierTextStretchAttr, sStretch);
	}
	//
	const auto oPairSizeFactor = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextSizeFactorAttr);
	if (oPairSizeFactor.first) {
		const std::string& sSizeFactor = oPairSizeFactor.second;
		oInit.m_fFontSize1 = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sModifierTextSizeFactorAttr, sSizeFactor, true
															, true, 0.001, true, 1.00);
	}
	//
	const auto oPairAddToChar = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierTextAddToCharAttr);
	if (oPairAddToChar.first) {
		const std::string& sAddToChar = oPairAddToChar.second;
		oInit.m_nAddToChar = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sModifierTextSizeFactorAttr, sAddToChar, false
																	, false, -1, false, -1);
	}

	oCtx.removeChecker(p0Element, true);

	return std::make_unique<TextModifier>(&oCtx.theme(), std::move(oInit));
}

} // namespace stmg

