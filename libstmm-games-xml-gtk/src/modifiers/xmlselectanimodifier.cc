/*
 * File:   xmlselectanimodifier.cc
 *
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

#include "modifiers/xmlselectanimodifier.h"

#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "themectx.h"
#include "xmlcommonerrors.h"
#include "xmlutil/xmlstrconv.h"

#include <stmm-games-gtk/modifiers/selectanimodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

//#include <cassert>
#include <string>
#include <utility>

namespace stmg
{

static const std::string s_sModifierSelectAniNodeName = "SelectAni";
//static const std::string s_sModifierSelectAniAniIdAttr = "aniName";
static const std::string s_sModifierSelectAniCaseNodeName = "Case";
static const std::string s_sModifierSelectAniCaseFromAttr = "from";
static const std::string s_sModifierSelectAniCaseFromExclAttr = "fromExcl";
static const std::string s_sModifierSelectAniCaseToAttr = "to";
static const std::string s_sModifierSelectAniCaseToExclAttr = "toExcl";
static const std::string s_sModifierSelectAniDefaultNodeName = "Default";

XmlSelectAniModifierParser::XmlSelectAniModifierParser()
: XmlModifierParser(s_sModifierSelectAniNodeName)
{
}

unique_ptr<StdThemeModifier> XmlSelectAniModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <SelectAni aniName="TILEANI:COVER">
	//     <Case from="0" fromExcl="false" to="0.5" toExcl="true">
	//         ...
	//     </Case>
	//     <Default>
	//         ...
	//     </Default>
	// </Select>
	oCtx.addChecker(p0Element);
	oCtx.pushCtx(Util::stringCompose("Modifier '%1': ", s_sModifierSelectAniNodeName));
	//
	const int32_t nAniIdx = parseModifierTileAniName(oCtx, p0Element, true);

	auto refSelectAniModifier = std::make_unique<SelectAniModifier>(&oCtx.theme(), nAniIdx);
	enum PAPRO
	{
		PAPRO_CASE = 0
		, PAPRO_CASE_DEFAULT = 1
		, PAPRO_END = 2
	};
	double fFrom;
	bool bFromExcl;
	double fTo;
	bool bToExcl;
//std::cout << "------------------>" << '\n';
	PAPRO ePaPro = PAPRO_CASE;
	getXmlConditionalParser()->visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0ModifierElement)
	{
		oCtx.addChecker(p0ModifierElement);
		const std::string sModifierName = p0ModifierElement->get_name();
		if (sModifierName == s_sModifierSelectAniCaseNodeName) {
			if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("Element '%1' must follow all '%2'"
											, s_sModifierSelectAniDefaultNodeName, s_sModifierSelectAniCaseNodeName));
			}
			parseModifierSelectAniCase(oCtx, p0ModifierElement, fFrom, bFromExcl, fTo, bToExcl);
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectAniModifier->addCase(fFrom, bFromExcl, fTo, bToExcl, std::move(aModifiers));
			ePaPro = PAPRO_CASE_DEFAULT;
			//
		} else if (sModifierName == s_sModifierSelectAniDefaultNodeName) {
			if (ePaPro == PAPRO_CASE) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
											, s_sModifierSelectAniDefaultNodeName, s_sModifierSelectAniCaseNodeName));
			} else if (ePaPro == PAPRO_END) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0ModifierElement, s_sModifierSelectAniDefaultNodeName);
			}
			auto aModifiers = parseSubModifiers(oCtx, p0ModifierElement);
			refSelectAniModifier->setDefault(std::move(aModifiers));
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
	return refSelectAniModifier;
}
void XmlSelectAniModifierParser::parseModifierSelectAniCase(ThemeCtx& oCtx, const xmlpp::Element* p0Element
															, double& fFrom, bool& bFromExcl, double& fTo, bool& bToExcl)
{
	fFrom = -1.0;
	bFromExcl = false;
	fTo = +1.0;
	bToExcl = false;
	const auto oPairFrom = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectAniCaseFromAttr);
	if (oPairFrom.first) {
		const std::string& sFrom = oPairFrom.second;
		fFrom = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sModifierSelectAniCaseFromAttr, sFrom, true
															, true, -1.0, true, 1.00);
	}
	const auto oPairFromExcl = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectAniCaseFromExclAttr);
	if (oPairFromExcl.first) {
		const std::string& sFromExcl = oPairFromExcl.second;
		bFromExcl = XmlUtil::strToBool(oCtx, p0Element, s_sModifierSelectAniCaseFromExclAttr, sFromExcl);
	}
	const auto oPairTo = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectAniCaseToAttr);
	if (oPairTo.first) {
		const std::string& sTo = oPairTo.second;
		fTo = XmlUtil::strToNumber<double>(oCtx, p0Element, s_sModifierSelectAniCaseToAttr, sTo, true
															, true, fFrom, true, 1.00);
	}
	const auto oPairToExcl = getXmlConditionalParser()->getAttributeValue(oCtx, p0Element, s_sModifierSelectAniCaseToExclAttr);
	if (oPairToExcl.first) {
		const std::string& sToExcl = oPairToExcl.second;
		bToExcl = XmlUtil::strToBool(oCtx, p0Element, s_sModifierSelectAniCaseToExclAttr, sToExcl);
	}
	if ((fFrom == fTo) && bFromExcl && ! bToExcl) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("'%1' and '%2' select empty range"
									, s_sModifierSelectAniCaseToAttr, s_sModifierSelectAniCaseFromAttr));
	}
}

} // namespace stmg

