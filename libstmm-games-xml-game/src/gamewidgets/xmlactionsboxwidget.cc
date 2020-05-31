/*
 * File:   xmlactionsboxwidget.cc
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

#include "gamewidgets/xmlactionsboxwidget.h"

#include "gamewidgets/xmlactionbasewidget.h"
#include "gamewidgets/xmlboxwidget.h"
#include "layoutctx.h"
#include "xmlcommonerrors.h"
#include <stmm-games-xml-base/xmlconditionalparser.h>
#include "xmlutil/xmlstrconv.h"

#include <stmm-games/widgets/actionwidget.h>
#include <stmm-games/util/util.h>
#include <stmm-games/widgets/actionsboxwidget.h>

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace stmg { class GameWidget; }
namespace xmlpp { class Element; }

namespace stmg
{


static const std::string s_sGameActionsBoxWidgetNodeName = "ActionsBoxWidget";
static const std::string s_sGameActionsBoxWidgetGridRowNodeName = "GridRow";
static const std::string s_sGameActionsBoxWidgetGridRowWeightAttr = "weight";
static const std::string s_sGameActionsBoxWidgetGridRowCellNodeName = "Cell";
static const std::string s_sGameActionsBoxWidgetGridRowCellWeightAttr = "weight";
//static const std::string s_sGameActionsBoxWidgetGridRowKANamesAttr = "names";
//static const std::string s_sGameActionsBoxWidgetGridRowRowPartAttr = "rowPart";
static std::vector<std::string const*> s_aGameActionsBoxWidgetChildElements{&s_sGameActionsBoxWidgetGridRowNodeName};

XmlActionsBoxWidgetParser::XmlActionsBoxWidgetParser()
: XmlActionBaseWidgetParser<XmlBoxWidgetParser>(s_sGameActionsBoxWidgetNodeName)
{
}
shared_ptr<GameWidget> XmlActionsBoxWidgetParser::parseGameWidget(LayoutCtx& oCtx, const xmlpp::Element* p0WidgetElement)
{
//std::cout << "XmlActionsBoxWidgetParser::parseGameWidget" << '\n';
	oCtx.addChecker(p0WidgetElement);
	ActionsBoxWidget::Init oInit;
	parseBoxWidget(oInit, oCtx, p0WidgetElement, s_aGameActionsBoxWidgetChildElements);

	int32_t nGridCols = 0;
	int32_t nGridRows = 0;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0WidgetElement, s_sGameActionsBoxWidgetGridRowNodeName, [&](const xmlpp::Element* p0ChildElement)
	{
		oCtx.addChecker(p0ChildElement);
		//
		const auto oPairRowPart = getXmlConditionalParser().getAttributeValue(oCtx, p0ChildElement, s_sGameActionsBoxWidgetGridRowWeightAttr);
		if (!oPairRowPart.first) {
			throw XmlCommonErrors::errorAttrNotFound(oCtx, p0ChildElement, s_sGameActionsBoxWidgetGridRowWeightAttr);
		}
		const std::string& sRowPart = oPairRowPart.second;
		const int32_t nRowPart = XmlUtil::strToNumber<int32_t>(oCtx, p0ChildElement, s_sGameActionsBoxWidgetGridRowWeightAttr, sRowPart
																		, false, true, 1, false, -1);
		oInit.m_aRows.push_back(nRowPart);

		nGridCols = parseActionsBoxRow(oCtx, p0ChildElement, oInit, ((nGridRows == 0) ? -1 : nGridCols));
		//
		++nGridRows;
		//
		oCtx.removeChecker(p0ChildElement, true);
	});
	if (oInit.m_aRows.empty()) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0WidgetElement, s_sGameActionsBoxWidgetGridRowNodeName);
	}
	// Child widget names are checked in parseBoxWidget
	oCtx.removeChecker(p0WidgetElement, false, true);
	return std::make_shared<ActionsBoxWidget>(std::move(oInit));
}
int32_t XmlActionsBoxWidgetParser::parseActionsBoxRow(LayoutCtx& oCtx, const xmlpp::Element* p0RowElement
													, ActionsBoxWidget::Init& oInit, int32_t nTotColumns)
{
	oCtx.addChecker(p0RowElement);

	const bool bFirstRow = (nTotColumns <= 0);
	int32_t nGridCols = 0;
	getXmlConditionalParser().visitNamedElementChildren(oCtx, p0RowElement, s_sGameActionsBoxWidgetGridRowCellNodeName, [&](const xmlpp::Element* p0CellElement)
	{
		oCtx.addChecker(p0CellElement);
		if (bFirstRow) {
			const auto oPairColumnPart = getXmlConditionalParser().getAttributeValue(oCtx, p0CellElement, s_sGameActionsBoxWidgetGridRowCellWeightAttr);
			if (!oPairColumnPart.first) {
				throw XmlCommonErrors::errorAttrNotFound(oCtx, p0CellElement, s_sGameActionsBoxWidgetGridRowCellWeightAttr);
			}
			const std::string& sColumnPart = oPairColumnPart.second;
			const int32_t nColumnPart = XmlUtil::strToNumber<int32_t>(oCtx, p0CellElement, s_sGameActionsBoxWidgetGridRowCellWeightAttr, sColumnPart
																			, false, true, 1, false, -1);
			oInit.m_aColumns.push_back(nColumnPart);
		} else {
			if (nGridCols >= nTotColumns) {
				throw XmlCommonErrors::error(oCtx, p0CellElement, Util::s_sEmptyString
												, Util::stringCompose("Row has too many elements '%1'"
																	, s_sGameActionsBoxWidgetGridRowCellNodeName));
			}
		}
		oInit.m_aActionCells.push_back(ActionsBoxWidget::BoxCell{});
		ActionsBoxWidget::BoxCell& oCell = oInit.m_aActionCells.back();
		XmlAction oAction = parseActionAttr(oCtx, p0CellElement, false);
		const bool bDefined = oAction.m_bDefined;
		if (!bDefined) {
			oCell.m_eActionType = ActionsBoxWidget::ACTION_NONE;
		} else {
			const ActionWidget::ACTION_TYPE eWAT = oAction.m_eActionType;
			oCell.m_nActionId = oAction.m_nActionId;
			oCell.m_sActionName = std::move(oAction.m_sActionName);
			oCell.m_nValue = oAction.m_nValue;
			static_assert(static_cast<int32_t>(ActionWidget::ACTION_KEY_ACTION) == static_cast<int32_t>(ActionsBoxWidget::ACTION_KEY_ACTION), "");
			static_assert(static_cast<int32_t>(ActionWidget::ACTION_INTERRUPT) == static_cast<int32_t>(ActionsBoxWidget::ACTION_INTERRUPT), "");
			static_assert(static_cast<int32_t>(ActionWidget::ACTION_EVENT_MSG) == static_cast<int32_t>(ActionsBoxWidget::ACTION_EVENT_MSG), "");
			oCell.m_eActionType = static_cast<ActionsBoxWidget::ACTION_TYPE>(eWAT);
		}
		++nGridCols;
		oCtx.removeChecker(p0CellElement, true);
	});
	if (bFirstRow && (nGridCols == 0)) {
		throw XmlCommonErrors::errorElementExpected(oCtx, p0RowElement, s_sGameActionsBoxWidgetGridRowCellNodeName);
	}
	if (!bFirstRow) {
		while (nGridCols < nTotColumns) {
			oInit.m_aActionCells.push_back(ActionsBoxWidget::BoxCell{});
			ActionsBoxWidget::BoxCell& oCell = oInit.m_aActionCells.back();
			oCell.m_eActionType = ActionsBoxWidget::ACTION_NONE;
			++nGridCols;
		}
	}

	oCtx.removeChecker(p0RowElement, true);
	return nGridCols;
}

} // namespace stmg
