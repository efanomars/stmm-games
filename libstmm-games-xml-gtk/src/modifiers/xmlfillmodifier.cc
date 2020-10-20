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
 * File:   xmlfillmodifier.cc
 */

#include "modifiers/xmlfillmodifier.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games-gtk/stdthememodifier.h>
#include <stmm-games-gtk/modifiers/fillmodifier.h>

#include <cstdint>
#include <string>
#include <utility>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierFillNodeName = "Fill";
static const std::string s_sModifierFillAlphaAttr = "alpha";

XmlFillModifierParser::XmlFillModifierParser()
: XmlModifierParser(s_sModifierFillNodeName)
{
}

unique_ptr<StdThemeModifier> XmlFillModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	FillModifier::Init oInit;

	oCtx.addChecker(p0Element);

	auto oPairAlpha = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierFillAlphaAttr);
	if (oPairAlpha.first) {
		const std::string& sAlpha = oPairAlpha.second;
		oInit.m_nAlpha = XmlUtil::strToRangeNumber<int32_t>(oCtx, p0Element, s_sModifierFillAlphaAttr, sAlpha, 0, 255);
	}

	oCtx.removeChecker(p0Element, true);
	return std::make_unique<FillModifier>(&oCtx.theme(), std::move(oInit));
}

} // namespace stmg

