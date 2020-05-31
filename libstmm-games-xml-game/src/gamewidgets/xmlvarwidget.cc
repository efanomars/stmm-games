/*
 * File:   xmlvarwidget.cc
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

#include "gamewidgets/xmlvarwidget.h"

#include "layoutctx.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/gamewidget.h>
#include <stmm-games/widgets/varwidget.h>

#include <array>
#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGameVarWidgetNodeName = "VarWidget";
static const std::string s_sGameVarWidgetTitleAttr = "title";
static const std::string s_sGameVarWidgetTitleFactorAttr = "titleFactor";
static const std::string s_sGameVarWidgetTitleBesideValueAttr = "titleBesideValue";
static const std::string s_sGameVarWidgetVariableAttr = "variable";
static const std::string s_sGameVarWidgetValueDigitsAttr = "valueDigits";

static const std::string s_sGameVarWidgetTitleAlignAttr = "titleAlign";
static const std::string s_sGameVarWidgetValueAlignAttr = "valueAlign";
	static const std::string s_sGameVarWidgetAlignAttrFill = "FILL";
	static const std::string s_sGameVarWidgetAlignAttrStart = "START";
	static const std::string s_sGameVarWidgetAlignAttrCenter = "CENTER";
	static const std::string s_sGameVarWidgetAlignAttrEnd = "END";


XmlVarWidgetParser::XmlVarWidgetParser()
: XmlGameWidgetParser(s_sGameVarWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlVarWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlVarWidgetParser::parseGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	VarWidget::Init oInit;
	parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);
	//
	const auto oPairTitle = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameVarWidgetTitleAttr);
	if (oPairTitle.first) {
		const std::string& sTitle = oPairTitle.second;
		oInit.m_sTitle = sTitle;
	}
	const auto oPairTitleFactor = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameVarWidgetTitleFactorAttr);
	if (oPairTitleFactor.first) {
		const std::string& sTitleFactor = oPairTitleFactor.second;
		const double fFactor = XmlUtil::strToNumber<double>(oCtx, p0WidgetElement, s_sGameVarWidgetTitleFactorAttr, sTitleFactor
																	, true, true, 0.1, false, -1.0);
		oInit.m_nTitleFactorPerc = 100.0 * fFactor;
	}
	const auto oPairTitleBesideValue = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameVarWidgetTitleBesideValueAttr);
	if (oPairTitleBesideValue.first) {
		const std::string& sTitleBesideValue = oPairTitleBesideValue.second;
		oInit.m_bTitleBesideValue = XmlUtil::strToBool(oCtx, p0WidgetElement, s_sGameVarWidgetTitleBesideValueAttr, sTitleBesideValue);
	}
	//
	const auto oPairVariable = parseVariable(oCtx, p0WidgetElement, s_sGameVarWidgetVariableAttr, true);
	oInit.m_nVarId = oPairVariable.first;
	oInit.m_eVarOwnerType = oPairVariable.second;

	const auto oPairValueDigits = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameVarWidgetValueDigitsAttr);
	if (oPairValueDigits.first) {
		const std::string& sValueDigits = oPairValueDigits.second;
		oInit.m_nVarDigits = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGameVarWidgetValueDigitsAttr, sValueDigits
																	, false, true, 1, false, -1);
	}
	//
	static const std::vector<char const *> s_aAlignEnumString{s_sGameVarWidgetAlignAttrFill.c_str(), s_sGameVarWidgetAlignAttrStart.c_str(), s_sGameVarWidgetAlignAttrCenter.c_str(), s_sGameVarWidgetAlignAttrEnd.c_str()};
	static const std::vector<WidgetAxisFit::ALIGN> s_aAlignEnumValue{WidgetAxisFit::ALIGN_FILL, WidgetAxisFit::ALIGN_START, WidgetAxisFit::ALIGN_CENTER, WidgetAxisFit::ALIGN_END};
	//
	getXmlConditionalParser().visitLayoutConfigNamedEnumAttributes(oCtx, p0WidgetElement, s_sGameVarWidgetTitleAlignAttr, g_nTotLayoutConfigs, s_aAlignEnumString
																, [&](const std::string& /*sLCAttrName*/, int32_t nEnum, int32_t nLayoutConfig)
	{
		const WidgetAxisFit::ALIGN eAlign = s_aAlignEnumValue[nEnum];
		if (nLayoutConfig < 0) {
			oInit.setTitleAlign(eAlign);
		} else {
			oInit.m_aTitleAlign[nLayoutConfig] = eAlign;
		}
	});
	//
	getXmlConditionalParser().visitLayoutConfigNamedEnumAttributes(oCtx, p0WidgetElement, s_sGameVarWidgetValueAlignAttr, g_nTotLayoutConfigs, s_aAlignEnumString
																, [&](const std::string& /*sLCAttrName*/, int32_t nEnum, int32_t nLayoutConfig)
	{
		const WidgetAxisFit::ALIGN eAlign = s_aAlignEnumValue[nEnum];
		if (nLayoutConfig < 0) {
			oInit.setValueAlign(eAlign);
		} else {
			oInit.m_aValueAlign[nLayoutConfig] = eAlign;
		}
	});

	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<VarWidget>(std::move(oInit));
}

} // namespace stmg
