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
 * File:   xmlrotatemodifier.cc
 */

#include "modifiers/xmlrotatemodifier.h"
#include "xmlgtkutil/xmlelapsedmapperparser.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/modifiers/rotatemodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <string>
#include <tuple>
#include <utility>

namespace stmg { class StdTheme; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierRotateNodeName = "Rotate";

static const std::string s_sModifierRotateElapsedMapperNodeName = "ElapsedMapper";


XmlRotateModifierParser::XmlRotateModifierParser()
: XmlModifierParser(s_sModifierRotateNodeName)
{
}

unique_ptr<StdThemeModifier> XmlRotateModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	oCtx.pushCtx(s_sModifierRotateNodeName + ": ");

	RotateModifier::Init oRInit;

	const auto oTupleAni = parseTileAniNameDefault(oCtx, p0Element, true);
	oRInit.m_nElapsedTileAniIdx = std::get<0>(oTupleAni);
	oRInit.m_fDefaultElapsed = std::get<1>(oTupleAni);
	oRInit.m_bInvert = std::get<2>(oTupleAni);

	const xmlpp::Element* p0ElapsedMapperElement = getXmlConditionalParser()->parseUniqueElement(oCtx, p0Element, s_sModifierRotateElapsedMapperNodeName, false);
	if (p0ElapsedMapperElement != nullptr) {
		stmg::XmlElapsedMapperParser oXmlElapsedMapperParser(*getXmlConditionalParser());
		oRInit.m_oMapper = oXmlElapsedMapperParser.parseElapsedMapper(oCtx, p0ElapsedMapperElement);
	}

	unique_ptr<RotateModifier> refRotateModifier = std::make_unique<RotateModifier>(&oTheme, std::move(oRInit));

	auto aModifiers = parseSubModifiers(oCtx, p0Element);
	refRotateModifier->addSubModifiers(std::move(aModifiers));

	oCtx.popCtx();
	// child elements already checked by parseModifiers
	oCtx.removeChecker(p0Element, false, true);
	return refRotateModifier;
}

} // namespace stmg

