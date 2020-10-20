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
 * File:   xmlfademodifier.cc
 */

#include "modifiers/xmlfademodifier.h"
#include "xmlgtkutil/xmlelapsedmapperparser.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/stdthememodifier.h>
#include <stmm-games-gtk/modifiers/fademodifier.h>

#include <string>
#include <tuple>
#include <utility>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierFadeNodeName = "Fade";

static const std::string s_sModifierFadeElapsedMapperNodeName = "ElapsedMapper";

XmlFadeModifierParser::XmlFadeModifierParser()
: XmlModifierParser(s_sModifierFadeNodeName)
{
}

unique_ptr<StdThemeModifier> XmlFadeModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	oCtx.pushCtx(s_sModifierFadeNodeName + ": ");

	FadeModifier::Init oFInit;

	const auto oTupleAni = parseTileAniNameDefault(oCtx, p0Element, true);
	oFInit.m_nElapsedTileAniIdx = std::get<0>(oTupleAni);
	oFInit.m_fDefaultElapsed = std::get<1>(oTupleAni);
	oFInit.m_bInvert = std::get<2>(oTupleAni);

	const xmlpp::Element* p0ElapsedMapperElement = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sModifierFadeElapsedMapperNodeName, false);
	if (p0ElapsedMapperElement != nullptr) {
		stmg::XmlElapsedMapperParser oXmlElapsedMapperParser(*getXmlConditionalParser());
		oFInit.m_oMapper = oXmlElapsedMapperParser.parseElapsedMapper(oCtx, p0ElapsedMapperElement);
	}

	unique_ptr<FadeModifier> refFadeModifier = std::make_unique<FadeModifier>(&oTheme, std::move(oFInit));

	auto aModifiers = parseSubModifiers(oCtx, p0Element, std::vector<std::string const*>{&s_sModifierFadeElapsedMapperNodeName});
	refFadeModifier->addSubModifiers(std::move(aModifiers));

	oCtx.popCtx();
	// child elements already checked by parseModifiers
	oCtx.removeChecker(p0Element, false, true);
	return refFadeModifier;
}

} // namespace stmg

