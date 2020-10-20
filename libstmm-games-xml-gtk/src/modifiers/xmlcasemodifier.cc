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
 * File:   xmlcasemodifier.cc
 */

#include "modifiers/xmlcasemodifier.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>

#include <stmm-games-gtk/modifiers/casemodifier.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>

#include <cassert>
#include <string>
#include <utility>

#include <stdint.h>

namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sModifierCaseNodeName = "SelectCase";
static const std::string s_sModifierCaseAssIdAttr = "idAss";

XmlCaseModifierParser::XmlCaseModifierParser()
: XmlModifierParser(s_sModifierCaseNodeName)
{
}

unique_ptr<StdThemeModifier> XmlCaseModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <SelectCase idAss="Imgs">
	//     <SelectedImage/>
	// </SelectCase>
	oCtx.addChecker(p0Element);
	oCtx.pushCtx(Util::stringCompose("Modifier '%1': ", s_sModifierCaseNodeName));
	//
	const auto oPairIdAss = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierCaseAssIdAttr);
	if (!oPairIdAss.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sModifierCaseAssIdAttr);
	}
	const std::string& sIdAss = oPairIdAss.second;
	if (sIdAss.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sModifierCaseAssIdAttr);
	}
	if (!oCtx.theme().hasAssignId(sIdAss)) {
		throw XmlCommonErrors::errorAttrWithValueNotDefined(oCtx, p0Element, s_sModifierCaseAssIdAttr, sIdAss);
	}
	const int32_t nAssId = oCtx.theme().getAssignId(sIdAss);
	assert(nAssId >= 0);
	//
	auto refCaseModifier = std::make_unique<CaseModifier>(&oCtx.theme(), nAssId);
	auto aModifiers = parseSubModifiers(oCtx, p0Element);
	refCaseModifier->addSubModifiers(std::move(aModifiers));
	oCtx.popCtx();
	// child elements already checked by parseModifiers
	oCtx.removeChecker(p0Element, false, true);
	return refCaseModifier;
}


} // namespace stmg

