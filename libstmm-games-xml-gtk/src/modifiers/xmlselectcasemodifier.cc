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
 * File:   xmlselectcasemodifier.cc
 */

#include "modifiers/xmlselectcasemodifier.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>

#include <stmm-games-gtk/modifiers/selectcasemodifier.h>
#include <stmm-games-gtk/stdtheme.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <cassert>
#include <string>
#include <utility>

namespace stmg
{

static const std::string s_sModifierSelectCaseNodeName = "Select";
static const std::string s_sModifierSelectCaseCaseNodeName = "Case";
static const std::string s_sModifierSelectCaseCaseAssIdAttr = "idAss";
static const std::string s_sModifierSelectCaseDefaultNodeName = "Default";

XmlSelectCaseModifierParser::XmlSelectCaseModifierParser()
: XmlModifierParser(s_sModifierSelectCaseNodeName)
{
}

unique_ptr<StdThemeModifier> XmlSelectCaseModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <Select>
	//     <Case idAss="swapperLP">
	//         <SelectedImage/>
	//     </Case>
	//     <Case idAss="swapperRP">
	//         <SelectedImage/>
	//     </Case>
	//     <Case idAss="squarsorP">
	//         <SelectedImage/>
	//     </Case>
	//     <Default>
	//         ......
	//     </Default>
	// </Select>
	oCtx.addChecker(p0Element);
	oCtx.pushCtx(Util::stringCompose("Modifier '%1': ", s_sModifierSelectCaseNodeName));
	//
	auto refSelectCaseModifier = std::make_unique<SelectCaseModifier>(&oCtx.theme());
	enum PAPRO
	{
		PAPRO_CASE = 0
		, PAPRO_CASE_DEFAULT = 1
		, PAPRO_END = 2
	};
	int32_t nAssId;
//std::cout << "------------------>" << '\n';
	PAPRO ePaPro = PAPRO_CASE;
	getXmlConditionalParser()->visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0ModifierElement)
	{
		oCtx.addChecker(p0ModifierElement);
		const std::string sModifierName = p0ModifierElement->get_name();
		if (sModifierName == s_sModifierSelectCaseCaseNodeName) {
			if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("Element '%1' must follow all '%2'"
											, s_sModifierSelectCaseDefaultNodeName, s_sModifierSelectCaseCaseNodeName));
			}
			parseModifierSelectCaseCase(oCtx, p0ModifierElement, nAssId);
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectCaseModifier->addCase(nAssId, std::move(aModifiers));
			ePaPro = PAPRO_CASE_DEFAULT;
			//
		} else if (sModifierName == s_sModifierSelectCaseDefaultNodeName) {
			if (ePaPro == PAPRO_CASE) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
											, s_sModifierSelectCaseDefaultNodeName, s_sModifierSelectCaseCaseNodeName));
			} else if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0ModifierElement, s_sModifierSelectCaseDefaultNodeName);
			}
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectCaseModifier->setDefault(std::move(aModifiers));
			ePaPro = PAPRO_END;
			//
		} else {
			throw XmlCommonErrors::errorElementInvalid(oCtx, p0ModifierElement, sModifierName);
		}
		// Child elements already checked in parseModifiers
		oCtx.removeChecker(p0ModifierElement, false, true);
	});
	oCtx.popCtx();
	// Child elements already checked in visitElementChildren
	oCtx.removeChecker(p0Element, false, true);
	return refSelectCaseModifier;
}
void XmlSelectCaseModifierParser::parseModifierSelectCaseCase(ThemeCtx& oCtx, const xmlpp::Element* p0Element, int32_t& nAssId)
{
	const auto oPairIdAss = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectCaseCaseAssIdAttr);
	if (!oPairIdAss.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sModifierSelectCaseCaseAssIdAttr);
	}
	const std::string& sIdAss = oPairIdAss.second;
	if (sIdAss.empty()) {
		throw XmlCommonErrors::errorAttrCannotBeEmpty(oCtx, p0Element, s_sModifierSelectCaseCaseAssIdAttr);
	}
	if (!oCtx.theme().hasAssignId(sIdAss)) {
		throw XmlCommonErrors::errorAttrWithValueNotDefined(oCtx, p0Element, s_sModifierSelectCaseCaseAssIdAttr, sIdAss);
	}
	nAssId = oCtx.theme().getAssignId(sIdAss);
	assert(nAssId >= 0);
}

} // namespace stmg

