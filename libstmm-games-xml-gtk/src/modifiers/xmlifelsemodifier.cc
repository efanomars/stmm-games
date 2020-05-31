/*
 * File:   xmlifelsemodifier.cc
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

#include "modifiers/xmlifelsemodifier.h"

#include <stmm-games-xml-base/xmltraitsparser.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "themectx.h"
#include "xmlcommonerrors.h"

#include <stmm-games-gtk/modifiers/ifelsemodifier.h>
#include <stmm-games-gtk/stdthememodifier.h>

#include <stmm-games/util/util.h>
#include <stmm-games/utile/tileselector.h>

#include <glibmm/ustring.h>
#include <libxml++/libxml++.h>

#include <cassert>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class StdTheme; }

namespace stmg
{

static const std::string s_sModifierIfElseNodeName = "Conditional";
static const std::string s_sModifierIfElseIfConditionNodeName = "If";
static const std::string s_sModifierIfElseElsifConditionNodeName = "Elsif";
static const std::string s_sModifierIfElseElseConditionNodeName = "Else";

XmlIfElseModifierParser::XmlIfElseModifierParser()
: XmlModifierParser(s_sModifierIfElseNodeName)
{
}

unique_ptr<StdThemeModifier> XmlIfElseModifierParser::parseModifier(ThemeCtx& oCtx, const xmlpp::Element* p0Element)
{
	// Typical usage:
	// <Conditional>
	//     <If><Char charNames="SWAPPER:LEFT,SWAPPER:RIGHT,SQUARSOR:SEL"/></If>
	//         ...
	//     <Elsif><Char charName="BOMB"/></Elsif>
	//         ...
	//     <Else/>
	//         ...
	// </Conditional>

	oCtx.addChecker(p0Element);
	StdTheme& oTheme = oCtx.theme();
	//
	oCtx.pushCtx("Modifier '" + s_sModifierIfElseNodeName + "'");
	//
	auto refIfElseModifier = std::make_unique<IfElseModifier>(&oTheme);
	enum PAPRO
	{
		PAPRO_COND = 0
		, PAPRO_ELSIF = 1
		, PAPRO_ELSE = 2
	};
	unique_ptr<TileSelector> refCondition;
	std::vector< unique_ptr<StdThemeModifier> > aModifiers;

	PAPRO ePaPro = PAPRO_COND;
	getXmlConditionalParser()->visitElementChildren(oCtx, p0Element, [&](const xmlpp::Element* p0ModifierElement)
	{
		const std::string sModifierName = p0ModifierElement->get_name();
//std::cout << "-------> sModifierName=" << sModifierName << '\n';
		if (sModifierName == s_sModifierIfElseIfConditionNodeName) {
			if (ePaPro != PAPRO_COND) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0Element, s_sModifierIfElseIfConditionNodeName);
			}
			refCondition = getXmlTraitsParser()->parseTileSelector(oCtx, p0ModifierElement);
			ePaPro = PAPRO_ELSIF;
			//
		} else if (sModifierName == s_sModifierIfElseElsifConditionNodeName) {
			if (ePaPro == PAPRO_COND) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
							, s_sModifierIfElseElsifConditionNodeName, s_sModifierIfElseIfConditionNodeName));
			} else if (ePaPro == PAPRO_ELSE) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' cannot follow '%2'"
								, s_sModifierIfElseElsifConditionNodeName, s_sModifierIfElseElseConditionNodeName));
			}
			refIfElseModifier->addCondition(std::move(refCondition), std::move(aModifiers));
			refCondition = getXmlTraitsParser()->parseTileSelector(oCtx, p0ModifierElement);
			aModifiers.clear();
			//
		} else if (sModifierName == s_sModifierIfElseElseConditionNodeName) {
			if (ePaPro == PAPRO_COND) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
								, s_sModifierIfElseElseConditionNodeName, s_sModifierIfElseIfConditionNodeName));
			} else if (ePaPro == PAPRO_ELSE) {
				throw XmlCommonErrors::errorElementTooMany(oCtx, p0Element, s_sModifierIfElseElseConditionNodeName);
			}
			refIfElseModifier->addCondition(std::move(refCondition), std::move(aModifiers));
			refCondition.reset();
			aModifiers.clear();
			ePaPro = PAPRO_ELSE;
			//
		} else {
			if (ePaPro == PAPRO_COND) {
				throw XmlCommonErrors::error(oCtx, p0ModifierElement, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
								, sModifierName, s_sModifierIfElseIfConditionNodeName));
			}
			auto refSubModifier = parseSubModifier(oCtx, p0ModifierElement);
			if (!refSubModifier) {
				throw XmlCommonErrors::errorElementInvalid(oCtx, p0ModifierElement, sModifierName);
			}
			aModifiers.push_back(std::move(refSubModifier));
		}
	});
	if (ePaPro == PAPRO_COND) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose("'%1' without '%2'"
									, s_sModifierIfElseNodeName, s_sModifierIfElseIfConditionNodeName));
	}
	if (!aModifiers.empty()) {
		if (ePaPro == PAPRO_ELSIF) {
			refIfElseModifier->addCondition(std::move(refCondition), std::move(aModifiers));
		} else {
			assert(ePaPro == PAPRO_ELSE);
			refIfElseModifier->setElse(std::move(aModifiers));
		}
	}
	oCtx.popCtx();
	oCtx.removeChecker(p0Element, false, true);
	return refIfElseModifier;
}

} // namespace stmg

