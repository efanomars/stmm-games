/*
 * Copyright © 2020  Stefano Marsili, <stemars@gmx.ch>
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
 * File:   xmlselectvarmodifier.cc
 */

#include "modifiers/xmlselectvarmodifier.h"

#include "themectx.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games-gtk/modifiers/selectvarmodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

//#include <cassert>
#include <string>
#include <utility>
#include <limits>

namespace stmg
{

static const std::string s_sModifierSelectVarNodeName = "SelectVar";
static const std::string s_sModifierSelectVarVarNameAttr = "name";
static const std::string s_sModifierSelectVarCaseNodeName = "Case";
static const std::string s_sModifierSelectVarCaseFromAttr = "from";
static const std::string s_sModifierSelectVarCaseToAttr = "to";
static const std::string s_sModifierSelectVarDefaultNodeName = "Default";

XmlSelectVarModifierParser::XmlSelectVarModifierParser()
: XmlModifierParser(s_sModifierSelectVarNodeName)
{
}

unique_ptr<StdThemeModifier> XmlSelectVarModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <SelectVar name="Points">
	//     <Case from="1000" to="2000">
	//         ...
	//     </Case>
	//     <Default>
	//         ...
	//     </Default>
	// </Select>
	oCtx.addChecker(p0Element);
	oCtx.pushCtx(Util::stringCompose("Modifier '%1': ", s_sModifierSelectVarNodeName));
	//
	const auto oPairVarName = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectVarVarNameAttr);
	if (!oPairVarName.first) {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0Element, s_sModifierSelectVarVarNameAttr);
	}
	const std::string& sVarName = oPairVarName.second;

	auto refSelectVarModifier = std::make_unique<SelectVarModifier>(&oCtx.theme(), sVarName);
	enum PAPRO
	{
		PAPRO_CASE = 0
		, PAPRO_CASE_DEFAULT = 1
		, PAPRO_END = 2
	};
	int32_t nFrom;
	int32_t nTo;
//std::cout << "------------------>" << '\n';
	PAPRO ePaPro = PAPRO_CASE;
	getXmlConditionalParser()->visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0ModifierElement)
	{
		oCtx.addChecker(p0ModifierElement);
		const std::string sModifierName = p0ModifierElement->get_name();
		if (sModifierName == s_sModifierSelectVarCaseNodeName) {
			if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("Element '%1' must follow all '%2'"
											, s_sModifierSelectVarDefaultNodeName, s_sModifierSelectVarCaseNodeName));
			}
			parseModifierSelectVarCase(oCtx, p0ModifierElement, nFrom, nTo);
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectVarModifier->addCase(nFrom, nTo, std::move(aModifiers));
			ePaPro = PAPRO_CASE_DEFAULT;
			//
		} else if (sModifierName == s_sModifierSelectVarDefaultNodeName) {
			if (ePaPro == PAPRO_CASE) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
											, s_sModifierSelectVarDefaultNodeName, s_sModifierSelectVarCaseNodeName));
			} else if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0ModifierElement, s_sModifierSelectVarDefaultNodeName);
			}
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectVarModifier->setDefault(std::move(aModifiers));
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
	return refSelectVarModifier;
}
void XmlSelectVarModifierParser::parseModifierSelectVarCase(ThemeCtx& oCtx, const xmlpp::Element* p0Element
															, int32_t& nFrom, int32_t& nTo)
{
	nFrom = std::numeric_limits<int32_t>::lowest();
	nTo = std::numeric_limits<int32_t>::max();
	const auto oPairFrom = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectVarCaseFromAttr);
	const bool bFromDefined = oPairFrom.first;
	if (bFromDefined) {
		const std::string& sFrom = oPairFrom.second;
		nFrom = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sModifierSelectVarCaseFromAttr, sFrom, false
															, false, -1, false, -1);
	}
	const auto oPairTo = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectVarCaseToAttr);
	const bool bToDefined = oPairTo.first;
	if (bToDefined) {
		const std::string& sTo = oPairTo.second;
		nTo = XmlUtil::strToNumber<int32_t>(oCtx, p0Element, s_sModifierSelectVarCaseToAttr, sTo, false
															, false, nFrom, false, -1);
	}
	assert(! ((nFrom > nTo) && (bFromDefined || bToDefined)));
	if (! (bFromDefined || bToDefined)) {
		// undefined
		nFrom = 0;
		nTo = -1;
	}
}

} // namespace stmg

