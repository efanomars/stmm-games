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
 * File:   xmlvolatilewidget.cc
 */

#include "gamewidgets/xmlvolatilewidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlimageparser.h>

#include <stmm-games/util/intset.h>
#include <stmm-games/widgets/volatilewidget.h>
#include <stmm-games/util/util.h>

#include <cassert>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{

static const std::string s_sGameVolatileWidgetNodeName = "VolatileWidget";
static const std::string s_sGameVolatileWidgetVariableAttr = "variable";
static const std::string s_sGameVolatileWidgetSelectNodeName = "SelectImg";
static const std::string s_sGameVolatileWidgetSelectDefaultImgAttr = "defaultImgId";
static const std::string s_sGameVolatileWidgetSelectCaseNodeName = "Case";
static const std::string s_sGameVolatileWidgetSelectCaseValueAttr = "value";
static const std::string s_sGameVolatileWidgetSelectCaseFromValueAttr = "fromValue";
static const std::string s_sGameVolatileWidgetSelectCaseToValueAttr = "toValue";
static const std::string s_sGameVolatileWidgetSelectCaseValuesAttr = "values";
//static const std::string s_sGameVolatileWidgetSelectCaseImgIdAttr = "imgId";


XmlVolatileWidgetParser::XmlVolatileWidgetParser()
: XmlGameWidgetParser(s_sGameVolatileWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlVolatileWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlVolatileWidgetParser::parseGameWidget A" << '\n';
	oCtx.addChecker(p0WidgetElement);
	VolatileWidget::Init oInit;
	parseContainerWidget(oInit, oCtx, p0WidgetElement, {&s_sGameVolatileWidgetSelectNodeName});
	if (oInit.m_aChildWidgets.size() != 1) {
		throw XmlCommonErrors::error(oCtx, p0WidgetElement, s_sGameVolatileWidgetNodeName, Util::stringCompose(
																			"Volatile widget must have exactly one child widget"));
	}
	//
	const auto oPairVariable = parseVariable(oCtx, p0WidgetElement, s_sGameVolatileWidgetVariableAttr, true);
	oInit.m_nVarId = oPairVariable.first;
	oInit.m_eVarOwnerType = oPairVariable.second;

	const xmlpp::Element* p0Select = getXmlConditionalParser().parseUniqueElement(oCtx, p0WidgetElement, s_sGameVolatileWidgetSelectNodeName, true);
	assert(p0Select != nullptr);
	parseSelect(oInit, oCtx, p0Select);

	oCtx.removeChecker(p0WidgetElement, false, true);
	return std::make_shared<VolatileWidget>(std::move(oInit));
}
void XmlVolatileWidgetParser::parseSelect(VolatileWidget::Init& oInit, LayoutCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0Element, s_sGameVolatileWidgetSelectCaseNodeName
														, [&](const xmlpp::Element* p0CaseElement)
	{
		auto oValueImg = parseCase(oCtx, p0CaseElement);
		oInit.m_aValueImgs.emplace_back(std::move(oValueImg));
	});
	if (oInit.m_aValueImgs.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0Element, s_sGameVolatileWidgetSelectCaseNodeName);
	}
	//
	oInit.m_nDefaultImgId = getXmlImageParser().parseImageId(oCtx, p0Element, s_sGameVolatileWidgetSelectDefaultImgAttr, false);
	//
	oCtx.removeChecker(p0Element, true);
}
VolatileWidget::ValueImg XmlVolatileWidgetParser::parseCase(LayoutCtx& oCtx, const xmlpp::Element* p0Element)
{
	oCtx.addChecker(p0Element);
	//
	VolatileWidget::ValueImg oValueImg;
	auto oPairIntSet = getXmlConditionalParser().parseIntSetAttributes(oCtx, p0Element, s_sGameVolatileWidgetSelectCaseValueAttr
																		, s_sGameVolatileWidgetSelectCaseFromValueAttr, s_sGameVolatileWidgetSelectCaseToValueAttr
																		, s_sGameVolatileWidgetSelectCaseValuesAttr, Util::s_sEmptyString
																		, false, -1, false, -1);
	if (! oPairIntSet.first) {
		throw XmlCommonErrors::errorAttrEitherMustBeDefined(oCtx, p0Element, s_sGameVolatileWidgetSelectCaseValueAttr
															, s_sGameVolatileWidgetSelectCaseFromValueAttr, s_sGameVolatileWidgetSelectCaseToValueAttr
															, s_sGameVolatileWidgetSelectCaseValuesAttr);
	}
	oValueImg.m_oValues = std::move(oPairIntSet.second);
	if (oValueImg.m_oValues.size() == 0) {
		throw XmlCommonErrors::error(oCtx, p0Element, Util::s_sEmptyString, Util::stringCompose(
														"Set defined by attribute '%1' and others cannot be empty", s_sGameVolatileWidgetSelectCaseValuesAttr));
	}
	oValueImg.m_nImgId = getXmlImageParser().parseImageId(oCtx, p0Element, false);
	//
	oCtx.removeChecker(p0Element, true);
	return oValueImg;
}

} // namespace stmg
