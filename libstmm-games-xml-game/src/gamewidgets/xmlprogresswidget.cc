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
 * File:   xmlprogresswidget.cc
 */

#include "gamewidgets/xmlprogresswidget.h"

#include "layoutctx.h"

#include <stmm-games-xml-base/xmlcommonerrors.h>
#include <stmm-games-xml-base/xmlcommonparser.h>
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include <stmm-games-xml-base/xmlutil/xmlstrconv.h>

#include <stmm-games/util/intset.h>
#include <stmm-games/util/namedobjindex.h>
#include <stmm-games/util/util.h>
#include <stmm-games/widgets/progresswidget.h>

#include <cassert>
#include <cstdint>
#include <string>
#include <utility>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }


namespace stmg
{


static const std::string s_sGameProgressWidgetNodeName = "ProgressWidget";
static const std::string s_sGameProgressWidgetMinValueAttr = "minValue";
static const std::string s_sGameProgressWidgetMaxValueAttr = "maxValue";
static const std::string s_sGameProgressWidgetThresholdValueAttr = "thresholdValue";
static const std::string s_sGameProgressWidgetDangerBelowAttr = "dangerBelow";
static const std::string s_sGameProgressWidgetVariableAttr = "variable";
static const std::string s_sGameProgressWidgetMarkAttr = "mark";
static const std::string s_sGameProgressWidgetMarksAttr = "marks";
static const std::string s_sGameProgressWidgetMarksFromAttr = "marksFrom";
static const std::string s_sGameProgressWidgetMarksToAttr = "marksTo";
static const std::string s_sGameProgressWidgetMarksStepAttr = "marksStep";


XmlProgressWidgetParser::XmlProgressWidgetParser()
: XmlGameWidgetParser(s_sGameProgressWidgetNodeName)
{
}

shared_ptr<GameWidget> XmlProgressWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
	oCtx.addChecker(p0WidgetElement);
	ProgressWidget::Init oInit;
	parseRelSizedGameWidget(oInit, oCtx, p0WidgetElement);
	//
	const auto oPairMinValue = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameProgressWidgetMinValueAttr);
	if (oPairMinValue.first) {
		const std::string& sMinValue = oPairMinValue.second;
		oInit.m_nMinValue = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGameProgressWidgetMinValueAttr, sMinValue
																, false, false, -1, false, -1);
	}
	const auto oPairMaxValue = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameProgressWidgetMaxValueAttr);
	if (oPairMaxValue.first) {
		const std::string& sMaxValue = oPairMaxValue.second;
		oInit.m_nMaxValue = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGameProgressWidgetMaxValueAttr, sMaxValue
																, false, false, -1, false, -1);
	}
	const auto oPairThresholdValue = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameProgressWidgetThresholdValueAttr);
	if (oPairThresholdValue.first) {
		const std::string& sThresholdValue = oPairThresholdValue.second;
		oInit.m_nThresholdValue = XmlUtil::strToNumber<int32_t>(oCtx, p0WidgetElement, s_sGameProgressWidgetThresholdValueAttr, sThresholdValue
																, false, false, -1, false, -1);
	}
	const auto oPairDangerBelow = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameProgressWidgetDangerBelowAttr);
	if (oPairDangerBelow.first) {
		const std::string& sDangerBelow = oPairDangerBelow.second;
		oInit.m_bDangerBelow = XmlUtil::strToBool(oCtx, p0WidgetElement, s_sGameProgressWidgetDangerBelowAttr, sDangerBelow);
	}
	//
	const auto oPairVariableName = getXmlConditionalParser().getAttributeValue(oCtx, p0WidgetElement, s_sGameProgressWidgetVariableAttr);
	if (oPairVariableName.first) {
		const std::string& sVariableName = oPairVariableName.second;
		std::string sVarType;
		if (oInit.m_nTeam < 0) {
			assert(oInit.m_nMate < 0);
			oInit.m_nVariableId = oCtx.getGameVariableTypes().getIndex(sVariableName);
			sVarType = "Game";
		} else if (oInit.m_nMate < 0) {
			oInit.m_nVariableId = oCtx.getTeamVariableTypes().getIndex(sVariableName);
			sVarType = "Team";
		} else {
			oInit.m_nVariableId = oCtx.getMateVariableTypes().getIndex(sVariableName);
			sVarType = "Mate";
		}
//std::cout << "XmlProgressWidgetParser::parseGameWidget   sVariableName=" << sVariableName << " sVarType=" << sVarType << "  oInit.m_nVarId=" << oInit.m_nVarId << '\n';
		if (oInit.m_nVariableId < 0) {
			throw XmlCommonErrors::error(oCtx, p0WidgetElement, s_sGameProgressWidgetVariableAttr
										, Util::stringCompose("%1 variable named '%2' not found", sVarType, sVariableName));
		}
	} else {
		throw XmlCommonErrors::errorAttrNotFound(oCtx, p0WidgetElement, s_sGameProgressWidgetVariableAttr);
	}
	auto oMarksPair = XmlCommonParser::parseIntSetAttrs(oCtx, p0WidgetElement, s_sGameProgressWidgetMarkAttr
														, s_sGameProgressWidgetMarksFromAttr, s_sGameProgressWidgetMarksToAttr, s_sGameProgressWidgetMarksStepAttr
														, s_sGameProgressWidgetMarksAttr, ""
														, true, oInit.m_nMinValue, true, oInit.m_nMaxValue);
	if (oMarksPair.first) {
		oInit.m_oMarks = std::move(oMarksPair.second);
	}
//std::cout << "XmlProgressWidgetParser::parseGameWidget  oInit.m_nTeam=" << oInit.m_nTeam << "  oInit.m_nMate=" << oInit.m_nMate << '\n';
	oCtx.removeChecker(p0WidgetElement, true);
	return std::make_shared<ProgressWidget>(std::move(oInit));
}

} // namespace stmg
